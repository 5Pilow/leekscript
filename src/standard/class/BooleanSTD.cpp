#include "BooleanSTD.hpp"
#include "../../type/Type.hpp"
#include "../../environment/Environment.hpp"
#if COMPILER
#include "../../vm/value/LSBoolean.hpp"
#include "../../vm/value/LSString.hpp"
#include "../../vm/value/LSNumber.hpp"
#endif

namespace ls {

BooleanSTD::BooleanSTD(Environment& env) : Module(env, "Boolean") {

	#if COMPILER
	env.boolean_class = std::make_unique<LSClass>(clazz.get());
	lsclass = env.boolean_class.get();
	#endif

	operator_("+", {
		{env.const_boolean, env.const_string, env.tmp_string, ADDR((void*) add)},
		{env.const_boolean, env.tmp_string, env.tmp_string, ADDR((void*) add_tmp)},
		{env.const_boolean, env.const_boolean, env.integer, ADDR(add_bool)},
		{env.const_boolean, env.const_real, env.real, ADDR(add_bool)},
		{env.const_boolean, env.const_integer, env.integer, ADDR(add_bool)}
	});

	operator_("-", {
		{env.const_boolean, env.const_boolean, env.integer, ADDR(sub_bool)},
		{env.const_boolean, env.const_real, env.real, ADDR(sub_bool)},
		{env.const_boolean, env.const_integer, env.integer, ADDR(sub_bool)}
	});

	operator_("*", {
		{env.const_boolean, env.const_boolean, env.integer, ADDR(mul_bool)},
		{env.const_boolean, env.const_real, env.real, ADDR(mul_bool)},
		{env.const_boolean, env.const_integer, env.integer, ADDR(mul_bool)}
	});

	method("compare", {
		{env.any, {env.const_any, env.const_any}, ADDR((void*) compare_ptr_ptr_ptr)},
		{env.integer, {env.const_boolean, env.const_any}, ADDR(compare_val_val)}
	});

	/** Internal **/
	method("to_string", {
		{env.tmp_string, {env.boolean}, ADDR((void*) to_string)}
	}, PRIVATE);
}

#if COMPILER

LSString* BooleanSTD::add(int boolean, LSString* string) {
	return new LSString((boolean ? "true" : "false") + *string);
}

LSString* BooleanSTD::add_tmp(int boolean, LSString* string) {
	(*string).insert(0, (boolean ? "true" : "false"));
	return string;
}

Compiler::value BooleanSTD::add_bool(Compiler& c, std::vector<Compiler::value> args, int) {
	return c.insn_add(args[0], args[1]);
}

Compiler::value BooleanSTD::sub_bool(Compiler& c, std::vector<Compiler::value> args, int) {
	return c.insn_sub(args[0], args[1]);
}

Compiler::value BooleanSTD::mul_bool(Compiler& c, std::vector<Compiler::value> args, int) {
	return c.insn_mul(args[0], args[1]);
}

int BooleanSTD::compare_ptr_ptr(LSBoolean* a, LSBoolean* b) {
	int res = 0;
	if (a->value) {
		if (not b->value) res = 1;
	} else {
		if (b->value) res = -1;
	}
	LSValue::delete_temporary(a);
	LSValue::delete_temporary(b);
	return res;
}

LSValue* BooleanSTD::compare_ptr_ptr_ptr(LSBoolean* a, LSBoolean* b) {
	return LSNumber::get(compare_ptr_ptr(a, b));
}

Compiler::value BooleanSTD::compare_val_val(Compiler& c, std::vector<Compiler::value> args, int) {
	return c.insn_cmpl(args[0], args[1]);
}

LSValue* BooleanSTD::to_string(bool b) {
	return new LSString(b ? "true" : "false");
}

#endif

}
