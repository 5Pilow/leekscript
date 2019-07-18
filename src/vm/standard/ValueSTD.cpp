#include "ValueSTD.hpp"
#include "JsonSTD.hpp"
#include "../value/LSBoolean.hpp"
#include "../value/LSString.hpp"
#include "../value/LSNumber.hpp"
#include "../LSValue.hpp"
#include "../VM.hpp"
#include "../Context.hpp"
#include "../../type/Type.hpp"

namespace ls {

ValueSTD::ValueSTD(VM* vm) : Module(vm, "Value") {

	LSValue::ValueClass = lsclass.get();

	/*
	 * Static attributes
	 */
	static_field("unknown", Type::any, unknown);

	/*
	 * Attributes
	 */
	field("class", Type::clazz(), attr_class);

	/*
	 * Operators
	 */
	auto sV = Type::template_("V");
	auto sT = Type::template_("T");
	template_(sV, sT).
	operator_("=", {
		{sV, sT, sT, op_store, 0, { new ChangeValueMutator() }, true}
	});

	operator_("is", {
		{Type::const_any, Type::const_class(), Type::boolean, op_instanceof}
	});
	operator_("==", {
		{Type::const_any, Type::const_any, Type::boolean, (void*) eq},
		{Type::const_any, Type::const_any, Type::boolean, op_equals}
	});
	operator_("===", {
		{Type::const_any, Type::const_any, Type::boolean, (void*) triple_eq, LEGACY},
		{Type::const_any, Type::const_any, Type::boolean, op_triple_equals, LEGACY}
	});
	operator_("!=", {
		{Type::const_any, Type::const_any, Type::boolean, op_not_equals}
	});
	operator_("<", {
		{Type::const_any, Type::const_any, Type::boolean, (void*) lt},
		{Type::const_any, Type::const_any, Type::boolean, op_lt},
	});
	operator_("<=", {
		{Type::const_any, Type::const_any, Type::boolean, (void*) le},
		{Type::const_any, Type::const_any, Type::boolean, op_le}
	});
	operator_(">", {
		{Type::const_any, Type::const_any, Type::boolean, (void*) gt},
		{Type::const_any, Type::const_any, Type::boolean, op_gt}
	});
	operator_(">=", {
		{Type::const_any, Type::const_any, Type::boolean, (void*) ge},
		{Type::const_any, Type::const_any, Type::boolean, op_ge}
	});
	operator_("and", {
		{Type::const_any, Type::const_any, Type::boolean, op_and}
	});
	operator_("&&", {
		{Type::const_any, Type::const_any, Type::boolean, op_and}
	});
	operator_("or", {
		{Type::const_any, Type::const_any, Type::boolean, op_or}
	});
	operator_("||", {
		{Type::const_any, Type::const_any, Type::boolean, op_or}
	});
	operator_("xor", {
		{Type::const_any, Type::const_any, Type::boolean, op_xor}
	});
	operator_("+", {
		{Type::const_any, Type::const_any, Type::any, (void*) ls_add, THROWS},
		{Type::const_any, Type::const_any, Type::any, op_add, THROWS},
	});
	operator_("+=", {
		{Type::const_any, Type::const_any, Type::any, (void*) ls_add_eq, THROWS, { new ConvertMutator() }, true}
	});
	operator_("-", {
		{Type::const_any, Type::const_any, Type::any, (void*) ls_sub, THROWS},
		{Type::const_any, Type::const_any, Type::any, op_sub, THROWS},
	});
	operator_("-=", {
		{Type::const_any, Type::const_any, Type::any, (void*) ls_sub_eq, THROWS, {}, true}
	});
	operator_("*", {
		{Type::const_any, Type::const_any, Type::any, (void*) ls_mul, THROWS},
		{Type::const_any, Type::const_any, Type::any, op_mul, THROWS}
	});
	operator_("*=", {
		{Type::const_any, Type::const_any, Type::any, (void*) ls_mul_eq, THROWS, {}, true}
	});
	operator_("**", {
		{Type::const_any, Type::const_any, Type::any, (void*) ls_pow, THROWS},
	});
	operator_("**=", {
		{Type::const_any, Type::const_any, Type::any, (void*) ls_pow_eq, THROWS, {}, true}
	});
	operator_("/", {
		{Type::const_any, Type::const_any, Type::any, (void*) ls_div, THROWS},
	});
	operator_("/=", {
		{Type::const_any, Type::const_any, Type::any, (void*) ls_div_eq, THROWS, {}, true}
	});
	operator_("\\", {
		{Type::const_any, Type::const_any, Type::any, (void*) ls_int_div, THROWS}
	});
	operator_("\\=", {
		{Type::const_any, Type::const_any, Type::any, (void*) ls_int_div_eq, THROWS, {}, true}
	});
	operator_("%", {
		{Type::const_any, Type::const_any, Type::any, (void*) ls_mod, THROWS}
	});
	operator_("%=", {
		{Type::const_any, Type::const_any, Type::any, (void*) ls_mod_eq, THROWS, {}, true}
	});
	operator_("%%", {
		{Type::const_any, Type::const_any, Type::any, (void*) ls_double_mod},
	});
	operator_("%%=", {
		{Type::const_any, Type::const_any, Type::any, (void*) ls_double_mod_eq, THROWS, {}, true}
	});
	operator_("&", {
		{Type::const_any, Type::const_any, Type::integer, (void*) ls_bit_and, THROWS}
	});
	operator_("&=", {
		{Type::any, Type::const_any, Type::integer, (void*) ls_bit_and_eq, THROWS, {}, true}
	});
	operator_("|", {
		{Type::const_any, Type::const_any, Type::integer, (void*) ls_bit_or, THROWS}
	});
	operator_("|=", {
		{Type::any, Type::const_any, Type::integer, (void*) ls_bit_or_eq, THROWS, {}, true}
	});
	operator_("^", {
		{Type::const_any, Type::const_any, Type::integer, (void*) ls_bit_xor, THROWS}
	});
	operator_("^=", {
		{Type::any, Type::const_any, Type::integer, (void*) ls_bit_xor_eq, THROWS, {}, true}
	});
	operator_("<<", {
		{Type::const_any, Type::const_any, Type::integer, bit_shift_left, THROWS}
	});
	operator_("<<=", {
		{Type::const_any, Type::const_any, Type::integer, (void*) ls_bit_shift_left_eq, 0, {}, true},
		{Type::integer, Type::const_integer, Type::integer, bit_shift_left_eq, 0, {}, true},
	});
	operator_(">>", {
		{Type::const_integer, Type::const_integer, Type::integer, bit_shift_right, THROWS}
	});
	operator_(">>=", {
		{Type::const_any, Type::const_any, Type::integer, (void*) ls_bit_shift_right_eq, 0, {}, true},
		{Type::const_integer, Type::const_integer, Type::integer, bit_shift_right_eq, 0, {}, true},
	});
	operator_(">>>", {
		{Type::const_integer, Type::const_integer, Type::integer, bit_shift_uright, THROWS}
	});
	operator_(">>>=", {
		{Type::const_any, Type::const_any, Type::integer, (void*) ls_bit_shift_uright_eq, 0, {}, true},
		{Type::const_integer, Type::const_integer, Type::integer, bit_shift_uright_eq, 0, {}, true}
	});
	operator_("in", {
		{Type::const_any, Type::const_any, Type::boolean, (void*) in},
		{Type::const_any, Type::const_any, Type::boolean, op_in},
	});
	operator_("<=>", {
		{Type::any, Type::any, Type::any, (void*) &op_swap_ptr, 0, {}, true, true},
		{Type::integer, Type::integer, Type::integer, op_swap_val, 0, {}, true, true},
	});
	auto T = Type::template_("T");
	auto R = Type::template_("R");
	template_(T, R).
	operator_("~", {
		{T, Type::fun(R, {T}), R, op_call},
	});

	/*
	 * Methods
	 */
	method("copy", {
		{Type::any, {Type::const_any}, copy}
	});
	method("string", {
		{Type::string, {Type::const_any}, to_string}
	});
	method("json", {
		{Type::tmp_string, {Type::const_any}, (void*) &LSValue::ls_json},
		{Type::tmp_string, {Type::const_any}, JsonSTD::encode},
	});
	method("typeID", {
		{Type::integer, {Type::const_any}, typeID}
	});
	method("move", {
		{Type::any, {Type::const_any}, (void*) &LSValue::move}
	});
	method("move_inc", {
		{Type::any, {Type::const_any}, (void*) &LSValue::move_inc}
	});
	method("ptr", {
		{Type::any, {Type::const_any}, (void*) &LSValue::move}
	});
	method("absolute", {
		{Type::integer, {Type::const_any}, (void*) absolute, THROWS}
	});
	method("clone", {
		{Type::any, {Type::const_any}, (void*) clone}
	});
	method("delete", {
		{Type::void_, {Type::const_any}, (void*) &LSValue::free}
	});
	method("delete_tmp", {
		{Type::void_, {Type::const_any}, (void*) &LSValue::delete_temporary}
	});
	method("dec_refs", {
		{Type::void_, {Type::const_any}, (void*) &LSValue::delete_ref}
	});
	method("delete_ref", {
		{Type::void_, {Type::const_any}, (void*) &LSValue::delete_ref2}
	});
	method("not", {
		{Type::boolean, {Type::const_any}, (void*) ls_not}
	});
	method("minus", {
		{Type::any, {Type::const_any}, (void*) ls_minus}
	});
	method("dec", {
		{Type::any, {Type::const_any}, (void*) ls_dec}
	});
	method("pre_dec", {
		{Type::any, {Type::const_any}, (void*) ls_pre_dec}
	});
	method("decl", {
		{Type::any, {Type::const_any}, (void*) ls_decl}
	});
	method("pre_decl", {
		{Type::any, {Type::const_any}, (void*) ls_pre_decl}
	});
	method("inc", {
		{Type::any, {Type::const_any}, (void*) ls_inc}
	});
	method("pre_inc", {
		{Type::any, {Type::const_any}, (void*) ls_pre_inc}
	});
	method("incl", {
		{Type::any, {Type::const_any}, (void*) ls_incl}
	});
	method("pre_incl", {
		{Type::any, {Type::const_any}, (void*) ls_pre_incl}
	});
	method("pre_tilde", {
		{Type::any, {Type::const_any}, (void*) ls_pre_tilde}
	});
	method("attr", {
		{Type::any, {Type::any, Type::i8_ptr}, (void*) attr},
	});
	method("attrL", {
		{Type::any, {Type::any, Type::i8_ptr}, (void*) attrL},
	});
	method("int", {
		{Type::integer, {Type::const_any}, (void*) integer}
	});
	method("real", {
		{Type::real, {Type::const_any}, (void*) real}
	});
	method("real_delete", {
		{Type::real, {Type::const_any}, (void*) real_delete}
	});
	method("long", {
		{Type::long_, {Type::const_any}, (void*) long_}
	});
	method("range", {
		{Type::any, {Type::const_any, Type::integer, Type::integer}, (void*) range}
	});
	method("at", {
		{Type::any, {Type::const_any, Type::const_any}, (void*) at}
	});
	method("atl", {
		{Type::any, {Type::const_any, Type::const_any}, (void*) atl}
	});
	method("in_i", {
		{Type::boolean, {Type::const_any, Type::integer}, (void*) in_i}
	});
	method("in", {
		{Type::boolean, {Type::const_any, Type::const_any}, (void*) in}
	});
	method("is_null", {
		{Type::boolean, {Type::const_any}, (void*) is_null}
	});
	method("to_bool", {
		{Type::boolean, {Type::const_any}, (void*) to_bool}
	});
	method("type", {
		{Type::integer, {Type::const_any}, (void*) type}
	});
	method("delete_previous", {
		{Type::void_, {Type::any}, (void*) delete_previous}
	});
	method("get_int", {
		{Type::integer, {Type::any}, (void*) get_int}
	});
	method("get_class", {
		{Type::clazz(), {Type::any}, (void*) get_class}
	});
	method("export_ctx_var", {
		{Type::void_, {Type::i8_ptr, Type::any}, (void*) export_context_variable},
		{Type::void_, {Type::i8_ptr, Type::integer}, (void*) export_context_variable_int},
		{Type::void_, {Type::i8_ptr, Type::long_}, (void*) export_context_variable_long},
		{Type::void_, {Type::i8_ptr, Type::real}, (void*) export_context_variable_real},
	});
}

/*
 * Static attributes
 */
Compiler::value ValueSTD::unknown(Compiler& c) {
	return c.insn_call(Type::any, {c.new_integer(floor(1 + ((double) rand() / RAND_MAX) * 100))}, "Number.new");
}

/*
 * Attributes
 */
Compiler::value ValueSTD::attr_class(Compiler& c, Compiler::value a) {
	auto clazz = c.insn_class_of(a);
	c.insn_delete_temporary(a);
	return clazz;
}

/*
 * Operators
 */
Compiler::value ValueSTD::op_store(Compiler& c, std::vector<Compiler::value> args, int flags) {
	// std::cout << "op_store " << args << " " << flags << std::endl;
	auto x = args[0];
	auto y = args[1];
	auto ny = c.insn_convert(y, x.t->pointed());
	// Move the object
	ny = c.insn_move_inc(ny);
	// Delete previous value
	if (not (flags & EMPTY_VARIABLE)) {
		c.insn_delete_variable(x);
	}
	if (ny.t->is_mpz_ptr()) {
		c.insn_store(x, c.insn_load(ny));
	} else {
		c.insn_store(x, ny);
	}
	if (flags & NO_RETURN) {
		return {};
	} else {
		if (y.t->is_mpz_ptr()) {
			y.t = y.t->not_temporary();
			return c.insn_clone_mpz(y);
		} else {
			return c.clone(y);
		}
	}
}

Compiler::value ValueSTD::op_instanceof(Compiler& c, std::vector<Compiler::value> args, bool) {
	auto r = c.insn_eq(c.insn_class_of(args[0]), args[1]);
	c.insn_delete_temporary(args[0]);
	c.insn_delete_temporary(args[1]);
	return r;
}

Compiler::value ValueSTD::op_equals(Compiler& c, std::vector<Compiler::value> args, bool) {
	return c.insn_eq(args[0], args[1]);
}
Compiler::value ValueSTD::op_triple_equals(Compiler& c, std::vector<Compiler::value> args, bool) {
	if (args[0].t->id() > 0 and args[1].t->id() > 0 and args[0].t->id() != args[1].t->id()) {
		c.insn_delete_temporary(args[0]);
		c.insn_delete_temporary(args[1]);
		return c.new_bool(false);
	}
	auto a = c.insn_eq(c.insn_typeof(args[0]), c.insn_typeof(args[1]));
	auto b = c.insn_eq(args[0], args[1]);
	return c.insn_and(a, b);
}

Compiler::value ValueSTD::op_not_equals(Compiler& c, std::vector<Compiler::value> args, bool) {
	return c.insn_ne(args[0], args[1]);
}

Compiler::value ValueSTD::op_lt(Compiler& c, std::vector<Compiler::value> args, bool) {
	if (args[0].t->id() == args[1].t->id() or args[0].t->id() == 0 or args[1].t->id() == 0) {
		auto ap = c.insn_to_any(args[0]);
		auto bp = c.insn_to_any(args[1]);
		auto res = c.insn_call(Type::boolean, {ap, bp}, "Value.operator<");
		c.insn_delete_temporary(ap);
		c.insn_delete_temporary(bp);
		return res;
	} else {
		auto res = c.insn_lt(c.insn_typeof(args[0]), c.insn_typeof(args[1]));
		c.insn_delete_temporary(args[0]);
		c.insn_delete_temporary(args[1]);
		return res;
	}
}

Compiler::value ValueSTD::op_le(Compiler& c, std::vector<Compiler::value> args, bool) {
	if (args[0].t->id() == args[1].t->id() or args[0].t->id() == 0 or args[1].t->id() == 0) {
		auto ap = c.insn_to_any(args[0]);
		auto bp = c.insn_to_any(args[1]);
		auto res = c.insn_call(Type::boolean, {ap, bp}, "Value.operator<");
		c.insn_delete_temporary(ap);
		c.insn_delete_temporary(bp);
		return res;
	} else {
		auto res = c.insn_le(c.insn_typeof(args[0]), c.insn_typeof(args[1]));
		c.insn_delete_temporary(args[0]);
		c.insn_delete_temporary(args[1]);
		return res;
	}
}

Compiler::value ValueSTD::op_gt(Compiler& c, std::vector<Compiler::value> args, bool) {
	if (args[0].t->id() == args[1].t->id() or args[0].t->id() == 0	or args[1].t->id() == 0) {
		auto ap = c.insn_to_any(args[0]);
		auto bp = c.insn_to_any(args[1]);
		auto res = c.insn_call(Type::boolean, {ap, bp}, "Value.operator>");
		c.insn_delete_temporary(ap);
		c.insn_delete_temporary(bp);
		return res;
	} else {
		auto res = c.insn_gt(c.insn_typeof(args[0]), c.insn_typeof(args[1]));
		c.insn_delete_temporary(args[0]);
		c.insn_delete_temporary(args[1]);
		return res;
	}
}

Compiler::value ValueSTD::op_ge(Compiler& c, std::vector<Compiler::value> args, bool) {
	if (args[0].t->id() == args[1].t->id() or args[0].t->id() == 0 or args[1].t->id() == 0) {
		auto res = c.insn_call(Type::boolean, {c.insn_to_any(args[0]), c.insn_to_any(args[1])}, "Value.operator>=");
		c.insn_delete_temporary(args[0]);
		c.insn_delete_temporary(args[1]);
		return res;
	} else {
		auto res = c.insn_ge(c.insn_typeof(args[0]), c.insn_typeof(args[1]));
		c.insn_delete_temporary(args[0]);
		c.insn_delete_temporary(args[1]);
		return res;
	}
}

Compiler::value ValueSTD::op_and(Compiler& c, std::vector<Compiler::value> args, bool) {
	auto res = c.insn_and(c.insn_to_bool(args[0]), c.insn_to_bool(args[1]));
	c.insn_delete_temporary(args[0]);
	c.insn_delete_temporary(args[1]);
	return res;
}

Compiler::value ValueSTD::op_or(Compiler& c, std::vector<Compiler::value> args, bool) {
	auto res = c.insn_or(c.insn_to_bool(args[0]), c.insn_to_bool(args[1]));
	c.insn_delete_temporary(args[0]);
	c.insn_delete_temporary(args[1]);
	return res;
}

Compiler::value ValueSTD::op_xor(Compiler& c, std::vector<Compiler::value> args, bool) {
	auto a = c.insn_to_bool(args[0]);
	auto b = c.insn_to_bool(args[1]);
	auto r = c.insn_or(
		c.insn_and(a, c.insn_not_bool(b)),
		c.insn_and(b, c.insn_not_bool(a))
	);
	c.insn_delete_temporary(args[0]);
	c.insn_delete_temporary(args[1]);
	return r;
}

Compiler::value ValueSTD::op_add(Compiler& c, std::vector<Compiler::value> args, bool) {
	return c.insn_add(args[0], args[1]);
}
Compiler::value ValueSTD::op_sub(Compiler& c, std::vector<Compiler::value> args, bool) {
	return c.insn_sub(args[0], args[1]);
}
Compiler::value ValueSTD::op_mul(Compiler& c, std::vector<Compiler::value> args, bool) {
	return c.insn_mul(args[0], args[1]);
}
Compiler::value ValueSTD::op_div(Compiler& c, std::vector<Compiler::value> args, bool) {
	return c.insn_div(args[0], args[1]);
}

int ValueSTD::ls_bit_and(LSValue* x, LSValue* y) {
	LSNumber *a, *b;
	if ((a = dynamic_cast<LSNumber*>(x)) == nullptr or (b = dynamic_cast<LSNumber*>(y)) == nullptr) {
		LSValue::delete_temporary(x);
		LSValue::delete_temporary(y);
		throw vm::ExceptionObj(vm::Exception::NO_SUCH_OPERATOR);
	}
	auto res = (int) a->value & (int) b->value;
	LSValue::delete_temporary(x);
	LSValue::delete_temporary(y);
	return res;
}

int ValueSTD::ls_bit_and_eq(LSValue** x, LSValue* y) {
	LSNumber *a, *b;
	if ((a = dynamic_cast<LSNumber*>(*x)) == nullptr or (b = dynamic_cast<LSNumber*>(y)) == nullptr) {
		LSValue::delete_temporary(y);
		throw vm::ExceptionObj(vm::Exception::NO_SUCH_OPERATOR);
	}
	auto res = (int) a->value & (int) b->value;
	LSValue::delete_temporary(y);
	((LSNumber*) *x)->value = res;
	return res;
}

int ValueSTD::ls_bit_or(LSValue* x, LSValue* y) {
	LSNumber *a, *b;
	if ((a = dynamic_cast<LSNumber*>(x)) == nullptr or
		(b = dynamic_cast<LSNumber*>(y)) == nullptr) {
		LSValue::delete_temporary(x);
		LSValue::delete_temporary(y);
		throw vm::ExceptionObj(vm::Exception::NO_SUCH_OPERATOR);
	}
	auto res = (int) a->value | (int) b->value;
	LSValue::delete_temporary(x);
	LSValue::delete_temporary(y);
	return res;
}

int ValueSTD::ls_bit_or_eq(LSValue** x, LSValue* y) {
	LSNumber *a, *b;
	if ((a = dynamic_cast<LSNumber*>(*x)) == nullptr or (b = dynamic_cast<LSNumber*>(y)) == nullptr) {
		LSValue::delete_temporary(y);
		throw vm::ExceptionObj(vm::Exception::NO_SUCH_OPERATOR);
	}
	auto res = (int) a->value | (int) b->value;
	LSValue::delete_temporary(y);
	((LSNumber*) *x)->value = res;
	return res;
}

int ValueSTD::ls_bit_xor(LSValue* x, LSValue* y) {
	LSNumber *a, *b;
	if ((a = dynamic_cast<LSNumber*>(x)) == nullptr or
		(b = dynamic_cast<LSNumber*>(y)) == nullptr) {
		LSValue::delete_temporary(x);
		LSValue::delete_temporary(y);
		throw vm::ExceptionObj(vm::Exception::NO_SUCH_OPERATOR);
	}
	auto res = (int) a->value ^ (int) b->value;
	LSValue::delete_temporary(x);
	LSValue::delete_temporary(y);
	return res;
}

int ValueSTD::ls_bit_xor_eq(LSValue** x, LSValue* y) {
	LSNumber *a, *b;
	if ((a = dynamic_cast<LSNumber*>(*x)) == nullptr or (b = dynamic_cast<LSNumber*>(y)) == nullptr) {
		LSValue::delete_temporary(y);
		throw vm::ExceptionObj(vm::Exception::NO_SUCH_OPERATOR);
	}
	auto res = (int) a->value ^ (int) b->value;
	LSValue::delete_temporary(y);
	((LSNumber*) *x)->value = res;
	return res;
}

Compiler::value ValueSTD::bit_shift_left(Compiler& c, std::vector<Compiler::value> args, bool) {
	auto r = c.insn_shl(c.to_int(args[0]), c.to_int(args[1]));
	c.insn_delete_temporary(args[0]);
	c.insn_delete_temporary(args[1]);
	return r;
}
Compiler::value ValueSTD::bit_shift_left_eq(Compiler& c, std::vector<Compiler::value> args, bool) {
	if (args[0].t->pointed()->is_primitive() && args[1].t->is_primitive()) {
		auto res = c.insn_shl(c.insn_load(args[0]), args[1]);
		c.insn_store(args[0], res);
		return res;
	} else {
		return c.insn_invoke(Type::integer, {args[0], c.insn_to_any(args[1])}, "Value.operator<<=");
	}
}
int ValueSTD::ls_bit_shift_left_eq(LSValue** x, LSValue* y) {
	LSNumber *a, *b;
	if ((a = dynamic_cast<LSNumber*>(*x)) == nullptr or (b = dynamic_cast<LSNumber*>(y)) == nullptr) {
		LSValue::delete_temporary(y);
		throw vm::ExceptionObj(vm::Exception::NO_SUCH_OPERATOR);
	}
	auto res = (int) a->value << (int) b->value;
	LSValue::delete_temporary(y);
	((LSNumber*) *x)->value = res;
	return res;
}
Compiler::value ValueSTD::bit_shift_right(Compiler& c, std::vector<Compiler::value> args, bool) {
	auto r = c.insn_ashr(c.to_int(args[0]), c.to_int(args[1]));
	c.insn_delete_temporary(args[0]);
	c.insn_delete_temporary(args[1]);
	return r;
}
Compiler::value ValueSTD::bit_shift_right_eq(Compiler& c, std::vector<Compiler::value> args, bool) {
	if (args[0].t->pointed()->is_primitive() && args[1].t->is_primitive()) {
		auto res = c.insn_ashr(c.insn_load(args[0]), args[1]);
		c.insn_store(args[0], res);
		return res;
	} else {
		return c.insn_invoke(Type::integer, {args[0], c.insn_to_any(args[1])}, "Value.operator>>=");
	}
}
int ValueSTD::ls_bit_shift_right_eq(LSValue** x, LSValue* y) {
	LSNumber *a, *b;
	if ((a = dynamic_cast<LSNumber*>(*x)) == nullptr or (b = dynamic_cast<LSNumber*>(y)) == nullptr) {
		LSValue::delete_temporary(y);
		throw vm::ExceptionObj(vm::Exception::NO_SUCH_OPERATOR);
	}
	auto res = (int) a->value >> (int) b->value;
	LSValue::delete_temporary(y);
	((LSNumber*) *x)->value = res;
	return res;
}

Compiler::value ValueSTD::bit_shift_uright(Compiler& c, std::vector<Compiler::value> args, bool) {
	auto r = c.insn_lshr(c.to_int(args[0]), c.to_int(args[1]));
	c.insn_delete_temporary(args[0]);
	c.insn_delete_temporary(args[1]);
	return r;
}
Compiler::value ValueSTD::bit_shift_uright_eq(Compiler& c, std::vector<Compiler::value> args, bool) {
	if (args[0].t->pointed()->is_primitive() && args[1].t->is_primitive()) {
		auto res = c.insn_lshr(c.insn_load(args[0]), args[1]);
		c.insn_store(args[0], res);
		return res;
	} else {
		return c.insn_invoke(Type::integer, {args[0], c.insn_to_any(args[1])}, "Value.operator>>>=");
	}
}
int ValueSTD::ls_bit_shift_uright_eq(LSValue** x, LSValue* y) {
	LSNumber *a, *b;
	if ((a = dynamic_cast<LSNumber*>(*x)) == nullptr or (b = dynamic_cast<LSNumber*>(y)) == nullptr) {
		LSValue::delete_temporary(y);
		throw vm::ExceptionObj(vm::Exception::NO_SUCH_OPERATOR);
	}
	auto res = (uint32_t) ((LSNumber*) a)->value >> (uint32_t) ((LSNumber*) b)->value;
	LSValue::delete_temporary(y);
	((LSNumber*) *x)->value = res;
	return res;
}

Compiler::value ValueSTD::op_in(Compiler& c, std::vector<Compiler::value> args, bool) {
	if (args[1].t->is_integer()) {
		return c.insn_invoke(Type::boolean, args, "Value.in_i");
	} else {
		return c.insn_invoke(Type::boolean, args, "Value.in");
	}
}
bool ValueSTD::in_i(LSValue* x, int k) {
	return x->in_i(k);
}
bool ValueSTD::in(LSValue* x, LSValue* y) {
	return x->in(y);
}

Compiler::value ValueSTD::op_swap_val(Compiler& c, std::vector<Compiler::value> args, bool) {
	auto x_addr = args[0];
	auto y_addr = args[1];
	auto x = c.insn_load(x_addr);
	auto y = c.insn_load(y_addr);
	c.insn_store(x_addr, y);
	c.insn_store(y_addr, x);
	return y;
}

LSValue* ValueSTD::op_swap_ptr(LSValue** x, LSValue** y) {
	auto tmp = *x;
	*x = *y;
	*y = tmp;
	return *x;
}

Compiler::value ValueSTD::op_call(Compiler& c, std::vector<Compiler::value> args, bool) {
	auto fun = args[1];
	return c.insn_call(fun, {args[0]});
}

Compiler::value ValueSTD::copy(Compiler& c, std::vector<Compiler::value> args, bool) {
	if (args[0].t->temporary) {
		return args[0];
	}
	return c.clone(args[0]);
}

Compiler::value ValueSTD::to_string(Compiler& c, std::vector<Compiler::value> args, bool) {
	if (args[0].t->is_bool()) {
		return c.insn_call(Type::tmp_string, args, "Boolean.to_string");
	} else if (args[0].t->is_integer()) {
		return c.insn_call(Type::tmp_string, args, "Number.int_to_string");
	} else if (args[0].t->is_long()) {
		return c.insn_call(Type::tmp_string, args, "Number.long_to_string");
	} else if (args[0].t->is_mpz_ptr()) {
		auto s = c.insn_call(Type::tmp_string, args, "Number.mpz_to_string");
		c.insn_delete_temporary(args[0]);
		return s;
	} else if (args[0].t->is_real()) {
		return c.insn_call(Type::tmp_string, args, "Number.real_to_string");
	} else {
		// Default type : pointer
		return c.insn_call(Type::tmp_string, args, "Value.json");
	}
}

Compiler::value ValueSTD::typeID(Compiler& c, std::vector<Compiler::value> args, bool) {
	return c.insn_typeof(args[0]);
}

int ValueSTD::absolute(LSValue* v) {
	return v->abso();
}
LSValue* ValueSTD::clone(LSValue* v) {
	return v->clone();
}
LSValue* ValueSTD::attr(LSValue* v, char* field) {
	return v->attr(field);
}
LSValue** ValueSTD::attrL(LSValue* v, char* field) {
	return v->attrL(field);
}
bool ValueSTD::ls_not(LSValue* x) {
	return x->ls_not();
}
LSValue* ValueSTD::ls_minus(LSValue* x) {
	return x->ls_minus();
}
LSValue* ValueSTD::ls_inc(LSValue* x) {
	return x->ls_inc();
}
LSValue* ValueSTD::ls_pre_inc(LSValue* x) {
	return x->ls_preinc();
}
LSValue* ValueSTD::ls_incl(LSValue** x) {
	return (*x)->ls_inc();
}
LSValue* ValueSTD::ls_pre_incl(LSValue** x) {
	return (*x)->ls_preinc();
}
LSValue* ValueSTD::ls_dec(LSValue* x) {
	return x->ls_dec();
}
LSValue* ValueSTD::ls_pre_dec(LSValue* x) {
	return x->ls_predec();
}
LSValue* ValueSTD::ls_decl(LSValue** x) {
	return (*x)->ls_dec();
}
LSValue* ValueSTD::ls_pre_decl(LSValue** x) {
	return (*x)->ls_predec();
}
LSValue* ValueSTD::ls_pre_tilde(LSValue* v) {
	return v->ls_tilde();
}
LSValue* ValueSTD::ls_add(LSValue* x, LSValue* y) {
	return x->add(y);
}
LSValue* ValueSTD::ls_add_eq(LSValue** x, LSValue* y) {
	return (*x)->add_eq(y);
}
LSValue* ValueSTD::ls_sub(LSValue* x, LSValue* y) {
	return x->sub(y);
}
LSValue* ValueSTD::ls_sub_eq(LSValue** x, LSValue* y) {
	return (*x)->sub_eq(y);
}
LSValue* ValueSTD::ls_mul(LSValue* x, LSValue* y) {
	return x->mul(y);
}
LSValue* ValueSTD::ls_mul_eq(LSValue** x, LSValue* y) {
	return (*x)->mul_eq(y);
}
LSValue* ValueSTD::ls_div(LSValue* x, LSValue* y) {
	return x->div(y);
}
LSValue* ValueSTD::ls_div_eq(LSValue** x, LSValue* y) {
	return (*x)->div_eq(y);
}
LSValue* ValueSTD::ls_int_div(LSValue* x, LSValue* y) {
	return x->int_div(y);
}
LSValue* ValueSTD::ls_int_div_eq(LSValue** x, LSValue* y) {
	return (*x)->int_div_eq(y);
}
LSValue* ValueSTD::ls_mod(LSValue* x, LSValue* y) {
	return x->mod(y);
}
LSValue* ValueSTD::ls_mod_eq(LSValue** x, LSValue* y) {
	return (*x)->mod_eq(y);
}
LSValue* ValueSTD::ls_double_mod(LSValue* x, LSValue* y) {
	return x->double_mod(y);
}
LSValue* ValueSTD::ls_double_mod_eq(LSValue** x, LSValue* y) {
	return (*x)->double_mod_eq(y);
}
LSValue* ValueSTD::ls_pow(LSValue* x, LSValue* y) {
	return x->pow(y);
}
LSValue* ValueSTD::ls_pow_eq(LSValue** x, LSValue* y) {
	return (*x)->pow_eq(y);
}

int ValueSTD::integer(const LSValue* x) {
	if (auto number = dynamic_cast<const LSNumber*>(x)) {
		return (int) number->value;
	} else if (auto boolean = dynamic_cast<const LSBoolean*>(x)) {
		return boolean->value ? 1 : 0;
	}
	LSValue::delete_temporary(x);
	throw vm::ExceptionObj(vm::Exception::NO_SUCH_OPERATOR);
}
double ValueSTD::real(const LSValue* x) {
	if (auto number = dynamic_cast<const LSNumber*>(x)) {
		return number->value;
	} else if (auto boolean = dynamic_cast<const LSBoolean*>(x)) {
		return boolean->value ? 1.0 : 0.0;
	}
	LSValue::delete_temporary(x);
	throw vm::ExceptionObj(vm::Exception::WRONG_ARGUMENT_TYPE);
}
double ValueSTD::real_delete(const LSValue* x) {
	if (auto number = dynamic_cast<const LSNumber*>(x)) {
		auto v = number->value;
		LSValue::delete_temporary(x);
		return v;
	} else if (auto boolean = dynamic_cast<const LSBoolean*>(x)) {
		return boolean->value ? 1.0 : 0.0;
	}
	LSValue::delete_temporary(x);
	throw vm::ExceptionObj(vm::Exception::WRONG_ARGUMENT_TYPE);
}
long ValueSTD::long_(const LSValue* x) {
	if (auto number = dynamic_cast<const LSNumber*>(x)) {
		return (long) number->value;
	} else if (auto boolean = dynamic_cast<const LSBoolean*>(x)) {
		return boolean->value ? 1l : 0l;
	}
	LSValue::delete_temporary(x);
	throw vm::ExceptionObj(vm::Exception::NO_SUCH_OPERATOR);
}

LSValue* ValueSTD::range(LSValue* a, int start, int end) {
	return a->range(start, end);
}
LSValue* ValueSTD::at(LSValue* array, LSValue* key) {
	return array->at(key);
}
LSValue** ValueSTD::atl(LSValue* array, LSValue* key) {
	return array->atL(key);
}
int ValueSTD::type(LSValue* x) {
	return x->type;
}
bool ValueSTD::is_null(LSValue* x) {
	return x->type == LSValue::NULLL;
}
bool ValueSTD::to_bool(LSValue* x) {
	return x->to_bool();
}
bool ValueSTD::eq(LSValue* x, LSValue* y) {
	return *x == *y;
}
bool ValueSTD::triple_eq(LSValue* x, LSValue* y) {
	return x->type == y->type and *x == *y ;
}
bool ValueSTD::lt(LSValue* x, LSValue* y) {
	return *x < *y;
}
bool ValueSTD::le(LSValue* x, LSValue* y) {
	return *x <= *y;
}
bool ValueSTD::gt(LSValue* x, LSValue* y) {
	return *x > *y;
}
bool ValueSTD::ge(LSValue* x, LSValue* y) {
	return *x >= *y;
}
void ValueSTD::delete_previous(LSValue* previous) {
	if (previous != nullptr) {
		LSValue::delete_ref(previous);
	}
}
int ValueSTD::get_int(LSNumber* x) {
	return (int) x->value;
}
LSValue* ValueSTD::get_class(LSValue* x) {
	return x->getClass();
}
void ValueSTD::export_context_variable_int(char* name, int v) {
	VM::current()->context->add_variable(name, reinterpret_cast<void*&>(v), Type::integer);
}
void ValueSTD::export_context_variable_long(char* name, long v) {
	VM::current()->context->add_variable(name, (void*) v, Type::long_);
}
void ValueSTD::export_context_variable_real(char* name, double v) {
	VM::current()->context->add_variable(name, reinterpret_cast<void*&>(v), Type::real);
}
void ValueSTD::export_context_variable(char* name, LSValue* v) {
	// std::cout << "export context variable " << name << " " << (void*) v << " " << v->refs << std::endl;
	auto n = LSValue::obj_count;
	v = v->move_inc();
	// Don't count the object cloned
	if (LSValue::obj_count > n) {
		LSValue::obj_count = n;
	}
	VM::current()->context->add_variable(name, v, Type::any);
}

}
