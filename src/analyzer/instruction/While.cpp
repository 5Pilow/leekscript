#include "While.hpp"
#include "../value/Number.hpp"
#include "../semantic/SemanticAnalyzer.hpp"
#include "../semantic/Variable.hpp"
#include "../value/Phi.hpp"
#include "../semantic/Mutation.hpp"
#include "../../colors.h"

namespace ls {

While::While(Environment& env) : Instruction(env) {
	body = nullptr;
	jumping = true;
}

void While::print(std::ostream& os, int indent, PrintOptions options) const {

	// for (const auto& conversion : conversions) {
	// 	os << std::get<0>(conversion) << " " << std::get<0>(conversion)->type << " = (" << std::get<1>(conversion)->type << ") " << std::get<0>(conversion)->parent << std::endl;
	// }
	os << "while (" << std::endl;
	if (condition) {
		condition->print(os, indent, { options.debug, true, options.condensed });
		os << condition->sections.front()->color << "┃ " << END_COLOR << tabs(indent) << ") ";
	}
	body->print(os, indent, options);
}

Location While::location() const {
	return token->location;
}

void While::pre_analyze(SemanticAnalyzer* analyzer) {

	const auto& before = condition->sections.front()->predecessors[0];

	mutations.clear();
	conversions.clear();
	condition->variables.clear();
	condition->sections.front()->variables.clear();

	condition->pre_analyze(analyzer);

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
				current->add_conversion({ old_var->second, new_var, mutation.variable, mutation.section });
				conversions.push_back({ new_var, old_var->second, mutation.section });
				analyzer->leave_section();
				// std::cout << "While add conversion " << new_var << " from " << old_var->second << " section " << current->color << current->id << END_COLOR << std::endl;
				break;
			}
			current = current->predecessors.size() ? current->predecessors[0] : nullptr;
		}
	}

	if (mutations.size()) {

		condition->pre_analyze(analyzer);

		analyzer->enter_loop((Instruction*) this);
		mutations.clear(); // Va être re-rempli par la seconde analyse

		body->pre_analyze(analyzer);
		analyzer->leave_loop();

		for (const auto& phi : condition->sections.front()->phis) {
			// std::cout << "While phi " << phi->variable << std::endl;
			for (const auto& mutation : mutations) {
				// std::cout << "While mutation " << mutation.variable << " " << mutation.section->id << std::endl;
				if (mutation.variable->name == phi->variable2->name) {
					phi->variable2 = mutation.variable;
					// std::cout << "While set var for phi " << phi->variable2 << std::endl;
				}
			}
		}
	}
}

void While::analyze(SemanticAnalyzer* analyzer, const Type*) {

	analyzer->leave_section(); // Leave previous section

	condition->sections.front()->analyze(analyzer);
	condition->sections.front()->instructions.front()->analyze(analyzer);
	condition->sections.front()->analyze_end(analyzer);

	throws = condition->sections.front()->instructions[0]->throws;

	analyzer->enter_loop((Instruction*) this);
	body->is_void = true;
	body->analyze(analyzer);
	analyzer->leave_loop();

	for (const auto& conversion : conversions) {
		std::get<0>(conversion)->section->reanalyze_conversions(analyzer);
	}

	if (conversions.size()) {
		condition->sections.front()->analyze(analyzer);
		condition->sections.front()->instructions.front()->analyze(analyzer);
		condition->sections.front()->analyze_end(analyzer);

		throws = condition->sections.front()->instructions[0]->throws;

		analyzer->enter_loop((Instruction*) this);
		body->is_void = true;
		body->analyze(analyzer);
		analyzer->leave_loop();
	}

	throws |= body->throws;
	if (body->may_return) {
		may_return = body->may_return;
		return_type = body->return_type;
	}
}

#if COMPILER
Compiler::value While::compile(Compiler& c) const {
	c.mark_offset(token->location.start.line);

	c.leave_section(); // Leave previous section

	// Condition section
	auto cond = condition->compile(c);
	c.inc_ops(1);
	auto cond_boolean = c.insn_to_bool(cond);
	condition->sections.back()->condition = cond_boolean;
	c.insn_delete_temporary(cond);
	condition->compile_end(c);

	c.enter_loop(end_section, condition->sections.front());
	auto body_v = body->compile(c);
	if (body_v.v) {
		c.insn_delete_temporary(body_v);
	}
	body->compile_end(c);
	c.leave_loop();

	return { c.env };
}
#endif

std::unique_ptr<Instruction> While::clone(Block* parent) const {
	auto w = std::make_unique<While>(type->env);
	w->token = token;

	auto current_section = parent->sections.back();

	w->end_section = new Section(type->env, "end");

	w->condition = unique_static_cast<Block>(condition->clone(parent));
	w->condition->sections.front()->name = "condition";
	w->continue_section = w->condition->sections.front();

	w->body = unique_static_cast<Block>(body->clone(w->condition.get()));
	w->body->sections.front()->name = "body";
	w->body->set_end_section(w->condition->sections.front());

	w->condition->sections.front()->add_successor(w->end_section);
	w->end_section->add_predecessor(w->condition->sections.front());

	return w;
}

}
