#include "While.hpp"
#include "../value/Number.hpp"
#include "../semantic/SemanticAnalyzer.hpp"
#include "../semantic/Variable.hpp"
#include "../value/Phi.hpp"
#include "../semantic/Mutation.hpp"
#include "../../colors.h"

namespace ls {

While::While(Environment& env) : Instruction(env) {
	// condition = nullptr;
	body = nullptr;
	jumping = true;
}

void While::print(std::ostream& os, int indent, PrintOptions options) const {

	// for (const auto& conversion : conversions) {
	// 	os << std::get<0>(conversion) << " " << std::get<0>(conversion)->type << " = (" << std::get<1>(conversion)->type << ") " << std::get<0>(conversion)->parent << std::endl;
	// }
	os << "while (" << std::endl;
	condition_section->print(os, indent, options);

	os << condition_section->color << "┃ " << END_COLOR << tabs(indent) << ") ";
	body->print(os, indent, options);
}

Location While::location() const {
	return token->location;
}

void While::pre_analyze(SemanticAnalyzer* analyzer) {

	const auto& before = condition_section->predecessors[0];

	mutations.clear();
	conversions.clear();
	condition_section->variables.clear();
	body->sections.back()->variables.clear();

	condition_section->pre_analyze(analyzer);

	analyzer->enter_loop((Instruction*) this);
	body->pre_analyze(analyzer);
	analyzer->leave_loop();

	// std::cout << "While mutations : " << mutations.size() << std::endl;
	for (const auto& mutation : mutations) {
		auto current = before;
		// std::cout << "mutation : " << mutation.variable << std::endl;
		while (current) {
			auto old_var = current->variables.find(mutation.variable->name);
			if (old_var != current->variables.end()) {
				analyzer->enter_section(current);
				auto new_var = analyzer->update_var(old_var->second, false);
				current->add_conversion(old_var->second, new_var, mutation.section);
				conversions.push_back({ new_var, old_var->second, mutation.section });
				analyzer->leave_section();
				// std::cout << "While add conversion " << new_var << " from " << old_var->second << " section " << current->color << current->id << END_COLOR << std::endl;
				break;
			}
			current = current->predecessors.size() ? current->predecessors[0] : nullptr;
		}
	}

	if (mutations.size()) {

		condition_section->pre_analyze(analyzer);

		analyzer->enter_loop((Instruction*) this);
		mutations.clear(); // Va être re-rempli par la seconde analyse

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

void While::analyze(SemanticAnalyzer* analyzer, const Type*) {

	analyzer->leave_section(); // Leave previous section

	condition_section->analyze(analyzer);
	condition_section->instructions.front()->analyze(analyzer);
	condition_section->analyze_end(analyzer);

	throws = condition_section->instructions.front()->throws;

	analyzer->enter_loop((Instruction*) this);
	body->is_void = true;
	body->analyze(analyzer);
	analyzer->leave_loop();

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
	}

	throws |= body->throws;
	if (body->may_return) {
		may_return = body->may_return;
		returning = body->returning;
		return_type = body->return_type;
	}
}

#if COMPILER
Compiler::value While::compile(Compiler& c) const {
	c.mark_offset(token->location.start.line);

	c.leave_section(); // Leave previous section

	// Condition section
	c.enter_section(condition_section);
	auto cond = condition_section->instructions.front()->compile(c);
	auto cond_boolean = c.insn_to_bool(cond);
	condition_section->instructions.front()->compile_end(c);
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
