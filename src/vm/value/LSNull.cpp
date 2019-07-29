#include "LSNull.hpp"
#include "LSClass.hpp"
#include "LSNumber.hpp"
#include "../VM.hpp"
#include "../../environment/Environment.hpp"

namespace ls {

LSValue* LSNull::null_var;

LSValue* LSNull::get() {
	return null_var;
}

LSNull::LSNull() : LSValue(LSValue::NULLL, 1, true) {}

LSNull::~LSNull() {}

bool LSNull::to_bool() const {
	return false;
}

bool LSNull::ls_not() const {
	return true;
}

bool LSNull::eq(const LSValue* v) const {
	return dynamic_cast<const LSNull*>(v);
}

bool LSNull::lt(const LSValue* v) const {
	if (dynamic_cast<const LSNull*>(v)) {
		return false;
	}
	return LSValue::lt(v);
}

std::ostream& LSNull::dump(std::ostream& os, int) const {
	os << "null";
	return os;
}

LSValue* LSNull::getClass(VM* vm) const {
	return vm->env.null_class.get();
}

}
