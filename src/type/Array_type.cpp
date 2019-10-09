#include "Array_type.hpp"
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

Array_type::Array_type(const Type* element) : Pointer_type(Type::structure("array<" + element->getName() + ">", {
	element->env.integer, // ?
	element->env.integer, // ?
	element->env.integer, // ?
	element->env.integer, // refs
	element->env.boolean, // native
	element->pointer(), // vector.begin
	element->pointer(), // vector.end
	element->pointer() // vector.data
})), _element(element) {
}

const Type* Array_type::key() const {
	return env.integer;
}
const Type* Array_type::element() const {
	return _element;
}
bool Array_type::operator == (const Type* type) const {
	if (auto array = dynamic_cast<const Array_type*>(type)) {
		return _element == array->_element;
	}
	return false;
}
int Array_type::distance(const Type* type) const {
	if (not temporary and type->temporary) return -1;
	if (dynamic_cast<const Any_type*>(type->folded)) { return 1000; }
	if (auto array = dynamic_cast<const Array_type*>(type->folded)) {
		if (_element == env.never) return 0;
		if (array->_element == env.never) return 999;
		if (array->_element->is_void()) {
			return 999;
		}
		return _element->fold() == array->_element->fold() ? 0 : -1;
	}
	return -1;
}
const Type* Array_type::iterator() const {
	const auto merged = _element->fold();
	if (merged->is_bool()) return env.i8->pointer();
	if (merged->is_integer()) return env.integer->pointer();
	if (merged->is_long()) return env.long_->pointer();
	if (merged->is_real()) return env.real->pointer();
	return env.any->pointer();
}
std::string Array_type::class_name() const {
	return "Array";
}
const std::string Array_type::getName() const {
	return "array<" + _element->getName() + ">";
}
Json Array_type::json() const {
	return {
		{ "name", "array" },
		{ "element", _element->json() }
	};
}
std::ostream& Array_type::print(std::ostream& os) const {
	os << BLUE_BOLD << "array" << END_COLOR;
	if (_element != env.void_) {
		os << "<" << _element << ">";
	}
	return os;
}
Type* Array_type::clone() const {
	return new Array_type { _element };
}

}