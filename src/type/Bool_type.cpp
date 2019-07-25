#include "Bool_type.hpp"
#include "../colors.h"
#include "../compiler/Compiler.hpp"
#include "Integer_type.hpp"
#include "Long_type.hpp"
#include "Real_type.hpp"
#include "Bool_type.hpp"
#include "Number_type.hpp"
#include "Any_type.hpp"
#include "Mpz_type.hpp"

namespace ls {

bool Bool_type::operator == (const Type* type) const {
	return dynamic_cast<const Bool_type*>(type) != nullptr;
}
int Bool_type::distance(const Type* type) const {
	if (not temporary and type->temporary) return -1;
	if (dynamic_cast<const Bool_type*>(type->folded)) { return 0; }
	if (dynamic_cast<const Integer_type*>(type->folded)) { return 101; }
	if (dynamic_cast<const Long_type*>(type->folded)) { return 102; }
	if (dynamic_cast<const Real_type*>(type->folded)) { return 103; }
	if (dynamic_cast<const Mpz_type*>(type->folded)) { return 104; }
	if (dynamic_cast<const Number_type*>(type->folded)) { return 105; }
	if (dynamic_cast<const Any_type*>(type->folded)) { return 106; }
	return -1;
}
#if COMPILER
llvm::Type* Bool_type::llvm(Compiler& c) const {
	return llvm::Type::getInt1Ty(c.getContext());
}
#endif
std::string Bool_type::class_name() const {
	return "Boolean";
}
std::ostream& Bool_type::print(std::ostream& os) const {
	os << BLUE_BOLD << "bool" << END_COLOR;
	return os;
}
Type* Bool_type::clone() const {
	return new Bool_type {};
}

}