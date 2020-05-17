#include "Meta_concat_type.hpp"
#include "../colors.h"
#if COMPILER
#include "../compiler/Compiler.hpp"
#endif

namespace ls {

bool Meta_concat_type::operator == (const Type* type) const {
	return false;
}
int Meta_concat_type::distance(const Type* type) const {
	return -1;
}
#if COMPILER
llvm::Type* Meta_concat_type::llvm(Compiler& c) const {
	return llvm::Type::getVoidTy(c.getContext());
}
#endif
std::string Meta_concat_type::class_name() const {
	return "";
}
Json Meta_concat_type::json() const {
	return {
		t1->json(),
		t2->json()
	};
}
std::ostream& Meta_concat_type::print(std::ostream& os) const {
	os << C_GREY << t1 << " + " << t2 << END_COLOR;
	return os;
}
Type* Meta_concat_type::clone() const {
	return new Meta_concat_type(t1, t2);
}

}