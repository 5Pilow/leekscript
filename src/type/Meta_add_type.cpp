#include "Meta_add_type.hpp"
#include "../colors.h"
#if COMPILER
#include "../compiler/Compiler.hpp"
#endif

namespace ls {

bool Meta_add_type::operator == (const Type* type) const {
	return false;
}
int Meta_add_type::distance(const Type* type) const {
	return -1;
}
#if COMPILER
llvm::Type* Meta_add_type::llvm(Compiler& c) const {
	return llvm::Type::getVoidTy(c.getContext());
}
#endif
std::string Meta_add_type::class_name() const {
	return "";
}
Json Meta_add_type::json() const {
	return {
		t1->json(),
		t2->json()
	};
}
std::ostream& Meta_add_type::print(std::ostream& os) const {
	os << C_GREY << t1 << " | " << t2 << END_COLOR;
	return os;
}
Type* Meta_add_type::clone() const {
	return new Meta_add_type(t1, t2);
}

}