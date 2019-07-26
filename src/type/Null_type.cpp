#include "Null_type.hpp"
#include "Type.hpp"
#include "../colors.h"
#include "Pointer_type.hpp"
#include "Struct_type.hpp"
#include "Any_type.hpp"
#include "../environment/Environment.hpp"

namespace ls {

Null_type::Null_type(Environment& env) : Pointer_type(Type::structure("null", {
	env.integer, // ?
	env.integer, // ?
	env.integer, // ?
	env.integer, // refs
	env.boolean // native
}), true) {}

bool Null_type::operator == (const Type* type) const {
	return dynamic_cast<const Null_type*>(type);
}
int Null_type::distance(const Type* type) const {
	if (not temporary and type->temporary) return -1;
	if (dynamic_cast<const Any_type*>(type->folded)) { return 1; }
	if (dynamic_cast<const Null_type*>(type->folded)) { return 0; }
	return -1;
}
std::string Null_type::class_name() const {
	return "Null";
}
std::ostream& Null_type::print(std::ostream& os) const {
	os << BLUE_BOLD << "null" << END_COLOR;
	return os;
}
Type* Null_type::clone() const {
	return new Null_type { env };
}

}