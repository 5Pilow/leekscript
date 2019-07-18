#include "Real_type.hpp"
#include "../colors.h"
#include "../compiler/Compiler.hpp"
#include "Number_type.hpp"
#include "Any_type.hpp"
#include "Mpz_type.hpp"
#include "Bool_type.hpp"
#include "Integer_type.hpp"
#include "Long_type.hpp"

namespace ls {

bool Real_type::operator == (const Type* type) const {
	return dynamic_cast<const Real_type*>(type) != nullptr;
}
int Real_type::distance(const Type* type) const {
	if (not temporary and type->temporary) return -1;
	if (dynamic_cast<const Real_type*>(type->folded)) { return 0; }
	if (dynamic_cast<const Mpz_type*>(type->folded)) { return 3; }
	if (dynamic_cast<const Bool_type*>(type->folded)) { return 102; }
	if (dynamic_cast<const Integer_type*>(type->folded)) { return 101; }
	if (dynamic_cast<const Long_type*>(type->folded)) { return 100; }
	if (dynamic_cast<const Number_type*>(type->folded)) { return 4; }
	if (dynamic_cast<const Any_type*>(type->folded)) { return 5; }
	return -1;
}
#if COMPILER
llvm::Type* Real_type::llvm(const Compiler& c) const {
	return llvm::Type::getDoubleTy(c.getContext());
}
#endif
std::string Real_type::class_name() const {
	return "Number";
}
std::ostream& Real_type::print(std::ostream& os) const {
	os << BLUE_BOLD << "real" << END_COLOR;
	return os;
}
Type* Real_type::clone() const {
	return new Real_type {};
}

}