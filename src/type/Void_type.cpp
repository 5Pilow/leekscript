#include "Void_type.hpp"
#include "../colors.h"
#include "../compiler/Compiler.hpp"

namespace ls {

bool Void_type::operator == (const Type* type) const {
	return type == Type::void_;
}
int Void_type::distance(const Type* type) const {
	if (dynamic_cast<const Void_type*>(type->folded)) { return 1; }
	return -1;
}
#if COMPILER
llvm::Type* Void_type::llvm(const Compiler& c) const {
	return llvm::Type::getVoidTy(c.getContext());
}
#endif
std::string Void_type::class_name() const {
	return "";
}
std::ostream& Void_type::print(std::ostream& os) const {
	os << C_GREY << "void" << END_COLOR;
	return os;
}
Type* Void_type::clone() const {
	return new Void_type();
}

}