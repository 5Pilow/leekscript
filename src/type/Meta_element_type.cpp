#include "Meta_element_type.hpp"
#include "../colors.h"
#if COMPILER
#include "../compiler/Compiler.hpp"
#endif

namespace ls {

bool Meta_element_type::operator == (const Type* type) const {
	return this->type == type;
}
int Meta_element_type::distance(const Type* type) const {
	return 0;
}
#if COMPILER
llvm::Type* Meta_element_type::llvm(Compiler& c) const {
	return llvm::Type::getVoidTy(c.getContext());
}
#endif
std::string Meta_element_type::class_name() const {
	return "";
}
Json Meta_element_type::json() const {
	return {
		{ "name", "element" },
		{ "element", type->json() }
	};
}
std::ostream& Meta_element_type::print(std::ostream& os) const {
	os << type << ".element";
	return os;
}
Type* Meta_element_type::clone() const {
	return new Meta_element_type(type);
}

}