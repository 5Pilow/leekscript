#include "Boolean.hpp"
#include "../../type/Type.hpp"
#include "../semantic/SemanticAnalyzer.hpp"

namespace ls {

Boolean::Boolean(Environment& env, Token* token) : Value(env), token(token) {
	this->value = token->type == TokenType::TRUE;
	type = env.boolean;
	constant = true;
}

void Boolean::print(std::ostream& os, int, PrintOptions options) const {
	os << (value ? "true" : "false");
	if (options.debug) {
		os << " " << type;
	}
}

Location Boolean::location() const {
	return token->location;
}

#if COMPILER
Compiler::value Boolean::compile(Compiler& c) const {
	return c.new_bool(value);
}
#endif

std::unique_ptr<Value> Boolean::clone() const {
	auto b = std::make_unique<Boolean>(type->env, token);
	b->value = value;
	return b;
}

}
