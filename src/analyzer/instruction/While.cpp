#include "While.hpp"
#include "../value/Number.hpp"
#include "../semantic/SemanticAnalyzer.hpp"
#include "../semantic/Variable.hpp"
#include "../value/Phi.hpp"
#include "../../colors.h"

namespace ls {

While::While(Environment& env) : Instruction(env) {
	// condition = nullptr;
	body = nullptr;
	jumping = true;
}

void While::print(std::ostream& os, int indent, PrintOptions options) const {
	os << "while ("<< std::endl;
	// condition->print(os, indent + 1, options);
	condition_section->print(os, indent, options);
	os << " " << tabs(indent) << ") ";
	body->print(os, indent, options);
	if (body2_activated) {
		os << " ";
		body2->print(os, indent, options);
	}
	// for (const auto& assignment : assignments) {
	// 	os << std::endl << tabs(indent) << assignment.first << " " << assignment.first->type << " = " << assignment.second << " " << assignment.second->type;
	// }
}

Location While::location() const {
	return token->location;
}


void While::pre_analyze(SemanticAnalyzer* analyzer) {
	condition_section->pre_analyze(analyzer);
	body->is_loop_body = true;
	body->pre_analyze(analyzer);

	// for (const auto& variable : body->mutations) {
	// 	if ((variable->root ? variable->root : variable)->block != body.get()) {
	// 		mutations.push_back(variable);
	// 	}
	// }

	// if (mutations.size()) {
	// 	body2 = std::move(unique_static_cast<Block>(body->clone()));
	// 	body2->is_loop_body = true;
	// 	body2->is_loop = true;
	// 	for (const auto& variable : body->variables) {
	// 		if (variable.second->parent and variable.second->parent->block != body.get()) {
	// 			body2->variables.insert({ variable.first, variable.second });
	// 		}
	// 	}
	// 	body2->pre_analyze(analyzer);
	// }
	// for (const auto& variable : body->variables) {
	// 	// std::cout << "Foreach assignment " << variable.second << " " << (void*) variable.second->block->branch << " " << (void*) analyzer->current_block()->branch << std::endl;
	// 	if (variable.second->parent) {
	// 		auto new_var = analyzer->update_var(variable.second->parent);
	// 		variable.second->assignment = true;
	// 		assignments.push_back({ new_var, variable.second });
	// 	}
	// }
}

void While::analyze(SemanticAnalyzer* analyzer, const Type*) {
	condition_section->analyze(analyzer);
	condition_section->instructions.front()->analyze(analyzer);
	throws = condition_section->instructions.front()->throws;

	analyzer->enter_loop();
	body->is_void = true;
	if (body2) body2->is_void = true;
	body->analyze(analyzer);

	// body2_activated = std::any_of(mutations.begin(), mutations.end(), [&](Variable* variable) {
	// 	std::cout << "mutation " << variable->parent << " " << variable->parent->type << " => " << variable << " " << variable->type << std::endl;
	// 	return variable->parent->type != variable->type;
	// });
	// std::cout << "body2 activated " << body2_activated << std::endl;
	// if (body2_activated) {
	// 	body2->analyze(analyzer);
	// } else if (body2) {
	// 	body2->enabled = false;
	// }
	analyzer->leave_loop();

	throws |= body->throws;
	if (body->may_return) {
		may_return = body->may_return;
		returning = body->returning;
		return_type = body->return_type;
	}

	// for (const auto& assignment : assignments) {
	// 	assignment.first->type = assignment.second->type;
	// }
}

#if COMPILER
Compiler::value While::compile(Compiler& c) const {
	c.mark_offset(token->location.start.line);

	c.leave_section(); // Leave previous section

	// Condition section
	c.enter_section(condition_section);
	auto cond = condition_section->instructions.front()->compile(c);
	auto cond_boolean = c.insn_to_bool(cond);
	c.insn_delete_temporary(cond);

	c.leave_section_condition(cond_boolean);

	c.inc_ops(1);
	c.enter_loop(end_section, condition_section);
	auto body_v = body->compile(c);
	if (body_v.v) {
		c.insn_delete_temporary(body_v);
	}
	body->compile_end(c);
	c.leave_loop();

	// for (const auto& assignment : assignments) {
	// 	// std::cout << "Store variable " << assignment.first << " = " << assignment.second << std::endl;
	// 	assignment.first->val = c.create_entry(assignment.first->name, assignment.first->type);
	// 	c.insn_store(assignment.first->val, c.insn_load(assignment.second->val));
	// }
	return { c.env };
}
#endif

std::unique_ptr<Instruction> While::clone() const {
	auto w = std::make_unique<While>(type->env);
	w->token = token;
	w->condition_section = condition_section;
	w->body = unique_static_cast<Block>(body->clone());
	return w;
}

}
