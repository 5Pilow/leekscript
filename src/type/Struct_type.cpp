#include "Struct_type.hpp"
#include "../colors.h"
#include <iostream>
#include "Type.hpp"
#include "Any_type.hpp"

namespace ls {

Struct_type::Struct_type(const std::string name, std::initializer_list<const Type*> types) : _name(name), _types(types) {}
Struct_type::Struct_type(const std::string name, std::vector<const Type*> types) : _name(name), _types(types) {}

const Type* Struct_type::member(int p) const {
	return _types.at(p);
}
bool Struct_type::operator == (const Type* type) const {
	if (auto s = dynamic_cast<const Struct_type*>(type)) {
		return s->_types == _types;
	}
	return false;
}
int Struct_type::distance(const Type* type) const {
	if (not temporary and type->temporary) return -1;
	if (dynamic_cast<const Any_type*>(type->folded)) { return 1; }
	if (auto s = dynamic_cast<const Struct_type*>(type->folded)) {
		if (s->_types == _types) {
			return 0;
		}
	}
	return -1;
}
#if COMPILER
llvm::Type* Struct_type::llvm(const Compiler& c) const {
	if (_llvm_type == nullptr) {
		std::vector<llvm::Type*> llvm_types;
		for (const auto& type : _types) {
			llvm_types.push_back(type->llvm(c));
		}
		((Struct_type*) this)->_llvm_type = llvm::StructType::create(llvm_types, _name);
	}
	return _llvm_type;
}
#endif
std::string Struct_type::class_name() const {
	return "Struct";
}
std::ostream& Struct_type::print(std::ostream& os) const {
	os << BLUE_BOLD << _name << END_COLOR << " { ";
	for (size_t i = 0; i < _types.size(); ++i) {
		if (i > 0) os << ", ";
		os << _types[i];
	}
	os << " }";
	return os;
}
Type* Struct_type::clone() const {
	return new Struct_type { _name, _types };
}

}