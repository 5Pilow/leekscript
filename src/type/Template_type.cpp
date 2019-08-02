#include "Template_type.hpp"
#include "../colors.h"
#include "Type.hpp"
#include "Any_type.hpp"
#include "../environment/Environment.hpp"
#if COMPILER
#include "../compiler/Compiler.hpp"
#endif

namespace ls {

Template_type::Template_type(Environment& env, const std::string name) : Any_type(env), _name(name), _implementation(env.void_) {}

void Template_type::reset() const {
	((Template_type*) this)->_implementation = _implementation->env.void_;
}
void Template_type::implement(const Type* implementation) const {
	((Template_type*) this)->_implementation = implementation;
}
bool Template_type::operator == (const Type* type) const {
	if (this == type) return true;
	if (_implementation == _implementation->env.void_) return false;
	return _implementation->operator == (type);
}
#if COMPILER
llvm::Type* Template_type::llvm(Compiler& c) const {
	// assert(_implementation._types.size() > 0);
	// return _implementation._types[0]->llvm();
	return llvm::Type::getInt32Ty(c.getContext());
}
#endif
std::ostream& Template_type::print(std::ostream& os) const {
	os << BLUE_BOLD << _name;
	if (_implementation != _implementation->env.void_) {
		os << "." << _implementation;
	}
	os << END_COLOR;
	return os;
}
Type* Template_type::clone() const {
	return new Template_type { env, _name };
}

}