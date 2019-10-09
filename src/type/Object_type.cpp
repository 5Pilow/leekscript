#include "Object_type.hpp"
#include "../colors.h"
#include "Type.hpp"
#include "Struct_type.hpp"
#include "Any_type.hpp"
#include "../environment/Environment.hpp"

namespace ls {

Object_type::Object_type(Environment& env, bool native) : Pointer_type(Type::structure("object", {
	env.integer, // ?
	env.integer, // ?
	env.integer, // ?
	env.integer, // refs
	env.boolean // native
}), native) {}

bool Object_type::operator == (const Type* type) const {
	return dynamic_cast<const Object_type*>(type);
}
int Object_type::distance(const Type* type) const {
	if (not temporary and type->temporary) return -1;
	if (dynamic_cast<const Any_type*>(type->folded)) { return 1; }
	if (dynamic_cast<const Object_type*>(type->folded)) { return 0; }
	return -1;
}
std::string Object_type::class_name() const {
	return "Object";
}
Json Object_type::json() const {
	return {
		{ "name", "object" }
	};
}
std::ostream& Object_type::print(std::ostream& os) const {
	os << BLUE_BOLD << "object" << END_COLOR;
	return os;
}
Type* Object_type::clone() const {
	return new Object_type { env };
}

}