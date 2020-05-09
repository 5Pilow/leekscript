#include "If.hpp"
#include "../semantic/SemanticAnalyzer.hpp"
#include "Number.hpp"
#include "../instruction/Return.hpp"
#include "../instruction/Break.hpp"
#include "../instruction/Continue.hpp"
#include "../semantic/Variable.hpp"
#include "Phi.hpp"

namespace ls {

If::If(Environment& env, bool ternary) : Value(env) {
	this->ternary = ternary;
	jumping = true;
}

void If::print(std::ostream& os, int indent, PrintOptions options) const {
	// if (ternary) {
	// 	os << "(";
	// 	condition->print(os, indent, options);
	// 	os << " ? ";
	// 	then->sections[0]->instructions[0]->print(os, indent, options);
	// 	os << " : ";
	// 	elze->sections[0]->instructions[0]->print(os, indent, options);
	// 	os << ")";
	// } else {
	os << "if (";
	condition->print(os, indent + 1, options);
	os << ") ";
	then->print(os, indent, options);
	if (elze != nullptr and (options.sections or elze->instructions.size())) {
		os << " else ";
		elze->print(os, indent, options);
	}
}

Location If::location() const {
	return { token->location.file, token->location.start, elze ? elze->location().end : then->location().end };
}

void If::pre_analyze(SemanticAnalyzer* analyzer) {
	condition->pre_analyze(analyzer);
	then->pre_analyze(analyzer);
	if (elze != nullptr) {
		elze->pre_analyze(analyzer);
	}
}

void If::analyze(SemanticAnalyzer* analyzer) {
	// std::cout << "If " << is_void << std::endl;
	const auto& env = analyzer->env;

	condition->analyze(analyzer);
	throws = condition->throws;

	then->is_void = is_void;
	then->analyze(analyzer);

	type = then->type;

	if (elze != nullptr) {
		elze->is_void = is_void;
		elze->analyze(analyzer);
		// std::cout << "elze type " << elze->type << std::endl;
		if (type->is_void() and not elze->type->is_void() and not then->returning) {
			type = env.null;
		} else if (type != env.void_ and elze->type == env.void_) {
			type = type->operator + (env.null);
		} else {
			type = type->operator + (elze->type);
		}
	} else if (not type->is_void() or then->returning) {
		type = type->operator + (env.null);
	}
	if (is_void) type = env.void_;
	throws |= then->throws or (elze != nullptr and elze->throws);
	returning = then->returning and (elze != nullptr and elze->returning);
	may_return = then->may_return or (elze != nullptr and elze->may_return);
	return_type = then->return_type;
	if (elze != nullptr) return_type = return_type->operator + (elze->return_type);
}

Hover If::hover(SemanticAnalyzer& analyzer, size_t position) const {
	if (condition->location().contains(position)) {
		return condition->hover(analyzer, position);
	}
	if (then->location().contains(position)) {
		return then->hover(analyzer, position);
	}
	if (elze && elze->location().contains(position)) {
		return elze->hover(analyzer, position);
	}
	return { type, location() };
}

#if COMPILER
Compiler::value If::compile(Compiler& c) const {

	Compiler::value then_v { c.env };
	Compiler::value else_v { c.env };
	bool compile_elze = elze != nullptr or not type->is_void();

	auto cond = condition->compile(c);
	auto cond_boolean = c.insn_to_bool(cond);
	condition->compile_end(c);
	c.insn_delete_temporary(cond);

	c.leave_section_condition(cond_boolean);

	then_v = c.insn_convert(then->compile(c), type->fold(), true);
	if (!then_v.v and not then->returning) then_v = c.insn_convert(c.new_null(), type->fold());
	then->compile_end(c);

	if (compile_elze) {
		if (elze) {
			else_v = c.insn_convert(elze->compile(c), type->fold(), true);
			if (type != c.env.void_ and else_v.t == c.env.void_) {
				else_v = c.insn_convert(c.new_null(), type->fold());
			}
			elze->compile_end(c);
		} else if (not type->is_void()) {
			else_v = c.insn_convert(c.new_null(), type->fold());
		}
	}

	if (end_section) {
		c.enter_section(end_section);

		auto r = [&]() -> Compiler::value { if (type->is_void()) {
			return { c.env };
		} else {
			return c.insn_phi(type, then_v, then->sections.back().get(), else_v, elze->sections.back().get());
		}}();
		return r;
	}
	return { c.env };
}
#endif

std::unique_ptr<Value> If::clone(Block* parent) const {
	auto iff = std::make_unique<If>(type->env);
	iff->token = token;
	iff->ternary = ternary;
	iff->condition = condition->clone(parent);
	iff->then = unique_static_cast<Block>(then->clone(parent));
	iff->elze = elze ? unique_static_cast<Block>(elze->clone(parent)) : nullptr;

	if (not iff->then->returning or (iff->elze and not iff->elze->returning)) {
		iff->end_section = new Section(type->env, "end_if");
		if (not iff->then->returning) {
			iff->then->sections.back()->add_successor(iff->end_section);
			iff->end_section->add_predecessor(iff->then->sections.back().get());
		}
		if (iff->elze and not iff->elze->returning) {
			iff->elze->sections.back()->add_successor(iff->end_section);
			iff->end_section->add_predecessor(iff->elze->sections.back().get());
		}
	}
	return iff;
}

}
