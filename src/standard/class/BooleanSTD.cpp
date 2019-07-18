#include "BooleanSTD.hpp"
#include "../../type/Type.hpp"
#if COMPILER
#include "../../vm/value/LSBoolean.hpp"
#include "../../vm/value/LSString.hpp"
#include "../../vm/value/LSNumber.hpp"
#endif

namespace ls {

BooleanSTD::BooleanSTD(VM* vm) : Module(vm, "Boolean") {

	#if COMPILER
	LSBoolean::clazz = lsclass.get();
	#endif

	operator_("+", {
		{Type::const_boolean, Type::const_string, Type::tmp_string, ADDR((void*) add)},
		{Type::const_boolean, Type::tmp_string, Type::tmp_string, ADDR((void*) add_tmp)},
		{Type::const_boolean, Type::const_boolean, Type::integer, ADDR(add_bool)},
		{Type::const_boolean, Type::const_real, Type::real, ADDR(add_bool)},
		{Type::const_boolean, Type::const_integer, Type::integer, ADDR(add_bool)}
	});

	operator_("-", {
		{Type::const_boolean, Type::const_boolean, Type::integer, ADDR(sub_bool)},
		{Type::const_boolean, Type::const_real, Type::real, ADDR(sub_bool)},
		{Type::const_boolean, Type::const_integer, Type::integer, ADDR(sub_bool)}
	});

	operator_("*", {
		{Type::const_boolean, Type::const_boolean, Type::integer, ADDR(mul_bool)},
		{Type::const_boolean, Type::const_real, Type::real, ADDR(mul_bool)},
		{Type::const_boolean, Type::const_integer, Type::integer, ADDR(mul_bool)}
	});

	method("compare", {
		{Type::any, {Type::const_any, Type::const_any}, ADDR((void*) compare_ptr_ptr_ptr)},
		{Type::integer, {Type::const_boolean, Type::const_any}, ADDR(compare_val_val)}
	});

	/** Internal **/
	method("to_string", {
		{Type::tmp_string, {Type::boolean}, ADDR((void*) to_string)}
	});
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
