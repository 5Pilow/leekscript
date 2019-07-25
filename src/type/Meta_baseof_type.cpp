#include "Meta_baseof_type.hpp"
#include "../colors.h"
#include "../compiler/Compiler.hpp"

namespace ls {

void Meta_baseof_type::reset() const {
	((Meta_baseof_type*) this)->result = nullptr;
	type->reset();
}
bool Meta_baseof_type::operator == (const Type* type) const {
	return false;
}
int Meta_baseof_type::distance(const Type* type) const {
	return -1;
}
#if COMPILER
llvm::Type* Meta_baseof_type::llvm(Compiler& c) const {
	return llvm::Type::getVoidTy(c.getContext());
}
#endif
std::string Meta_baseof_type::class_name() const {
	return "";
}
std::ostream& Meta_baseof_type::print(std::ostream& os) const {
	os << C_GREY << type << " : " << base << END_COLOR;
	return os;
}
Type* Meta_baseof_type::clone() const {
	return new Meta_baseof_type(type, base);
}

}