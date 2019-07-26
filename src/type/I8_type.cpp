#include "I8_type.hpp"
#include "../colors.h"
#include <iostream>
#include "../compiler/Compiler.hpp"
#include "Long_type.hpp"
#include "Any_type.hpp"
#include "Real_type.hpp"
#include "Mpz_type.hpp"
#include "Bool_type.hpp"
#include "Integer_type.hpp"
#include "Number_type.hpp"

namespace ls {

bool I8_type::operator == (const Type* type) const {
	return dynamic_cast<const I8_type*>(type) != nullptr;
}
int I8_type::distance(const Type* type) const {
	if (not temporary and type->temporary) return -1;
	if (dynamic_cast<const I8_type*>(type->folded)) { return 0; }
	if (dynamic_cast<const Integer_type*>(type->folded)) { return 1; }
	if (dynamic_cast<const Long_type*>(type->folded)) { return 2; }
	if (dynamic_cast<const Real_type*>(type->folded)) { return 3; }
	if (dynamic_cast<const Mpz_type*>(type->folded)) { return 4; }
	if (dynamic_cast<const Number_type*>(type->folded)) { return 5; }
	if (dynamic_cast<const Any_type*>(type->folded)) { return 6; }
	if (dynamic_cast<const Bool_type*>(type->folded)) { return 100; }
	return -1;
}
#if COMPILER
llvm::Type* I8_type::llvm(Compiler& c) const {
	return llvm::Type::getInt8Ty(c.getContext());
}
#endif
std::string I8_type::class_name() const {
	return "Number";
}
std::ostream& I8_type::print(std::ostream& os) const {
	os << BLUE_BOLD << "i8" << END_COLOR;
	return os;
}
Type* I8_type::clone() const {
	return new I8_type { env };
}

}