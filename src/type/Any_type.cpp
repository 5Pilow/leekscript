#include "Any_type.hpp"
#include "Type.hpp"
#include "../colors.h"
#include "Struct_type.hpp"
#include "../environment/Environment.hpp"

namespace ls {

Any_type::Any_type(Environment& env, bool native) : Pointer_type(Type::structure("any", {
	env.integer, // ?
	env.integer, // ?
	env.integer, // ?
	env.integer, // refs
	env.boolean // native
}), native) {}

bool Any_type::operator == (const Type* type) const {
	return dynamic_cast<const Any_type*>(type);
}
int Any_type::distance(const Type* type) const {
	if (not temporary and type->temporary) return -1;
	if (dynamic_cast<const Any_type*>(type->folded)) { return 0; }
	return 100000 + type->distance(this);
}
std::string Any_type::class_name() const {
	return "Value";
}
std::ostream& Any_type::print(std::ostream& os) const {
	os << BLUE_BOLD << "any" << END_COLOR;
	return os;
}
Type* Any_type::clone() const {
	return new Any_type { env };
}

}