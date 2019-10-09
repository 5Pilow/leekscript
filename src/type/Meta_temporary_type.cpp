#include "Meta_temporary_type.hpp"
#include "../colors.h"
#if COMPILER
#include "../compiler/Compiler.hpp"
#endif

namespace ls {

bool Meta_temporary_type::operator == (const Type* type) const {
	return false;
}
int Meta_temporary_type::distance(const Type* type) const {
	return -1;
}
#if COMPILER
llvm::Type* Meta_temporary_type::llvm(Compiler& c) const {
	return llvm::Type::getVoidTy(c.getContext());
}
#endif
std::string Meta_temporary_type::class_name() const {
	return "";
}
Json Meta_temporary_type::json() const {
	return type->json();
}
std::ostream& Meta_temporary_type::print(std::ostream& os) const {
	os << C_GREY << "tmp(" << type << ")" << END_COLOR;
	return os;
}
Type* Meta_temporary_type::clone() const {
	return new Meta_temporary_type(type);
}

}