#include "Never_type.hpp"
#include "Type.hpp"
#include "../colors.h"

namespace ls {

Never_type::Never_type(Environment& env) : Any_type(env) {}

bool Never_type::operator == (const Type* type) const {
	return dynamic_cast<const Never_type*>(type);
}
int Never_type::distance(const Type* type) const {
	if (not temporary and type->temporary) return -1;
	if (dynamic_cast<const Never_type*>(type->folded)) { return 0; }
	return 100000 + type->distance(this);
}
std::ostream& Never_type::print(std::ostream& os) const {
	os << BLUE_BOLD << "never" << END_COLOR;
	return os;
}
Type* Never_type::clone() const {
	return new Never_type { env };
}

}