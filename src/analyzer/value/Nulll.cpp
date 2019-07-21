#include "Nulll.hpp"
#include "../../vm/value/LSNull.hpp"
#include "../../type/Type.hpp"

namespace ls {

Nulll::Nulll(Token* token) : token(token) {
	type = Type::null;
	constant = true;
}

void Nulll::print(std::ostream& os, int, PrintOptions options) const {
	os << "null";
	if (options.debug) {
		os << " " << type;
	}
}

Location Nulll::location() const {
	return token->location;
}

#if COMPILER
Compiler::value Nulll::compile(Compiler& c) const {
	return c.new_null();
}
#endif

std::unique_ptr<Value> Nulll::clone() const {
	return std::make_unique<Nulll>(token);
}

}
