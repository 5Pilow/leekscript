#include "Throw.hpp"
#include "../semantic/SemanticAnalyzer.hpp"
#include "../value/Function.hpp"

namespace ls {

Throw::Throw(Environment& env, Token* token, std::unique_ptr<Value> v) : Instruction(env), token(token), expression(std::move(v)) {
	throws = true;
	jumping = true;
}

void Throw::print(std::ostream& os, int indent, PrintOptions options) const {
	os << "throw";
	if (expression != nullptr) {
		os << " ";
		expression->print(os, indent, options);
	}
}

Location Throw::location() const {
	if (expression != nullptr) {
		return expression->location();
	} else {
		return token->location;
	}
}

void Throw::pre_analyze(SemanticAnalyzer* analyzer) {
	if (expression != nullptr) {
		expression->pre_analyze(analyzer);
	}
}

void Throw::analyze(SemanticAnalyzer* analyzer, const Type*) {
	if (expression != nullptr) {
		expression->analyze(analyzer);
	}
}

#if COMPILER
Compiler::value Throw::compile(Compiler& c) const {

	auto exception = c.new_integer((int) vm::Exception::EXCEPTION);
	if (expression != nullptr) {
		exception = expression->compile(c);
	}
	c.mark_offset(token->location.start.line);

	c.insn_throw(exception);

	return { c.env };
}
#endif

std::unique_ptr<Instruction> Throw::clone() const {
	auto ex = expression ? expression->clone() : nullptr;
	return std::make_unique<Throw>(type->env, token, std::move(ex));
}

}
