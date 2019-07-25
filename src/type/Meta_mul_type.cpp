#include "Meta_mul_type.hpp"
#include "../colors.h"
#include "../compiler/Compiler.hpp"

namespace ls {

bool Meta_mul_type::operator == (const Type* type) const {
	return false;
}
int Meta_mul_type::distance(const Type* type) const {
	return -1;
}
#if COMPILER
llvm::Type* Meta_mul_type::llvm(Compiler& c) const {
	return llvm::Type::getVoidTy(c.getContext());
}
#endif
std::string Meta_mul_type::class_name() const {
	return "";
}
std::ostream& Meta_mul_type::print(std::ostream& os) const {
	os << C_GREY << t1 << " * " << t2 << END_COLOR;
	return os;
}
Type* Meta_mul_type::clone() const {
	return new Meta_mul_type(t1, t2);
}

}