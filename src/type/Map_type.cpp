#include <iostream>
#include "Map_type.hpp"
#include "Any_type.hpp"
#include "../colors.h"
#include "Any_type.hpp"
#include "Struct_type.hpp"

namespace ls {

Map_type::Map_type(Type key, Type element) : Pointer_type(Type {
	new Struct_type(std::string("_map"), {
		Type::INTEGER, // ?
		Type::INTEGER, // ?
		Type::INTEGER, // ?
		Type::INTEGER, // ?
		Type::BOOLEAN, // native
		element.pointer(),
		element.pointer(),
		element.pointer(),
		Type({ new Struct_type("map_node", {
			Type::LONG, Type::LONG,	Type::LONG,	Type::LONG,
			element	
		}) }).pointer()
	})
}), _key(key), _element(element) {}

Type Map_type::key() const {
	return _key;
}
Type Map_type::element() const {
	return _element;
}
bool Map_type::operator == (const Base_type* type) const {
	if (auto map = dynamic_cast<const Map_type*>(type)) {
		return _element == map->_element && _key == map->_key;
	}
	return false;
}
bool Map_type::compatible(const Base_type* type) const {
	if (auto map = dynamic_cast<const Map_type*>(type)) {
		return _element.compatible(map->_element) && _key.compatible(map->_key);
	}
	return false;
}
int Map_type::distance(const Base_type* type) const {
	if (dynamic_cast<const Any_type*>(type)) { return 1000; }
	if (auto map = dynamic_cast<const Map_type*>(type)) {
		if (_element._types.size() == 0 or _key._types.size() == 0) {
			return 999;
		}
		return _element.distance(map->_element) + _key.distance(map->_key);
	}
	return -1;
}
std::string Map_type::clazz() const {
	return "Map";
}
std::ostream& Map_type::print(std::ostream& os) const {
	os << BLUE_BOLD << "map" << END_COLOR << "<" << _key << ", " << _element << ">";
	return os;
}

}