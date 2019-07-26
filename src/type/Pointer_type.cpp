#include "Pointer_type.hpp"
#include "Type.hpp"
#include "../colors.h"

namespace ls {

Pointer_type::Pointer_type(const Type* type, bool native) : Type(type->env, native), _type(type) {}

const Type* Pointer_type::pointed() const {
	return _type;
}
const Value* Pointer_type::function() const {
	return _type->function();
}
const Type* Pointer_type::return_type() const {
	return _type->return_type();
}
const Type* Pointer_type::argument(size_t i) const {
	return _type->argument(i);
}
const std::vector<const Type*>& Pointer_type::arguments() const {
	return _type->arguments();
}
bool Pointer_type::callable() const {
	if (_type->is_function()) {
		return true;
	}
	return false;
}
bool Pointer_type::operator == (const Type* type) const {
	if (auto p = dynamic_cast<const Pointer_type*>(type)) {
		return p->_type == _type;
	}
	return false;
}
int Pointer_type::distance(const Type* type) const {
	if (not temporary and type->temporary) return -1;
	auto p = dynamic_cast<const Pointer_type*>(type->folded);
	if (_type->is_mpz()) {
		if (p and p->pointed()->is_mpz()) return 0;
		if (not type->is_mpz()) {
			return _type->distance(type->folded);
		}
	}
	if (_type->is_function()) {
		if (p and p->pointed()->is_function()) return 0;
		return _type->distance(type->folded);
	}
	if (p) {
		return _type->distance(p->_type);
	}
	return -1;
}
std::string Pointer_type::class_name() const {
	return _type->class_name();
}
#if COMPILER
llvm::Type* Pointer_type::llvm(Compiler& c) const {
	if (_llvm_type == nullptr) {
		((Pointer_type*) this)->_llvm_type = _type->llvm(c)->getPointerTo();
	}
	return _llvm_type;
}
#endif
std::ostream& Pointer_type::print(std::ostream& os) const {
	os << _type << BLUE_BOLD << "*" << END_COLOR;
	return os;
}
Type* Pointer_type::clone() const {
	return new Pointer_type { _type };
}

}