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

bool Real_type::operator == (const Base_type* type) const {
	return dynamic_cast<const Real_type*>(type) != nullptr;
}
int Real_type::distance(const Base_type* type) const {
	if (dynamic_cast<const Any_type*>(type)) { return 5; }
	if (dynamic_cast<const Number_type*>(type)) { return 4; }
	if (dynamic_cast<const Mpz_type*>(type)) { return 3; }
	if (dynamic_cast<const Real_type*>(type)) { return 0; }
	if (dynamic_cast<const Long_type*>(type)) { return 100; }
	if (dynamic_cast<const Integer_type*>(type)) { return 101; }
	if (dynamic_cast<const Bool_type*>(type)) { return 102; }
	return -1;
}
llvm::Type* Real_type::llvm() const {
	return llvm::Type::getDoubleTy(LLVMCompiler::context);
}
std::ostream& Real_type::print(std::ostream& os) const {
	os << BLUE_BOLD << "real" << END_COLOR;
	return os;
}

}