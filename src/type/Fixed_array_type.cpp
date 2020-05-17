#include "Fixed_array_type.hpp"
#include "../colors.h"
#include "Type.hpp"
#include <iostream>
#include "Struct_type.hpp"
#include "Any_type.hpp"
#include "../environment/Environment.hpp"
#if COMPILER
#include "../compiler/Compiler.hpp"
#endif

namespace ls {

Fixed_array_type::Fixed_array_type(const std::vector<const Type*>& elements) : Array_type(Type::compound(elements)), _elements(elements), _element(Type::compound(elements)) {
}

const Type* Fixed_array_type::key() const {
	return env.integer;
}

const Type* Fixed_array_type::element() const {
	return _element;
}

const Type* Fixed_array_type::element(size_t index) const {
	assert(index >= 0 && index < _elements.size());
	return _elements[index];
}

const std::vector<const Type*>& Fixed_array_type::elements() const {
	return _elements;
}

size_t Fixed_array_type::size() const {
	return _elements.size();
}

bool Fixed_array_type::operator == (const Type* type) const {
	if (auto array = dynamic_cast<const Array_type*>(type)) {
		return _element == array->element();
	}
	return false;
}
int Fixed_array_type::distance(const Type* type) const {
	if (not temporary and type->temporary) return -1;
	if (dynamic_cast<const Any_type*>(type->folded)) { return 1000; }
	if (auto array = dynamic_cast<const Fixed_array_type*>(type->folded)) {
		if (array->size() == 1 and array->element(0) == env.void_) return 0;
		if (_element == env.never) return 0;
		if (array->element() == env.never) return 999;
		if (array->element()->is_void()) {
			return 999;
		}
		return _element->fold() == array->element()->fold() ? 0 : -1;
	}
	return -1;
}
const Type* Fixed_array_type::iterator() const {
	const auto merged = _element->fold();
	if (merged->is_bool()) return env.i8->pointer();
	if (merged->is_integer()) return env.integer->pointer();
	if (merged->is_long()) return env.long_->pointer();
	if (merged->is_real()) return env.real->pointer();
	return env.any->pointer();
}
std::string Fixed_array_type::class_name() const {
	return "Array";
}
const std::string Fixed_array_type::getName() const {
	std::string name = "array[";
	for (size_t i = 0; i < _elements.size(); ++i) {
		if (i > 0) { name += ", "; }
		name += _elements[i]->getName();
	}
	name += "]";
	return name;
}

Json Fixed_array_type::json() const {
	Json elements;
	for (const auto& arg : _elements) {
		elements.push_back(arg->json());
	}
	return {
		{ "name", "array" },
		{ "elements", elements }
	};
}

std::ostream& Fixed_array_type::print(std::ostream& os) const {
	os << BLUE_BOLD << "array[" << END_COLOR;
	for (size_t i = 0; i < _elements.size(); ++i) {
		if (i > 0) { os << BLUE_BOLD << ", " << END_COLOR; }
		os << _elements[i];
	}
	os << BLUE_BOLD << "]" << END_COLOR;
	return os;
}

Type* Fixed_array_type::clone() const {
	return new Fixed_array_type { _elements };
}

}