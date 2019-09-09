#include "While.hpp"
#include "../../compiler/value/Number.hpp"
#include "../../vm/LSValue.hpp"
#include "../../vm/value/LSNull.hpp"
#include "../semantic/SemanticAnalyzer.hpp"
#include "../semantic/Variable.hpp"
#include "../value/Phi.hpp"

namespace ls {

While::While() {
	condition = nullptr;
	body = nullptr;
}

void While::print(std::ostream& os, int indent, bool debug, bool condensed) const {
	os << "while ";
	condition->print(os, indent + 1, debug);
	os << " ";
	body->print(os, indent, debug);
	if (body2_activated) {
		os << " ";
		body2->print(os, indent, debug);
	}
	for (const auto& assignment : assignments) {
		os << std::endl << tabs(indent) << assignment.first << " " << assignment.first->type << " = " << assignment.second << " " << assignment.second->type;
	}
}

Location While::location() const {
	return token->location;
}


void While::pre_analyze(SemanticAnalyzer* analyzer) {
	condition->pre_analyze(analyzer);
	body->is_loop_body = true;
	body->pre_analyze(analyzer);

	for (const auto& variable : body->mutations) {
		if ((variable->root ? variable->root : variable)->block != body.get()) {
			mutations.push_back(variable);
		}
	}

	if (mutations.size()) {
		body2 = std::move(unique_static_cast<Block>(body->clone()));
		body2->is_loop_body = true;
		body2->is_loop = true;
		for (const auto& variable : body->variables) {
			if (variable.second->parent and variable.second->parent->block != body.get()) {
				body2->variables.insert({ variable.first, variable.second });
			}
		}
		body2->pre_analyze(analyzer);
	}
	for (const auto& variable : body->variables) {
		// std::cout << "Foreach assignment " << variable.second << " " << (void*) variable.second->block->branch << " " << (void*) analyzer->current_block()->branch << std::endl;
		if (variable.second->parent) {
			auto new_var = analyzer->update_var(variable.second->parent);
			variable.second->assignment = true;
			assignments.push_back({ new_var, variable.second });
		}
	}
}

void While::analyze(SemanticAnalyzer* analyzer, const Type*) {
	condition->analyze(analyzer);
	throws = condition->throws;

	analyzer->enter_loop();
	body->is_void = true;
	if (body2) body2->is_void = true;
	body->analyze(analyzer);

	body2_activated = std::any_of(mutations.begin(), mutations.end(), [&](Variable* variable) {
		// std::cout << "mutation " << variable->parent << " " << variable->parent->type << " => " << variable << " " << variable->type << std::endl;
		return variable->parent->type != variable->type;
	});
	if (body2_activated) {
		body2->analyze(analyzer);
	} else if (body2) {
		body2->enabled = false;
	}
	analyzer->leave_loop();
	
	throws |= body->throws;
	if (body->may_return) {
		may_return = body->may_return;
		returning = body->returning;
		return_type = body->return_type;
	}

	for (const auto& assignment : assignments) {
		assignment.first->type = assignment.second->type;
	}
}

Compiler::value While::compile(Compiler& c) const {
	c.mark_offset(token->location.start.line);
	auto cond_label = c.insn_init_label("cond");
	auto end_label = c.insn_init_label("end");
	auto loop_label = c.insn_init_label("loop");
	Compiler::label cond2_label;
	Compiler::label loop2_label;
	if (body2_activated) {
		cond2_label = c.insn_init_label("cond2");
		loop2_label = c.insn_init_label("loop2");
	}
	c.insn_branch(&cond_label);
	c.insn_label(&cond_label);
	auto cond = condition->compile(c);
	auto cond_bool = c.insn_to_bool(cond);
	condition->compile_end(c);
	c.insn_delete_temporary(cond);
	c.insn_if_new(cond_bool, &loop_label, &end_label);
	c.insn_label(&loop_label);
	c.inc_ops(1);
	c.enter_loop(&end_label, &cond_label);
	auto body_v = body->compile(c);
	if (body_v.v) {
		c.insn_delete_temporary(body_v);
	}
	c.leave_loop();
	c.insn_branch(body2_activated ? &cond2_label : &cond_label);

	if (body2_activated) {
		c.insn_label(&cond2_label);
		auto cond2 = condition->compile(c);
		condition->compile_end(c);
		auto cond2_bool = c.insn_to_bool(cond2);
		c.insn_delete_temporary(cond2);
		c.insn_if_new(cond2_bool, &loop2_label, &end_label);
		c.insn_label(&loop2_label);
		c.inc_ops(1);
		c.enter_loop(&end_label, &cond2_label);
		auto body2_v = body2->compile(c);
		if (body2_v.v) {
			c.insn_delete_temporary(body2_v);
		}
		c.leave_loop();
		c.insn_branch(&cond2_label);
	}

	c.insn_label(&end_label);

	for (const auto& assignment : assignments) {
		// std::cout << "Store variable " << assignment.first << " = " << assignment.second << std::endl;
		assignment.first->val = c.create_entry(assignment.first->name, assignment.first->type);
		c.insn_store(assignment.first->val, c.insn_load(assignment.second->val));
	}
	return {};
}

std::unique_ptr<Instruction> While::clone() const {
	auto w = std::make_unique<While>();
	w->token = token;
	w->condition = condition->clone();
	w->body = unique_static_cast<Block>(body->clone());
	return w;
}

}
