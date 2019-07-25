#include "Integer_type.hpp"
#include "../colors.h"
#include <iostream>
#include "../compiler/Compiler.hpp"
#include "Long_type.hpp"
#include "Any_type.hpp"
#include "Real_type.hpp"
#include "Mpz_type.hpp"
#include "Bool_type.hpp"
#include "Number_type.hpp"

namespace ls {

const Type* Integer_type::key() const {
	return Type::integer;
}
const Type* Integer_type::element() const {
	return Type::integer;
}
const Type* Integer_type::iterator() const {
	return Type::structure("int_iterator", {
		Type::integer,
		Type::integer,
		Type::integer
	});
}
bool Integer_type::operator == (const Type* type) const {
	return dynamic_cast<const Integer_type*>(type) != nullptr;
}
int Integer_type::distance(const Type* type) const {
	if (not temporary and type->temporary) return -1;
	if (dynamic_cast<const Integer_type*>(type->folded)) { return 0; }
	if (dynamic_cast<const Long_type*>(type->folded)) { return 1; }
	if (dynamic_cast<const Real_type*>(type->folded)) { return 2; }
	if (dynamic_cast<const Mpz_type*>(type->folded)) { return 3; }
	if (dynamic_cast<const Number_type*>(type->folded)) { return 4; }
	if (dynamic_cast<const Any_type*>(type->folded)) { return 5; }
	if (dynamic_cast<const Bool_type*>(type->folded)) { return 100; }
	return -1;
}
#if COMPILER
llvm::Type* Integer_type::llvm(Compiler& c) const {
	return llvm::Type::getInt32Ty(c.getContext());
}
#endif
std::string Integer_type::class_name() const {
	return "Number";
}
std::ostream& Integer_type::print(std::ostream& os) const {
	os << BLUE_BOLD << "int" << END_COLOR;
	return os;
}
Type* Integer_type::clone() const {
	return new Integer_type {};
}

}