#include "Boolean.hpp"
#include "../../vm/value/LSBoolean.hpp"
#include "../../type/Type.hpp"

namespace ls {

Boolean::Boolean(Token* token) : token(token) {
	this->value = token->type == TokenType::TRUE;
	type = Type::boolean;
	constant = true;
}

void Boolean::print(std::ostream& os, int, bool debug, bool condensed) const {
	os << (value ? "true" : "false");
	if (debug) {
		os << " " << type;
	}
}

Location Boolean::location() const {
	return token->location;
}

Compiler::value Boolean::compile(Compiler& c) const {
	return c.new_bool(value);
}

std::unique_ptr<Value> Boolean::clone() const {
	auto b = std::make_unique<Boolean>(token);
	b->value = value;
	return b;
}

}
