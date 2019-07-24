#include <cmath>
#include <sstream>
#include "LSMpz.hpp"
#include "LSNumber.hpp"
#include "LSNull.hpp"
#include "LSFunction.hpp"
#include "LSBoolean.hpp"
#include "LSString.hpp"
#include "LSClass.hpp"
#include "LSClosure.hpp"
#include "../VM.hpp"

namespace ls {

LSClass* LSMpz::clazz;

LSMpz* LSMpz::get_from_mpz(VM* vm, __mpz_struct i) {
	return new LSMpz(vm, i);
}
LSMpz* LSMpz::get_from_tmp(VM* vm, __mpz_struct i) {
	auto mpz = new LSMpz(vm);
	mpz->value = i;
	return mpz;
}
LSMpz* LSMpz::get(VM* vm) {
	auto mpz = new LSMpz(vm);
	mpz_init(&mpz->value);
	vm->mpz_created++;
	return mpz;
}
LSMpz* LSMpz::get(VM* vm, long i) {
	return new LSMpz(vm, i);
}

LSMpz::LSMpz(VM* vm) : LSValue(MPZ), vm(vm) {
	// Value not initialized
}

LSMpz::LSMpz(VM* vm, __mpz_struct value) : LSValue(MPZ), vm(vm) {
	mpz_init_set(&this->value, &value);
	vm->mpz_created++;
}
LSMpz::LSMpz(VM* vm, long l) : LSValue(MPZ), vm(vm) {
	mpz_init_set_si(&value, l);
	vm->mpz_created++;
}

LSMpz::~LSMpz() {
	mpz_clear(&value);
	vm->mpz_deleted++;
}

/*
 * LSValue methods
 */
bool LSMpz::to_bool() const {
	return mpz_cmp_ui(&value, 0);
}

bool LSMpz::ls_not() const {
	bool r = !mpz_cmp_ui(&value, 0);
	LSValue::delete_temporary(this);
	return r;
}

LSValue* LSMpz::ls_minus() {
	if (refs == 0) {
		mpz_neg(&value, &value);
		return this;
	}
	auto r = new LSMpz(vm);
	mpz_neg(&r->value, &value);
	return r;
}

LSValue* LSMpz::ls_tilde() {
	if (refs == 0) {
		mpz_com(&value, &value);
		return this;
	}
	auto r = new LSMpz(vm);
	mpz_com(&r->value, &value);
	return r;
}

LSValue* LSMpz::ls_preinc() {
	mpz_add_ui(&value, &value, 1);
	return this;
}

LSValue* LSMpz::ls_inc() {
	auto r = LSMpz::get_from_mpz(vm, value);
	mpz_add_ui(&value, &value, 1);
	return r;
}

LSValue* LSMpz::ls_predec() {
	mpz_sub_ui(&value, &value, 1);
	return this;
}

LSValue* LSMpz::ls_dec() {
	auto r = LSMpz::get_from_mpz(vm, value);
	mpz_sub_ui(&value, &value, 1);
	return r;
}

LSValue* LSMpz::add(LSValue* v) {
	if (v->type == NUMBER) {
		auto number = static_cast<LSNumber*>(v);
		if (refs == 0) {
			mpz_add_ui(&value, &value, number->value);
			LSValue::delete_temporary(number);
			return this;
		}
		auto r = LSMpz::get(vm);
		mpz_add_ui(&r->value, &value, number->value);
		LSValue::delete_temporary(number);
		return r;
	}
	if (v->type == BOOLEAN) {
		auto boolean = static_cast<LSBoolean*>(v);
		if (boolean->value) {
			if (refs == 0) {
				mpz_add_ui(&value, &value, 1);
				return this;
			}
			auto r = new LSMpz(vm);
			mpz_add_ui(&r->value, &value, 1);
			return r;
		}
		return this;
	}
	if (v->type != STRING) {
		LSValue::delete_temporary(this);
		LSValue::delete_temporary(v);
		throw vm::ExceptionObj(vm::Exception::NO_SUCH_OPERATOR);
	}
	auto string = static_cast<LSString*>(v);
	LSValue* r = new LSString("TODO" + *string);
	LSValue::delete_temporary(this);
	LSValue::delete_temporary(string);
	return r;
}

LSValue* LSMpz::add_eq(LSValue* v) {
	if (v->type == NUMBER) {
		auto number = static_cast<LSNumber*>(v);
		// value += number->value;
		if (number->refs == 0) delete number;
		return this;
	}
	if (v->type != BOOLEAN) {
		LSValue::delete_temporary(v);
		throw vm::ExceptionObj(vm::Exception::NO_SUCH_OPERATOR);
	}
	auto boolean = static_cast<LSBoolean*>(v);
	mpz_add_ui(&value, &value, boolean->value);
	return this;
}

LSValue* LSMpz::sub(LSValue* v) {
	if (v->type == NUMBER) {
		auto number = static_cast<LSNumber*>(v);
		if (refs == 0) {
			// value -= number->value;
			if (number->refs == 0) delete number;
			return this;
		}
		if (number->refs == 0) {
			// number->value = this->value - number->value;
			return number;
		}
		// return LSMpz::get(this->value - number->value);
		return LSMpz::get(0);
	}
	if (v->type != BOOLEAN) {
		LSValue::delete_temporary(this);
		LSValue::delete_temporary(v);
		throw vm::ExceptionObj(vm::Exception::NO_SUCH_OPERATOR);
	}
	auto boolean = static_cast<LSBoolean*>(v);
	if (boolean->value) {
		if (refs == 0) {
			mpz_sub_ui(&value, &value, 1);
			return this;
		}
		// return LSMpz::get(value - 1);
		return LSMpz::get(0);
	}
	return this;
}

LSValue* LSMpz::sub_eq(LSValue* v) {
	if (v->type == NUMBER) {
		auto number = static_cast<LSNumber*>(v);
		// value -= number->value;
		LSValue::delete_temporary(number);
		return this;
	}
	if (v->type != BOOLEAN) {
		LSValue::delete_temporary(v);
		throw vm::ExceptionObj(vm::Exception::NO_SUCH_OPERATOR);
	}
	// auto boolean = static_cast<LSBoolean*>(v);
	// value -= boolean->value;
	return this;
}

LSValue* LSMpz::mul(LSValue* v) {
	if (v->type == NUMBER) {
		auto number = static_cast<LSNumber*>(v);
		if (refs == 0) {
			mpz_mul_ui(&value, &value, number->value);
			LSValue::delete_temporary(number);
			return this;
		}
		auto r = LSMpz::get(vm);
		mpz_mul_ui(&r->value, &value, number->value);
		LSValue::delete_temporary(number);
		return r;
	}
	if (v->type == BOOLEAN) {
		auto boolean = static_cast<LSBoolean*>(v);
		if (boolean->value) {
			return this;
		}
		if (refs == 0) {
			mpz_set_ui(&value, 0);
			return this;
		}
		return LSMpz::get(vm, 0);
	}
	if (v->type != STRING) {
		LSValue::delete_temporary(this);
		LSValue::delete_temporary(v);
		throw vm::ExceptionObj(vm::Exception::NO_SUCH_OPERATOR);
	}
	auto string = static_cast<LSString*>(v);
	std::string r;
	// for (int i = 0; i < value; ++i) {
	// 	r += *string;
	// }
	if (refs == 0) delete this;
	if (string->refs == 0) {
		*string = r;
		return string;
	}
	return new LSString(r);
}

LSValue* LSMpz::mul_eq(LSValue* v) {
	if (v->type == NUMBER) {
		auto number = static_cast<LSNumber*>(v);
		// value *= number->value;
		LSValue::delete_temporary(number);
		return this;
	}
	if (v->type != BOOLEAN) {
		LSValue::delete_temporary(v);
		throw vm::ExceptionObj(vm::Exception::NO_SUCH_OPERATOR);
	}
	// auto boolean = static_cast<LSBoolean*>(v);
	// value *= boolean->value;
	return this;
}

LSValue* LSMpz::div(LSValue* v) {
	std::cout << "div " << this << " / " << v << std::endl;
	if (v->type == NUMBER) {
		auto number = static_cast<LSNumber*>(v);
		std::cout << "div refs " << refs << std::endl;
		if (refs == 0) {
			mpz_div_ui(&value, &value, number->value);
			if (number->refs == 0) delete number;
			std::cout << "return " << this << std::endl;
			return this;
		}
		if (number->refs == 0) {
			auto r = new LSMpz(vm);
			mpz_div_ui(&r->value, &value, number->value);
			delete number;
			return r;
		}
		// return LSMpz::get(value / number->value);
		return LSMpz::get(0);
	}
	if (v->type != BOOLEAN) {
		LSValue::delete_temporary(this);
		LSValue::delete_temporary(v);
		throw vm::ExceptionObj(vm::Exception::NO_SUCH_OPERATOR);
	}
	auto boolean = static_cast<LSBoolean*>(v);
	if (boolean->value) {
		return this;
	}
	if (refs == 0) {
		// value = NAN;
		return this;
	}
	return LSMpz::get(0);
}

LSValue* LSMpz::div_eq(LSValue* v) {
	if (v->type == NUMBER) {
		auto number = static_cast<LSNumber*>(v);
		// value /= number->value;
		LSValue::delete_temporary(number);
		return this;
	}
	if (v->type != BOOLEAN) {
		LSValue::delete_temporary(v);
		throw vm::ExceptionObj(vm::Exception::NO_SUCH_OPERATOR);
	}
	auto boolean = static_cast<LSBoolean*>(v);
	if (!boolean->value) {
		// value = NAN;
	}
	return this;
}

LSValue* LSMpz::int_div(LSValue* v) {
	if (v->type == NUMBER) {
		auto number = static_cast<LSNumber*>(v);
		if (refs == 0) {
			// value /= number->value;
			LSValue::delete_temporary(number);
			return this;
		}
		if (number->refs == 0) {
			// number->value = floor(value / number->value);
			return number;
		}
		// return LSMpz::get(floor(value / number->value));
		return LSMpz::get(0);
	}
	if (v->type != BOOLEAN) {
		LSValue::delete_temporary(this);
		LSValue::delete_temporary(v);
		throw vm::ExceptionObj(vm::Exception::NO_SUCH_OPERATOR);
	}
	auto boolean = static_cast<LSBoolean*>(v);
	if (!boolean->value) {
		LSValue::delete_temporary(this);
		LSValue::delete_temporary(v);
		throw vm::ExceptionObj(vm::Exception::DIVISION_BY_ZERO);
	}
	if (refs == 0) {
		return this;
	}
	return LSMpz::get_from_mpz(vm, value);
}

LSValue* LSMpz::int_div_eq(LSValue* v) {
	if (v->type == NUMBER) {
		auto number = static_cast<LSNumber*>(v);
		// value /= number->value;
		LSValue::delete_temporary(number);
		return this;
	}
	if (v->type != BOOLEAN) {
		LSValue::delete_temporary(this);
		LSValue::delete_temporary(v);
		throw vm::ExceptionObj(vm::Exception::NO_SUCH_OPERATOR);
	}
	auto boolean = static_cast<LSBoolean*>(v);
	if (!boolean->value) {
		LSValue::delete_temporary(v);
		throw vm::ExceptionObj(vm::Exception::DIVISION_BY_ZERO);
	}
	return this;
}

LSValue* LSMpz::pow(LSValue* v) {
	if (v->type == NUMBER) {
		auto number = static_cast<LSNumber*>(v);
		if (refs == 0) {
			mpz_pow_ui(&value, &value, number->value);
			if (number->refs == 0) delete number;
			return this;
		}
		if (number->refs == 0) {
			// number->value = std::pow(value, number->value);
			return number;
		}
		// return LSMpz::get(std::pow(value, number->value));
		return LSMpz::get(vm, 0);
	}
	if (v->type != BOOLEAN) {
		LSValue::delete_temporary(this);
		LSValue::delete_temporary(v);
		throw vm::ExceptionObj(vm::Exception::NO_SUCH_OPERATOR);
	}
	auto boolean = static_cast<LSBoolean*>(v);
	if (boolean->value) {
		return this;
	}
	if (refs == 0) {
		mpz_set_ui(&value, 1);
		return this;
	}
	return LSMpz::get(vm, 1);
}

LSValue* LSMpz::pow_eq(LSValue* v) {
	if (v->type == NUMBER) {
		auto number = static_cast<LSNumber*>(v);
		// value = std::pow(value, number->value);
		LSValue::delete_temporary(number);
		return this;
	}
	if (v->type != BOOLEAN) {
		LSValue::delete_temporary(v);
		throw vm::ExceptionObj(vm::Exception::NO_SUCH_OPERATOR);
	}
	auto boolean = static_cast<LSBoolean*>(v);
	if (!boolean->value) mpz_set_ui(&value, 1);
	return this;
}

LSValue* LSMpz::mod(LSValue* v) {
	if (v->type == NUMBER) {
		auto number = static_cast<LSNumber*>(v);
		if (refs == 0) {
			// value = fmod(value, number->value);
			LSValue::delete_temporary(number);
			return this;
		}
		if (number->refs == 0) {
			// number->value = fmod(value, number->value);
			return number;
		}
		// return LSMpz::get(fmod(value, number->value));
		return LSMpz::get(vm, 0);
	}
	if (v->type != BOOLEAN) {
		LSValue::delete_temporary(this);
		LSValue::delete_temporary(v);
		throw vm::ExceptionObj(vm::Exception::NO_SUCH_OPERATOR);
	}
	if (refs == 0) {
		mpz_set_ui(&value, 0);
		return this;
	}
	return LSMpz::get(vm, 0);
}

LSValue* LSMpz::mod_eq(LSValue* v) {
	if (v->type == NUMBER) {
		auto number = static_cast<LSNumber*>(v);
		// value = fmod(value, number->value);
		LSValue::delete_temporary(number);
		return this;
	}
	if (v->type != BOOLEAN) {
		LSValue::delete_temporary(v);
		throw vm::ExceptionObj(vm::Exception::NO_SUCH_OPERATOR);
	}
	mpz_set_ui(&value, 0);
	return this;
}

LSValue* LSMpz::double_mod(LSValue* v) {
	if (v->type == NUMBER) {
		auto number = static_cast<LSNumber*>(v);
		if (refs == 0) {
			// value = fmod(fmod(value, number->value) + number->value, number->value);
			// LSValue::delete_temporary(number);
			return this;
		}
		if (number->refs == 0) {
			// number->value = fmod(fmod(value, number->value) + number->value, number->value);
			return number;
		}
		// return LSMpz::get(fmod(fmod(value, number->value) + number->value, number->value));
		return LSMpz::get(vm, 0);
	}
	if (v->type != BOOLEAN) {
		LSValue::delete_temporary(this);
		LSValue::delete_temporary(v);
		throw vm::ExceptionObj(vm::Exception::NO_SUCH_OPERATOR);
	}
	if (refs == 0) {
		mpz_set_ui(&value, 0);
		return this;
	}
	return LSMpz::get(vm, 0);
}

LSValue* LSMpz::double_mod_eq(LSValue* v) {
	if (v->type == NUMBER) {
		// auto number = static_cast<LSNumber*>(v);
		// value = fmod(fmod(value, number->value) + number->value, number->value);
		// LSValue::delete_temporary(number);
		return this;
	}
	if (v->type != BOOLEAN) {
		LSValue::delete_temporary(v);
		throw vm::ExceptionObj(vm::Exception::NO_SUCH_OPERATOR);
	}
	mpz_set_ui(&value, 0);
	return this;
}

bool LSMpz::operator == (int v) const {
	return mpz_cmp_si(&value, v) == 0;
}

bool LSMpz::operator == (double v) const {
	return mpz_cmp_si(&value, v) == 0;
}

bool LSMpz::operator < (int v) const {
	return mpz_cmp_si(&value, v) < 0;
}

bool LSMpz::operator < (double v) const {
	return mpz_cmp_si(&value, v) < 0;
}

bool LSMpz::eq(const LSValue* v) const {
	if (v->type == NUMBER) {
		auto number = static_cast<const LSNumber*>(v);
		return mpz_cmp_ui(&this->value, number->value) == 0;
	}
	return false;
}

bool LSMpz::lt(const LSValue* v) const {
	if (v->type == NUMBER) {
		auto number = static_cast<const LSNumber*>(v);
		return mpz_cmp_ui(&this->value, number->value) < 0;
	}
	return LSValue::lt(v);
}

int LSMpz::abso() const {
	return std::abs((long) mpz_get_ui(&value));
}

LSValue* LSMpz::clone() const {
	return LSMpz::get_from_mpz(vm, value);
}

std::string LSMpz::json() const {
	std::ostringstream oss;
	oss << value;
	return oss.str();
}

std::ostream& LSMpz::dump(std::ostream& os, int) const {
	os << value;
	return os;
}

LSValue* LSMpz::getClass() const {
	return LSMpz::clazz;
}

}
