#include "Template_type.hpp"
#include "../colors.h"
#include "Type.hpp"
#include "Any_type.hpp"
#include "../environment/Environment.hpp"
#if COMPILER
#include "../compiler/Compiler.hpp"
#endif

namespace ls {

Template_type::Template_type(Environment& env, const std::string name) : Type(env), _name(name), _implementation(env.void_) {}

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
int Template_type::distance(const Type* type) const {
	if (_implementation == _implementation->env.void_) {
		if (dynamic_cast<const Any_type*>(type->folded)) { return 0; }
		return 100000 + type->distance(this);
	}
	return _implementation->distance(type);
}
#if COMPILER
llvm::Type* Template_type::llvm(Compiler& c) const {
	// assert(_implementation._types.size() > 0);
	// return _implementation._types[0]->llvm();
	return llvm::Type::getInt32Ty(c.getContext());
}
#endif

const std::string Template_type::getJsonName() const {
	return "Template";
}
std::string Template_type::class_name() const {
	return "Template";
}
const std::string Template_type::getName() const {
	return "Template";
}
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