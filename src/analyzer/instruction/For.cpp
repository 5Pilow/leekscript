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
	os << "for " << std::endl;
	init->print(os, indent, { options.debug, true, options.sections });
	os << " ;" << std::endl;
	if (condition_section != nullptr) {
		condition_section->print(os, indent, { options.debug, true, options.sections });
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
	return {nullptr, {0, 0, 0}, {0, 0, 0}};
}

void For::pre_analyze(SemanticAnalyzer* analyzer) {

	mutations.clear();
	conversions.clear();
	condition_section->variables.clear();
	increment->sections.back()->variables.clear();
	body->sections.back()->variables.clear();

	init->setup_branch(analyzer);
	analyzer->enter_block(init.get());

	init->variables.clear();
	for (const auto& section : init->sections) {
		section->pre_analyze(analyzer);
	}
	// std::cout << "init section vars : " << init->sections[0]->variables.size() << std::endl;

	analyzer->enter_loop((Instruction*) this);

	if (condition_section != nullptr) {
		condition_section->pre_analyze(analyzer);
	}

	body->pre_analyze(analyzer);

	increment->pre_analyze(analyzer);
	analyzer->leave_loop();

	// for (const auto& mutation : mutations) {
	// 	std::cout << "mutation " << mutation.variable << " " << mutation.section->id << std::endl;
	// }

	// std::cout << "For mutations : " << mutations.size() << std::endl;
	const auto& before = init->sections.back();
	for (const auto& mutation : mutations) {
		auto current = before;
		while (current) {
			auto old_var = current->variables.find(mutation.variable->name);
			if (old_var != current->variables.end()) {
				analyzer->enter_section(current);
				auto new_var = analyzer->update_var(old_var->second, false);
				current->add_conversion({ old_var->second, new_var, mutation.variable, mutation.section });
				conversions.push_back({ new_var, old_var->second, mutation.section });
				analyzer->leave_section();

				// std::cout << "For add conversion " << new_var << " from " << old_var->second << " section " << current->color << current->id << END_COLOR << std::endl;
				break;
			}
			current = current->predecessors.size() ? current->predecessors[0] : nullptr;
		}
	}

	// std::cout << "conversions: " << conversions.size() << std::endl;

	if (mutations.size()) {

		analyzer->enter_loop((Instruction*) this);
		mutations.clear(); // Va être re-rempli par la seconde analyse

		condition_section->pre_analyze(analyzer);

		body->pre_analyze(analyzer);

		increment->pre_analyze(analyzer);
		analyzer->leave_loop();

		for (const auto& phi : condition_section->phis) {
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
	analyzer->leave_block();
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
	if (condition_section != nullptr) {
		analyzer->enter_section(condition_section);
		condition_section->analyze(analyzer);
		if (condition_section->instructions.size()) {
			condition_section->instructions[0]->analyze(analyzer);
			throws |= condition_section->instructions[0]->throws;
		}
		analyzer->leave_section();
	}

	// Body
	analyzer->enter_loop((Instruction*) this);
	body->analyze(analyzer);
	throws |= body->throws;
	if (body->returning) returning = true;
	if (body->may_return) may_return = true;
	return_type = return_type->operator + (body->return_type);
	if (req_type->is_array()) {
		type = Type::array(body->type);
	}

	analyzer->leave_loop();

	// Increment
	analyzer->enter_block(increment.get());
	for (const auto& section : increment->sections) {
		section->analyze(analyzer);
		analyzer->enter_section(section);
		for (const auto& ins : section->instructions) {
			ins->is_void = true;
			ins->analyze(analyzer);
			throws |= ins->throws;
			if (ins->may_return) {
				returning = ins->returning;
				may_return = ins->may_return;
				return_type = return_type->operator + (ins->return_type);
			}
			if (ins->returning) { break; }
		}
		analyzer->leave_section();
	}

	analyzer->leave_block();

	for (const auto& conversion : conversions) {
		std::get<0>(conversion)->section->reanalyze_conversions(analyzer);
	}

	if (conversions.size()) {
		condition_section->analyze(analyzer);
		condition_section->instructions.front()->analyze(analyzer);
		throws = condition_section->instructions.front()->throws;

		analyzer->enter_loop((Instruction*) this);
		body->is_void = true;
		body->analyze(analyzer);
		analyzer->leave_loop();

		analyzer->enter_block(increment.get());
		for (const auto& section : increment->sections) {
			section->analyze(analyzer);
			analyzer->enter_section(section);
			for (const auto& ins : section->instructions) {
				ins->is_void = true;
				ins->analyze(analyzer);
				throws |= ins->throws;
				if (ins->may_return) {
					returning = ins->returning;
					may_return = ins->may_return;
					return_type = return_type->operator + (ins->return_type);
				}
				if (ins->returning) { break; }
			}
			analyzer->leave_section();
		}
	}
	analyzer->leave_block();
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
			if (dynamic_cast<Return*>(ins.get())) {
				auto return_v = c.clone(output_v);
				c.leave_block();
				return return_v;
			}
		}
		first = false;
	}

	// Cond
	c.leave_section();
	c.enter_section(condition_section);

	c.inc_ops(1);
	if (condition_section->instructions.size()) {
		auto condition_v = condition_section->instructions.front()->compile(c);
		auto bool_v = c.insn_to_bool(condition_v);
		condition_section->instructions.front()->compile_end(c);
		c.insn_delete_temporary(condition_v);
		c.leave_section_condition(bool_v);
	} else {
		c.leave_section();
	}

	// Body
	c.enter_loop(end_section, condition_section);
	auto body_v = body->compile(c);
	if (output_v.v && body_v.v) {
		c.insn_push_array(output_v, body_v);
	}
	body->compile_end(c);

	c.leave_loop();

	// Inc
	c.enter_block(increment.get());
	increment->compile(c);
	increment->compile_end(c);
	c.leave_block();

	// End
	c.enter_section(end_section);
	auto return_v = c.clone(output_v);

	c.leave_block(); // leave init block

	return return_v;
}
#endif

std::unique_ptr<Instruction> For::clone() const {
	auto f = std::make_unique<For>(type->env);
	f->token = token;
	f->init = unique_static_cast<Block>(init->clone());
	if (condition_section) f->condition_section = condition_section->clone();
	f->increment = unique_static_cast<Block>(increment->clone());
	f->body = unique_static_cast<Block>(body->clone());
	return f;
}

}
