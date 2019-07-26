#include "Placeholder_type.hpp"
#include "../colors.h"
#include "Type.hpp"
#include "Any_type.hpp"
#include "Meta_element_type.hpp"

namespace ls {

void Placeholder_type::implement(const Type* implementation) const {
	((Placeholder_type*) this)->_implementation = implementation;
}
const Type* Placeholder_type::element() const {
	if (_element) return _element;
	((Placeholder_type*) this)->_element = new Meta_element_type(this);
	return _element;
}
bool Placeholder_type::operator == (const Type* type) const {
	return this == type;
}
int Placeholder_type::distance(const Type* type) const {
	return 0;
}
std::ostream& Placeholder_type::print(std::ostream& os) const {
	os << BLUE_BOLD << _name;
	if (_implementation != nullptr) {
		os << "." << _implementation;
	}
	os << END_COLOR;
	return os;
}
Type* Placeholder_type::clone() const {
	return new Placeholder_type { env, _name };
}

}