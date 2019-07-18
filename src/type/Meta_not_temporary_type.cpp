#include "Meta_not_temporary_type.hpp"
#include "../colors.h"
#include "../compiler/Compiler.hpp"

namespace ls {

bool Meta_not_temporary_type::operator == (const Type* type) const {
	return false;
}
int Meta_not_temporary_type::distance(const Type* type) const {
	return -1;
}
#if COMPILER
llvm::Type* Meta_not_temporary_type::llvm(const Compiler& c) const {
	return llvm::Type::getVoidTy(c.getContext());
}
#endif
std::string Meta_not_temporary_type::class_name() const {
	return "";
}
std::ostream& Meta_not_temporary_type::print(std::ostream& os) const {
	os << C_GREY << "not_tmp(" << type << ")" << END_COLOR;
	return os;
}
Type* Meta_not_temporary_type::clone() const {
	return new Meta_not_temporary_type(type);
}

}