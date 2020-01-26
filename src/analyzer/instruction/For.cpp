#include "For.hpp"
#include "../instruction/Return.hpp"
#include "../semantic/SemanticAnalyzer.hpp"
#include "../semantic/Variable.hpp"
#include "../value/Value.hpp"
#include "../value/Phi.hpp"
#include "../../colors.h"

namespace ls {

For::For(Environment& env) : Instruction(env) {
	jumping = true;
}

void For::print(std::ostream& os, int indent, PrintOptions options) const {
	// for (const auto& conversion : conversions) {
	// 	os << conversion.first << " " << conversion.first->type << " = (" << conversion.second->type << ") " << conversion.first->parent << " [" << conversion.second << " " << conversion.second->type << "] ";
	// }
	os << "for ";
	if (options.sections) {
		os << std::endl;
	}
	init->print(os, indent, { options.debug, true, options.sections });
	os << " ;" << std::endl;
	if (condition) {
		condition->print(os, indent, { options.debug, true, options.sections });
	}
	os << " ;" << std::endl;
	increment->print(os, indent, { options.debug, true, options.sections });

	os << " ";
	body->print(os, indent, options);

	// if (options.debug) {
	// 	for (const auto& assignment : assignments) {
	// 		os << std::endl << tabs(indent) << assignment.first << " " << assignment.first->type << " = " << assignment.second << " " << assignment.second->type;
	// 	}
	// }
}

Location For::location() const {
	return { token->location.file, token->location.start, body->location().end };
}

void For::pre_analyze(SemanticAnalyzer* analyzer) {

	mutations.clear();
	conversions.clear();
	if (condition) {
		condition->sections.front()->variables.clear();
	}
	increment->sections.back()->variables.clear();
	body->sections.back()->variables.clear();

	init->setup_branch(analyzer);
	analyzer->enter_block(init.get());

	init->variables.clear();
	for (const auto& section : init->sections) {
		section->pre_analyze(analyzer);
	}

	analyzer->enter_loop((Instruction*) this);

	if (condition) {
		condition->pre_analyze(analyzer);
	}

	body->pre_analyze(analyzer);

	if (increment->reachable()) {
		increment->pre_analyze(analyzer);
	}
	analyzer->leave_loop();

	if (mutations.size()) {

		mutations.clear(); // Va être re-rempli par la seconde analyse

		if (condition) {
			condition->pre_analyze(analyzer);
		}

		analyzer->enter_loop((Instruction*) this);

		body->pre_analyze(analyzer);

		if (increment->reachable()) {
			increment->pre_analyze(analyzer);
		}
		analyzer->leave_loop();

		if (condition) {
			for (const auto& phi : condition->sections.front()->phis) {
				// std::cout << "For phi " << phi->variable << std::endl;
				for (const auto& mutation : mutations) {
					// std::cout << "For mutation " << mutation.variable << " " << mutation.section->id << std::endl;
					if (mutation.variable->name == phi->variable2->name) {
						phi->variable2 = mutation.variable;
						// std::cout << "For set var for phi " << phi->variable2 << std::endl;
					}
				}
			}
		}
	}
	analyzer->leave_block(); // init
}

void For::analyze(SemanticAnalyzer* analyzer, const Type* req_type) {
	// std::cout << "For::analyze() " << is_void << std::endl;
	auto& env = analyzer->env;

	if (req_type->is_array()) {
		type = req_type;
	} else {
		type = env.void_;
		body->is_void = true;
	}

	analyzer->enter_block(init.get());
	throws = false;

	// Init
	for (const auto& section : init->sections) {
		analyzer->enter_section(section);
		section->analyze(analyzer);
		for (const auto& ins : section->instructions) {
			ins->is_void = true;
			ins->analyze(analyzer);
			throws |= ins->throws;
			if (ins->may_return) {
				returning = ins->returning;
				may_return = ins->may_return;
				return_type = return_type->operator + (ins->return_type);
			}
			if (ins->returning) {
				analyzer->leave_block();
				return;
			}
		}
		analyzer->leave_section();
	}

	// Condition
	if (condition) {
		condition->sections.front()->analyze(analyzer);
		condition->sections.front()->instructions.front()->analyze(analyzer);
		condition->sections.front()->analyze_end(analyzer);
		throws |= condition->sections.front()->instructions[0]->throws;
	}

	// Body
	analyzer->enter_loop((Instruction*) this);
	body->analyze(analyzer);
	throws |= body->throws;
	if (body->may_return) may_return = true;
	return_type = return_type->operator + (body->return_type);
	if (req_type->is_array()) {
		type = Type::array(body->type);
	}

	analyzer->leave_loop();

	// Increment
	if (increment->reachable()) {
		analyzer->enter_block(increment.get());
		for (const auto& section : increment->sections) {
			section->analyze(analyzer);
			analyzer->enter_section(section);
			for (const auto& ins : section->instructions) {
				ins->is_void = true;
				ins->analyze(analyzer);
				throws |= ins->throws;
				if (ins->may_return) {
					may_return = ins->may_return;
					return_type = return_type->operator + (ins->return_type);
				}
				if (ins->returning) { break; }
			}
			analyzer->leave_section();
		}
	}

	analyzer->leave_block();

	if (mutations.size()) {
		if (condition) {
			condition->sections.front()->analyze(analyzer);
			condition->sections.front()->instructions.front()->analyze(analyzer);
			condition->sections.front()->analyze_end(analyzer);
			throws |= condition->sections.front()->instructions[0]->throws;
		}

		analyzer->enter_loop((Instruction*) this);
		body->is_void = true;
		body->analyze(analyzer);
		analyzer->leave_loop();

		if (increment->reachable()) {
			analyzer->enter_block(increment.get());
			for (const auto& section : increment->sections) {
				section->analyze(analyzer);
				analyzer->enter_section(section);
				for (const auto& ins : section->instructions) {
					ins->is_void = true;
					ins->analyze(analyzer);
					throws |= ins->throws;
					if (ins->may_return) {
						may_return = ins->may_return;
						return_type = return_type->operator + (ins->return_type);
					}
					if (ins->returning) { break; }
				}
				analyzer->leave_section();
			}
		}
	}
	analyzer->leave_block();
}

Hover For::hover(SemanticAnalyzer& analyzer, size_t position) const {
	return body->hover(analyzer, position);
}

#if COMPILER
Compiler::value For::compile(Compiler& c) const {

	c.enter_block(init.get()); // { for init ; cond ; inc { body } }<-- this block
	c.mark_offset(token->location.start.line);

	c.enter_section(init->sections.front());

	Compiler::value output_v { c.env };
	if (type->is_array()) {
		output_v = c.new_array(type->element(), {});
		c.insn_inc_refs(output_v);
		c.add_temporary_value(output_v); // Why create variable ? in case of `break 2` the output must be deleted
	}

	// Init
	bool first = true;
	for (const auto& section : init->sections) {
		if (not first) {
			c.enter_section(section);
		}
		for (const auto& ins : section->instructions) {
			ins->compile(c);
			if (dynamic_cast<Return*>(ins)) {
				auto return_v = c.clone(output_v);
				c.leave_block();
				return return_v;
			}
		}
		first = false;
	}

	// Cond
	c.leave_section();

	if (condition) {
		if (condition) {
			auto condition_v = condition->compile(c);
			auto bool_v = c.insn_to_bool(condition_v);
			condition->sections.back()->condition = bool_v;
			c.insn_delete_temporary(condition_v);
			condition->compile_end(c);
		} else {
			c.leave_section();
		}
	}

	// Body
	c.enter_loop(end_section, nullptr);
	auto body_v = body->compile(c);
	c.inc_ops(1);
	if (output_v.v && body_v.v) {
		c.insn_push_array(output_v, body_v);
	}
	body->compile_end(c);

	c.leave_loop();

	// Inc
	if (increment->reachable()) {
		c.enter_block(increment.get());
		increment->compile(c);
		increment->compile_end(c);
		c.leave_block();
	}

	// End
	c.enter_section(end_section);
	auto return_v = c.clone(output_v);

	c.leave_block(); // leave init block

	return return_v;
}
#endif

std::unique_ptr<Instruction> For::clone(Block* parent) const {
	auto f = std::make_unique<For>(type->env);
	f->token = token;

	auto current_section = parent->sections.back();

	f->init = std::make_unique<Block>(type->env);

	auto init_section = f->init->sections[0];
	init_section->name = "init";

	init_section->predecessors.push_back(current_section);
	current_section->successors.push_back(init_section);
	for (const auto& instruction : init->instructions) {
		f->init->add_instruction(unique_static_cast<Instruction>(instruction->clone(f->init.get())));
	}

	f->end_section = new Section(type->env, "end_for");

	if (condition) {
		f->condition = unique_static_cast<Block>(condition->clone(f->init.get()));
	}

	// increment
	auto increment_block = new Block(type->env);
	increment_block->sections.front()->name = "increment";
	f->continue_section = increment_block->sections.front();
	for (const auto& instruction : increment->instructions) {
		increment_block->add_instruction(instruction->clone(increment_block));
	}
	f->increment = std::unique_ptr<Block>(increment_block);

	// body
	auto condition_section = f->condition ? f->condition->sections.front() : init_section;

	f->body = unique_static_cast<Block>(body->clone(f->condition.get()));
	f->body->set_end_section(f->increment->sections.front());

	if (f->condition) {
		f->condition->sections.front()->successors.push_back(f->end_section);
		f->end_section->predecessors.push_back(f->condition->sections.front());
	}

	if (not f->increment->returning) {
		if (f->condition) {
			f->increment->sections.back()->successors.push_back(f->condition->sections.front());
			f->condition->sections.front()->predecessors.push_back(f->increment->sections.back());
		} else {
			f->increment->sections.back()->successors.push_back(f->body->sections.front());
			f->body->sections.front()->predecessors.push_back(f->increment->sections.back());
		}
	}

	return f;
}

}
