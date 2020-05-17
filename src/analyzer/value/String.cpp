#include "String.hpp"
#include "../semantic/SemanticAnalyzer.hpp"
#include "../../type/Type.hpp"

namespace ls {

String::String(Environment& env, Token* token) : Value(env), token(token) {
	type = env.tmp_string;
	constant = true;
}

void String::print(std::ostream& os, int, PrintOptions options) const {
	os << "'" << token->content << "'";
	if (options.debug) {
		os << " " << type;
	}
}

Location String::location() const {
	return token->location;
}

bool String::will_store(SemanticAnalyzer* analyzer, const Type* type) {
	if (!type->is_string()) {
		analyzer->add_error({Error::Type::NO_SUCH_OPERATOR, ErrorLevel::ERROR, location(), location(), {this->type->to_string(), "=", type->to_string()}});
	}
	return false;
}

#if COMPILER
Compiler::value String::compile(Compiler& c) const {
	auto s = c.new_const_string(token->content);
	return c.insn_call(c.env.tmp_string, {s}, "String.new.1");
}
#endif

std::unique_ptr<Value> String::clone(Block* parent) const {
	return std::make_unique<String>(type->env, token);
}

}
