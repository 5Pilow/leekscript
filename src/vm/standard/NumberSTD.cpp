#include "NumberSTD.hpp"
#include "../value/LSNumber.hpp"
#include "../../../lib/utf8.h"

using namespace std;

namespace ls {

NumberSTD::NumberSTD() : Module("Number") {

	static_field("pi", Type::FLOAT, (void*) &Number_pi);
	static_field("e", Type::FLOAT, (void*) &Number_e);
	static_field("phi", Type::FLOAT, (void*) &Number_phi);
	static_field("epsilon", Type::FLOAT, (void*) &Number_epsilon);

	method("abs", Type::NUMBER, Type::FLOAT_P, {}, (void*) &number_abs);
	method("acos", Type::NUMBER, Type::FLOAT_P, {}, (void*) &number_acos);
	method("asin", Type::NUMBER, Type::FLOAT_P, {}, (void*) &number_asin);
	method("atan", Type::NUMBER, Type::FLOAT_P, {}, (void*) &number_atan);
	method("cbrt", Type::NUMBER, Type::FLOAT_P, {}, (void*) &number_cbrt);
	method("ceil", Type::NUMBER, Type::FLOAT_P, {}, (void*) &number_ceil);
	method("cos", Type::NUMBER, Type::FLOAT_P, {}, (void*) &number_cos);
	method("exp", Type::NUMBER, Type::FLOAT_P, {}, (void*) &number_exp);
	method("floor", Type::NUMBER, Type::INTEGER_P, {}, (void*) &number_floor);
	method("hypot", Type::NUMBER, Type::FLOAT_P, {Type::NUMBER}, (void*) &number_hypot);
	method("log", Type::NUMBER, Type::FLOAT_P, {}, (void*) &number_log);
	method("log10", Type::NUMBER, Type::FLOAT_P, {}, (void*) &number_log10);
	method("max", {
		{Type::NUMBER, Type::NUMBER, {Type::NUMBER}, (void*) &number_max},
		{Type::FLOAT, Type::FLOAT, {Type::FLOAT}, nullptr},
		{Type::INTEGER, Type::INTEGER, {Type::INTEGER}, nullptr},
	});
	method("min", {
		{Type::NUMBER, Type::NUMBER, {Type::NUMBER}, (void*) &number_min},
		{Type::FLOAT, Type::FLOAT, {Type::FLOAT}, nullptr},
		{Type::INTEGER, Type::INTEGER, {Type::INTEGER}, nullptr},
	});
	method("pow", Type::NUMBER, Type::FLOAT_P, {Type::NUMBER}, (void*) &number_pow);
	method("round", Type::NUMBER, Type::INTEGER_P, {}, (void*) &number_round);
	method("signum", Type::NUMBER, Type::INTEGER_P, {}, (void*) &number_signum);
	method("sin", Type::NUMBER, Type::FLOAT_P, {}, (void*) &number_sin);
	method("sqrt", Type::NUMBER, Type::FLOAT_P, {}, (void*) &number_sqrt);
	method("tan", Type::NUMBER, Type::FLOAT_P, {}, (void*) &number_tan);
	method("toDegrees", Type::NUMBER, Type::FLOAT_P, {}, (void*) &number_toDegrees);
	method("toRadians", Type::NUMBER, Type::FLOAT_P, {}, (void*) &number_toRadians);
	method("isInteger", Type::NUMBER, Type::BOOLEAN, {}, (void*) &number_isInteger);
	method("char", Type::NUMBER, Type::STRING, {}, (void*) &number_char);

	static_method("abs", Type::FLOAT_P, {Type::NUMBER}, (void*) &number_abs);
	static_method("acos", Type::FLOAT_P, {Type::NUMBER}, (void*) &number_acos);
	static_method("asin", Type::FLOAT_P, {Type::NUMBER}, (void*) &number_asin);
	static_method("atan", Type::FLOAT_P, {Type::NUMBER}, (void*) &number_atan);
	static_method("atan2", Type::FLOAT_P, {Type::NUMBER, Type::NUMBER}, (void*) &number_atan2);
	static_method("cbrt", Type::FLOAT_P, {Type::NUMBER}, (void*) &number_cbrt);
	static_method("ceil", Type::FLOAT_P, {Type::NUMBER}, (void*) &number_ceil);
	static_method("cos", Type::FLOAT_P, {Type::NUMBER}, (void*) &number_cos);
	static_method("exp", Type::FLOAT_P, {Type::NUMBER}, (void*) &number_exp);
	static_method("floor", Type::INTEGER_P, {Type::NUMBER}, (void*) &number_floor);
	static_method("hypot", Type::FLOAT_P, {Type::NUMBER, Type::NUMBER}, (void*) &number_hypot);
	static_method("log", Type::FLOAT_P, {Type::NUMBER}, (void*) &number_log);
	static_method("log10", Type::FLOAT_P, {Type::NUMBER}, (void*) &number_log10);
	static_method("max", Type::FLOAT_P, {Type::NUMBER, Type::NUMBER}, (void*) &number_max);
	static_method("min", Type::FLOAT_P, {Type::NUMBER, Type::NUMBER}, (void*) &number_min);
	static_method("pow", Type::FLOAT_P, {Type::NUMBER, Type::NUMBER}, (void*) &number_pow);
	static_method("rand", Type::FLOAT, {}, (void*) &number_rand);
	static_method("randFloat", Type::FLOAT_P, {Type::NUMBER, Type::NUMBER}, (void*) &number_randFloat);
	static_method("randInt", Type::INTEGER, {Type::INTEGER, Type::INTEGER}, (void*) &number_randInt);
	static_method("round", Type::INTEGER_P, {Type::NUMBER}, (void*) &number_round);
	static_method("signum", Type::INTEGER_P, {Type::NUMBER}, (void*) &number_signum);
	static_method("sin", Type::FLOAT_P, {Type::NUMBER}, (void*) &number_sin);
	static_method("sqrt", Type::FLOAT_P, {Type::NUMBER}, (void*) &number_sqrt);
	static_method("tan", Type::FLOAT_P, {Type::NUMBER}, (void*) &number_tan);
	static_method("toDegrees", Type::FLOAT_P, {Type::NUMBER}, (void*) &number_toDegrees);
	static_method("toRadians", Type::FLOAT_P, {Type::NUMBER}, (void*) &number_toRadians);
	static_method("isInteger", Type::BOOLEAN, {Type::NUMBER}, (void*) &number_isInteger);
	static_method("char", Type::STRING, {Type::NUMBER}, (void*) &number_char);
}

jit_value_t Number_e(jit_function_t F) {
	return JIT_CREATE_CONST_FLOAT(F, JIT_FLOAT, M_E);
}
jit_value_t Number_phi(jit_function_t F) {
	return JIT_CREATE_CONST_FLOAT(F, JIT_FLOAT, 1.61803398874989484820);
}
jit_value_t Number_pi(jit_function_t F) {
	return JIT_CREATE_CONST_FLOAT(F, JIT_FLOAT, 3.14159265358979323846);
}
jit_value_t Number_epsilon(jit_function_t F) {
	return JIT_CREATE_CONST_FLOAT(F, JIT_FLOAT, std::numeric_limits<double>::epsilon());
}

LSNumber* number_abs(const LSNumber* number) {
	return LSNumber::get(fabs(number->value));
}

LSNumber* number_acos(const LSNumber* number) {
	return LSNumber::get(acos(number->value));
}

LSNumber* number_asin(const LSNumber* number) {
	return LSNumber::get(asin(number->value));
}

LSNumber* number_atan(const LSNumber* number) {
	return LSNumber::get(atan(number->value));
}

LSNumber* number_atan2(const LSNumber* x, const LSNumber* y) {
	return LSNumber::get(atan2(x->value, y->value));
}

LSNumber* number_cbrt(const LSNumber* number) {
	return LSNumber::get(cbrt(number->value));
}

LSNumber* number_ceil(const LSNumber* number) {
	return LSNumber::get(ceil(number->value));
}

LSNumber* number_cos(const LSNumber* x) {
	return LSNumber::get(cos(x->value));
}

LSNumber* number_exp(const LSNumber* x) {
	return LSNumber::get(exp(x->value));
}

LSNumber* number_floor(const LSNumber* number) {
	return LSNumber::get(floor(number->value));
}

LSNumber* number_hypot(const LSNumber* x, const LSNumber* y) {
	return LSNumber::get(hypot(x->value, y->value));
}

LSNumber* number_log(const LSNumber* x) {
	return LSNumber::get(log(x->value));
}

LSNumber* number_log10(const LSNumber* x) {
	return LSNumber::get(log10(x->value));
}

LSNumber* number_max(const LSNumber* x, const LSNumber* y) {
	return LSNumber::get(max(x->value, y->value));
}

LSNumber* number_min(const LSNumber* x, const LSNumber* y) {
	return new LSNumber(min(x->value, y->value));
}

LSNumber* number_pow(const LSNumber* x, const LSNumber* y) {
	return LSNumber::get(pow(x->value, y->value));
}

double number_rand() {
	return (double) rand() / RAND_MAX;
}

LSNumber* number_randFloat(const LSNumber* min, const LSNumber* max) {
	return LSNumber::get(min->value + ((double) rand() / RAND_MAX) * max->value);
}

int number_randInt(int min, int max) {
	return floor(min + ((double) rand() / RAND_MAX) * (max - min));
}

LSNumber* number_round(const LSNumber* number) {
	return LSNumber::get(round(number->value));
}

LSNumber* number_signum(const LSNumber* x) {
	if (x->value > 0) return LSNumber::get(1);
	if (x->value < 0) return LSNumber::get(-1);
	return LSNumber::get(0);
}

LSNumber* number_sin(const LSNumber* x) {
	return new LSNumber(sin(x->value));
}

LSNumber* number_sqrt(const LSNumber* x) {
	return new LSNumber(sqrt(x->value));
}

LSNumber* number_tan(const LSNumber* x) {
	return new LSNumber(tan(x->value));
}

LSNumber* number_toDegrees(const LSNumber* x) {
	return LSNumber::get((x->value * 180) / M_PI);
}

LSNumber* number_toRadians(const LSNumber* x) {
	return LSNumber::get((x->value * M_PI) / 180);
}

bool number_isInteger(const LSNumber* x) {
	double _;
	return modf(x->value, &_) == 0;
}

LSString* number_char(const LSNumber* number) {
	unsigned int n = number->value;
	char dest[5];
	u8_toutf8(dest, 5, &n, 1);
	return new LSString(dest);
}

}
