#include "Number_type.hpp"
#include "../colors.h"
#include <iostream>
#include "Integer_type.hpp"
#include "Long_type.hpp"
#include "Real_type.hpp"
#include "Bool_type.hpp"
#include "Any_type.hpp"
#include "Mpz_type.hpp"
#include "Struct_type.hpp"

namespace ls {

Number_type::Number_type(Environment& env) : Type(env) {}

bool Number_type::operator == (const Type* type) const {
	return dynamic_cast<const Number_type*>(type);
}
int Number_type::distance(const Type* type) const {
	if (not temporary and type->temporary) return -1;
	if (dynamic_cast<const Bool_type*>(type->folded)) { return 500; }
	if (dynamic_cast<const Integer_type*>(type->folded)) { return 400; }
	if (dynamic_cast<const Long_type*>(type->folded)) { return 300; }
	if (dynamic_cast<const Real_type*>(type->folded)) { return 200; }
	if (dynamic_cast<const Mpz_type*>(type->folded)) { return 100; }
	if (dynamic_cast<const Any_type*>(type->folded)) { return 1; }
	if (dynamic_cast<const Number_type*>(type->folded)) { return 0; }
	if (dynamic_cast<const Pointer_type*>(type->folded) and ((Pointer_type*) type)->pointed()->is_mpz()) { return 100; }
	return -1;
}
std::string Number_type::class_name() const {
	return "Number";
}
Json Number_type::json() const {
	return {
		{ "name", "number" }
	};
}
#if COMPILER
llvm::Type* Number_type::llvm(Compiler& c) const {
	assert(false);
}
#endif
std::ostream& Number_type::print(std::ostream& os) const {
	os << BLUE_BOLD << "number" << END_COLOR;
	return os;
}
Type* Number_type::clone() const {
	return new Number_type { env };
}

}