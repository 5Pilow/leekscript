#include "Foreach.hpp"
#include "../semantic/SemanticAnalyzer.hpp"
#include "../semantic/Variable.hpp"
#include "../../colors.h"
#include "../value/Phi.hpp"
#include "../value/VariableValue.hpp"

namespace ls {

Foreach::Foreach(Environment& env) : Instruction(env) {
	wrapper_block = std::make_unique<Block>(env);
	key_var = nullptr;
	value_var = nullptr;
	jumping = true;
}

void Foreach::print(std::ostream& os, int indent, PrintOptions options) const {
	os << "for ";
	if (options.sections) {
		os << std::endl;
	}
	wrapper_block->print(os, indent, { options.debug, true, options.sections });
	condition_section->print(os, indent, { options.debug, true, options.sections });

	if (options.sections) {
		os << condition_section->color << "┃" << END_COLOR << tabs(indent + 1);
	}
	if (key != nullptr) {
		os << key->content;
		if (options.debug) {
			os << " " << container->type->key();
		}
		os << " : ";
	}
	os << value_var;
	if (options.debug) {
		os << " " << container->type->element();
	}

	os << " in ";
	container->print(os, indent + 1, options);

	os << " ";
	body->print(os, indent, options);

	if (options.sections) {
		os << std::endl;
		increment_section->print(os, indent, { options.debug, true, options.sections });
	}
}

Location Foreach::location() const {
	return { token->location.file, token->location.start, body->location().end };
}

void Foreach::pre_analyze(SemanticAnalyzer* analyzer) {
	auto& env = analyzer->env;

	mutations.clear();
	conversions.clear();

	analyzer->enter_block(wrapper_block.get());

	analyzer->enter_loop((Instruction*) this);

	analyzer->enter_section(wrapper_block->sections.front());
	container->pre_analyze(analyzer);

	if (not value_var) {
		analyzer->enter_block(body.get());
		analyzer->enter_section(body->sections.front());
		if (key != nullptr) {
			key_var = analyzer->add_var(key, env.void_, nullptr);
			key_var->injected = key_var->loop_variable = true;
			assert(key_var);
		}
		value_var = analyzer->add_var(value, env.void_, nullptr);
		assert(value_var);
		value_var->injected = value_var->loop_variable = true;
		value_var->array = container.get();
		analyzer->leave_section();
		analyzer->leave_block();
	}

	body->pre_analyze(analyzer);

	analyzer->leave_loop();

	// std::cout << "conversions: " << conversions.size() << std::endl;

	if (mutations.size()) {

		mutations.clear(); // Va être re-rempli par la seconde analyse

		wrapper_block->sections.front()->variables.clear();
		analyzer->enter_section(wrapper_block->sections.front());
		container->pre_analyze(analyzer);

		condition_section->pre_analyze(analyzer);

		analyzer->enter_loop((Instruction*) this);

		body->pre_analyze(analyzer);

		analyzer->leave_loop();

		for (const auto& phi : condition_section->phis) {
			// std::cout << "phi " << phi->variable << std::endl;
			for (const auto& mutation : mutations) {
				// std::cout << "mutation " << mutation.variable << " " << mutation.section->id << std::endl;
				if (mutation.variable->name == phi->variable2->name) {
					phi->variable2 = mutation.variable;
					// std::cout << "set var for phi " << phi->variable2 << std::endl;
				}
			}
		}
	}
}

void Foreach::analyze(SemanticAnalyzer* analyzer, const Type* req_type) {
	auto& env = analyzer->env;
	if (req_type->is_array()) {
		type = req_type;
	} else {
		type = env.void_;
		body->is_void = true;
	}
	analyzer->enter_block(wrapper_block.get());
	analyzer->enter_section(wrapper_block->sections.front());

	container->analyze(analyzer);
	throws = container->throws;

	analyzer->leave_section();

	if (not container->type->is_void() and not container->type->iterable() and not container->type->is_any()) {
		analyzer->add_error({Error::Type::VALUE_NOT_ITERABLE, container->location(), container->location(), {container->to_string(), container->type->to_string()}});
		return;
	}

	key_type = container->type->key();
	value_type = container->type->element();
	if (key != nullptr) {
		key_var->type = key_type;
	}
	value_var->type = value_type;

	// Set the mode
	if (container->type->is_number() or container->type->is_string() or container->type->is_interval()) {
		mode = ForeachMode::VALUE;
	} else {
		mode = ForeachMode::ADDRESS;
	}

	analyzer->enter_section(condition_section);
	condition_section->analyze(analyzer);
	analyzer->leave_section();

	analyzer->enter_loop((Instruction*) this);
	body->analyze(analyzer);
	throws |= body->throws;
	if (body->may_return) {
		may_return = body->may_return;
		return_type = body->return_type;
	}
	if (req_type->is_array()) {
		type = Type::tmp_array(body->type);
	}

	analyzer->leave_loop();
	analyzer->leave_block();

	if (mutations.size()) {

		analyzer->enter_block(wrapper_block.get());

		// Re-analyze container
		analyzer->enter_section(wrapper_block->sections.front());
		container->analyze(analyzer);
		analyzer->leave_section();

		key_type = container->type->key();
		value_type = container->type->element();
		if (key != nullptr) {
			key_var->type = key_type;
		}
		value_var->type = value_type;

		// Set the mode
		if (container->type->is_number() or container->type->is_string() or container->type->is_interval()) {
			mode = ForeachMode::VALUE;
		} else {
			mode = ForeachMode::ADDRESS;
		}

		analyzer->enter_section(condition_section);
		condition_section->analyze(analyzer);
		analyzer->leave_section();

		analyzer->enter_loop((Instruction*) this);
		body->is_void = true;
		body->analyze(analyzer);
		if (body->may_return) {
			may_return = body->may_return;
			return_type = body->return_type;
		}
		analyzer->leave_loop();

		analyzer->leave_block();
	}
}

#if COMPILER
Compiler::value Foreach::compile(Compiler& c) const {

	c.enter_block(wrapper_block.get());
	c.enter_section(wrapper_block->sections.front());

	auto container_v = container->compile(c);
	if (mode == ForeachMode::COPY) {
		value_var->create_entry(c);
		if (key_var) key_var->create_entry(c);
	}

	auto output = type->element();

	// Potential output [for ...]
	Compiler::value output_v { c.env };
	output_v.t = c.env.void_;
	if (not output->is_void()) {
		output_v = c.new_array(output, {});
		c.insn_inc_refs(output_v);
		c.add_temporary_value(output_v); // Why create variable? in case of `break 2` the output must be deleted
	}

	c.insn_inc_refs(container_v);
	c.add_temporary_value(container_v);

	auto it = c.iterator_begin(container_v);

	// For arrays, if begin iterator is 0, jump to end directly
	if (container_v.t->is_array()) {
		auto empty_array = c.insn_pointer_eq(it, c.new_null_pointer(it.t));
		// c.insn_if_new(empty_array, &end_label, &cond_label);
		c.leave_section_condition(empty_array);
	} else {
		// c.insn_branch(&cond_label);
		c.leave_section();
	}

	// cond label:
	c.enter_section(condition_section);

	// Condition to continue
	auto finished = c.iterator_end(container_v, it);
	c.leave_section_condition(c.insn_not_bool(finished));

	// loop label:
	body->sections.front()->pre_compile(c);
	c.builder.SetInsertPoint(body->sections.front()->basic_block);

	// Get Value
	if (mode == ForeachMode::ADDRESS) {
		value_var->entry = c.iterator_get(container_v.t, it);
	} else if (mode == ForeachMode::VALUE) {
		value_var->val = c.iterator_get(container_v.t, it);
	}
	// Get Key
	if (key) {
		// if (mode == ForeachMode::ADDRESS) {
		// 	key_var->entry = c.iterator_key(container_v, it, {c.env});
		// } else if (mode == ForeachMode::VALUE) {
			key_var->val = c.iterator_key(container_v, it);
		// }
		// c.insn_store(key_v, c.iterator_key(container_v, it, c.insn_load(key_v)));
	}
	// Body
	auto body_v = body->compile(c);
	if (body_v.v) {
		if (output_v.v) {
			c.insn_push_array(output_v, body_v);
		} else {
			c.insn_delete_temporary(body_v);
		}
	}
	body->compile_end(c);

	// it++
	c.enter_section(increment_section);
	c.iterator_increment(container_v.t, it);
	c.leave_section();

	c.enter_section(end_section);

	auto return_v = c.clone(output_v); // otherwise it is delete by the leave_block
	c.leave_block(); // { for x in ['a' 'b'] { ... }<--- not this block }<--- this block

	return return_v;
}
#endif

std::unique_ptr<Instruction> Foreach::clone(Block* parent) const {
	auto f = std::make_unique<Foreach>(type->env);
	f->token = token;
	f->key = key;
	f->value = value;
	f->container = container->clone(f->wrapper_block.get());

	auto wrapper_section = f->wrapper_block->sections[0];
	wrapper_section->name = "wrapper";

	auto current_section = parent->sections.back();

	wrapper_section->predecessors.push_back(current_section);
	current_section->successors.push_back(wrapper_section);

	f->condition_section = new Section(type->env, "condition");
	f->end_section = new Section(type->env, "end");

	if (f->container->jumping) {
		f->container->set_end_section(f->condition_section);
	} else {
		f->condition_section->predecessors.push_back(wrapper_section);
		wrapper_section->successors.push_back(f->condition_section);
	}

	f->increment_section = new Section(type->env, "increment");
	f->continue_section = f->increment_section;

	f->body = unique_static_cast<Block>(body->clone(nullptr));

	f->body->sections.front()->add_predecessor(f->condition_section);
	f->condition_section->add_successor(f->body->sections.front());

	f->body->set_end_section(f->increment_section);

	f->condition_section->successors.push_back(f->end_section);
	f->end_section->predecessors.push_back(f->condition_section);

	f->increment_section->successors.push_back(f->condition_section);
	f->condition_section->predecessors.push_back(f->increment_section);

	return f;
}

}
