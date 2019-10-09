#include "Void_type.hpp"
#include "../colors.h"
#include "../environment/Environment.hpp"
#if COMPILER
#include "../compiler/Compiler.hpp"
#endif

namespace ls {

const Type* Void_type::element() const {
	return env.void_;
}
const Type* Void_type::key() const {
	return env.void_;
}
const Type* Void_type::return_type() const {
	return env.void_;
}

bool Void_type::operator == (const Type* type) const {
	return type == env.void_;
}
int Void_type::distance(const Type* type) const {
	if (dynamic_cast<const Void_type*>(type->folded)) { return 1; }
	return -1;
}
#if COMPILER
llvm::Type* Void_type::llvm(Compiler& c) const {
	return llvm::Type::getVoidTy(c.getContext());
}
#endif
std::string Void_type::class_name() const {
	return "";
}
Json Void_type::json() const {
	return {
		{ "name", "void" }
	};
}
std::ostream& Void_type::print(std::ostream& os) const {
	os << C_GREY << "void" << END_COLOR;
	return os;
}
Type* Void_type::clone() const {
	return new Void_type { env };
}

}