#include "Set_type.hpp"
#include "../colors.h"
#include <iostream>
#include "Any_type.hpp"
#include "Struct_type.hpp"
#include "../environment/Environment.hpp"

namespace ls {

std::unordered_map<const Type*, const Type*> Set_type::nodes;
std::unordered_map<const Type*, const Type*> Set_type::iterators;

Set_type::Set_type(const Type* element) : Pointer_type(Type::structure("set<" + element->getName() + ">", {
	element->env.integer, // ?
	element->env.integer, // ?
	element->env.integer, // ?
	element->env.integer, // ?
	element->env.boolean, // native
	element->pointer(),
	element->pointer(),
	element->pointer(),
	get_node_type(element)
})), _element(element) {}

const Type* Set_type::key() const {
	return env.integer;
}
const Type* Set_type::element() const {
	return _element;
}
const Type* Set_type::iterator() const {
	const auto merged = _element->fold();
	return get_iterator(merged);
}
bool Set_type::operator == (const Type* type) const {
	if (auto array = dynamic_cast<const Set_type*>(type)) {
		return _element == array->_element;
	}
	return false;
}
int Set_type::distance(const Type* type) const {
	if (not temporary and type->temporary) return -1;
	if (dynamic_cast<const Any_type*>(type->folded)) { return 1000; }
	if (auto set = dynamic_cast<const Set_type*>(type->folded)) {
		if (set->_element->is_void()) {
			return 999;
		}
		return _element->distance(set->_element);
	}
	return -1;
}
std::string Set_type::class_name() const {
	return "Set";
}
const std::string Set_type::getName() const {
	if (_element == env.void_) {
		return "set";
	}
	return "set<" + _element->getName() + ">";
}
Json Set_type::json() const {
	return {
		{ "name", "set" },
		{ "element", _element->json() }
	};
}
std::ostream& Set_type::print(std::ostream& os) const {
	os << BLUE_BOLD << "set" << END_COLOR << "<" << _element << ">";
	return os;
}
Type* Set_type::clone() const {
	return new Set_type { _element };
}

const Type* Set_type::get_iterator(const Type* element) {
	auto i = iterators.find(element);
	if (i != iterators.end()) return i->second;
	auto type = Type::structure("set_iterator<" + element->getName() + ">", {
		get_node_type(element),
		element->env.integer
	});
	iterators.insert({ element, type });
	return type;
}

const Type* Set_type::get_node_type(const Type* element) {
	auto i = nodes.find(element);
	if (i != nodes.end()) return i->second;
	auto& env = element->env;
	auto type = Type::structure("set_node<" + element->getName() + ">", {
		env.long_, env.long_, env.long_, env.long_,
		element
	})->pointer();
	nodes.insert({ element, type });
	return type;
}

}