#include "LSBoolean.hpp"
#include "LSNull.hpp"
#include "LSClass.hpp"
#include "LSString.hpp"
#include "LSNumber.hpp"
#include "../VM.hpp"
#include "../../environment/Environment.hpp"

namespace ls {

LSBoolean* LSBoolean::false_val;
LSBoolean* LSBoolean::true_val;

LSBoolean::LSBoolean(bool value) : LSValue(BOOLEAN, 1, true), value(value) {}

LSBoolean::~LSBoolean() {}

bool LSBoolean::to_bool() const {
	return value;
}

bool LSBoolean::ls_not() const {
	return !value;
}

LSValue* LSBoolean::ls_tilde() {
	return LSNumber::get(~(int) value);
}

LSValue* LSBoolean::ls_minus() {
	return LSNumber::get(-value);
}

LSValue* LSBoolean::add(LSValue* v) {
	if (v->type == NUMBER) {
		auto number = static_cast<LSNumber*>(v);
		if (this->value) {
			if (number->refs == 0) {
				number->value += 1;
				return number;
			}
			auto r = LSNumber::get(number->value + 1);
			LSValue::delete_temporary(number);
			return r;
		}
		return number;
	}
	if (v->type == BOOLEAN) {
		auto boolean = static_cast<LSBoolean*>(v);
		return LSNumber::get(value + boolean->value);
	}
	if (v->type == STRING) {
		auto string = static_cast<LSString*>(v);
		auto res = new LSString((value ? "true" : "false") + *string);
		LSValue::delete_temporary(string);
		return res;
	}
	LSValue::delete_temporary(v);
	return LSNull::get();
}

LSValue* LSBoolean::sub(LSValue* v) {
	if (v->type == NUMBER) {
		auto number = static_cast<LSNumber*>(v);
		if (this->value) {
			if (number->refs == 0) {
				number->value = 1 - number->value;
				return number;
			}
			return LSNumber::get(1 - number->value);
		}
		if (number->refs == 0) {
			number->value = -number->value;
			return number;
		}
		LSValue::delete_temporary(this);
		return LSNumber::get(-number->value);
	}
	if (v->type == BOOLEAN) {
		auto boolean = static_cast<LSBoolean*>(v);
		return LSNumber::get(value - boolean->value);
	}
	LSValue::delete_temporary(v);
	return LSNull::get();
}

bool LSBoolean::eq(const LSValue* v) const {
	if (v->type == BOOLEAN) {
		auto boolean = static_cast<const LSBoolean*>(v);
		return boolean->value == this->value;
	}
	return v->type == NUMBER and static_cast<const LSNumber*>(v)->value == this->value;
}

bool LSBoolean::lt(const LSValue* v) const {
	if (v->type == BOOLEAN) {
		auto boolean = static_cast<const LSBoolean*>(v);
		return this->value < boolean->value;
	}
	return LSValue::lt(v);
}

bool LSBoolean::operator < (int) const {
	return true;
}

bool LSBoolean::operator < (double) const {
	return true;
}

int LSBoolean::abso() const {
	return value;
}

LSValue* LSBoolean::clone() const {
	return (LSValue*) this;
}

std::ostream& LSBoolean::dump(std::ostream& os, int) const {
	os << (value ? "true" : "false");
	return os;
}

std::string LSBoolean::json() const {
	return value ? "true" : "false";
}

LSValue* LSBoolean::getClass(VM* vm) const {
	return vm->env.boolean_class.get();
}

}
