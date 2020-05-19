#include <iostream>
#include "Map_type.hpp"
#include "Any_type.hpp"
#include "../colors.h"
#include "Any_type.hpp"
#include "Struct_type.hpp"
#include "../environment/Environment.hpp"

namespace ls {

Map_type::Map_type(const Type* key, const Type* element) : Pointer_type(Type::structure("map<" + key->getName() + ", " + element->getName() + ">", {
	key->env.integer, // ?
	key->env.integer, // ?
	key->env.integer, // ?
	key->env.integer, // ?
	key->env.boolean, // native
	element->pointer(),
	element->pointer(),
	element->pointer(),
	Type::structure("map_node<" + key->getName() + ", " + element->getName() + ">", {
		key->env.long_, key->env.long_, key->env.long_, key->env.long_,
		key, element
	})->pointer()
})), _key(key), _element(element) {}

const Type* Map_type::key() const {
	return _key;
}
const Type* Map_type::element() const {
	return _element;
}
const Type* Map_type::iterator() const {
	const auto key_merged = _key->folded;
	const auto element_merged = _element->folded;
	return Type::structure("map_node<" + key_merged->getName() + ", " + element_merged->getName() + ">", {
		env.long_, env.long_, env.long_, env.long_, key_merged, element_merged
	})->pointer();
}
bool Map_type::operator == (const Type* type) const {
	if (auto map = dynamic_cast<const Map_type*>(type)) {
		return _element == map->_element && _key == map->_key;
	}
	return false;
}
int Map_type::distance(const Type* type) const {
	if (not temporary and type->temporary) return -1;
	if (dynamic_cast<const Any_type*>(type->folded)) { return 1000; }
	if (auto map = dynamic_cast<const Map_type*>(type->folded)) {
		if (map->element()->is_void() or map->key()->is_void()) {
			return 999;
		}
		return _element->distance(map->_element) + _key->distance(map->_key);
	}
	return -1;
}
std::string Map_type::class_name() const {
	return "Map";
}
const std::string Map_type::getName() const {
	if (_key == env.void_ && _element == env.void_) {
		return "map";
	}
	return "map<" + _key->getName() + ", " + _element->getName() + ">";
}
Json Map_type::json() const {
	return {
		{ "name", "array" },
		{ "key", _key->json() },
		{ "element", _element->json() }
	};
}
std::ostream& Map_type::print(std::ostream& os) const {
	os << BLUE_BOLD << "map" << END_COLOR << "<" << _key << ", " << _element << ">";
	return os;
}
Type* Map_type::clone() const {
	return new Map_type { _key, _element };
}

}