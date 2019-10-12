#include "ValueSTD.hpp"
#include "JsonSTD.hpp"
#include "../../analyzer/Context.hpp"
#include "../../type/Type.hpp"
#include "../../environment/Environment.hpp"
#if COMPILER
#include "../../vm/value/LSBoolean.hpp"
#include "../../vm/value/LSString.hpp"
#include "../../vm/value/LSNumber.hpp"
#include "../../vm/LSValue.hpp"
#include "../../vm/VM.hpp"
#endif

namespace ls {

ValueSTD::ValueSTD(Environment& env) : Module(env, "Value") {

	#if COMPILER
	env.value_class = std::make_unique<LSClass>(clazz.get());
	lsclass = env.value_class.get();
	#endif

	/*
	 * Static attributes
	 */
	static_field("unknown", env.any, ADDR(unknown));

	/*
	 * Attributes
	 */
	field("class", env.clazz(), ADDR(attr_class));

	/*
	 * Operators
	 */
	auto sV = env.template_("V");
	auto sT = env.template_("T");
	template_(sV, sT).
	operator_("=", {
		{sV, sT, sT, ADDR(op_store), 0, { new ChangeValueMutator() }, true}
	});

	operator_("is", {
		{env.const_any, env.const_class(), env.boolean, ADDR(op_instanceof)}
	});
	operator_("==", {
		{env.const_any, env.const_any, env.boolean, ADDR((void*) eq)},
		{env.const_any, env.const_any, env.boolean, ADDR(op_equals)}
	});
	operator_("===", {
		{env.const_any, env.const_any, env.boolean, ADDR((void*) triple_eq), LEGACY},
		{env.const_any, env.const_any, env.boolean, ADDR(op_triple_equals), LEGACY}
	});
	operator_("!=", {
		{env.const_any, env.const_any, env.boolean, ADDR(op_not_equals)}
	});
	operator_("<", {
		{env.const_any, env.const_any, env.boolean, ADDR((void*) lt)},
		{env.const_any, env.const_any, env.boolean, ADDR(op_lt)},
	});
	operator_("<=", {
		{env.const_any, env.const_any, env.boolean, ADDR((void*) le)},
		{env.const_any, env.const_any, env.boolean, ADDR(op_le)}
	});
	operator_(">", {
		{env.const_any, env.const_any, env.boolean, ADDR((void*) gt)},
		{env.const_any, env.const_any, env.boolean, ADDR(op_gt)}
	});
	operator_(">=", {
		{env.const_any, env.const_any, env.boolean, ADDR((void*) ge)},
		{env.const_any, env.const_any, env.boolean, ADDR(op_ge)}
	});
	operator_("and", {
		{env.const_any, env.const_any, env.boolean, ADDR(op_and)}
	});
	operator_("&&", {
		{env.const_any, env.const_any, env.boolean, ADDR(op_and)}
	});
	operator_("or", {
		{env.const_any, env.const_any, env.boolean, ADDR(op_or)}
	});
	operator_("||", {
		{env.const_any, env.const_any, env.boolean, ADDR(op_or)}
	});
	operator_("xor", {
		{env.const_any, env.const_any, env.boolean, ADDR(op_xor)}
	});
	operator_("+", {
		{env.const_any, env.const_any, env.any, ADDR((void*) ls_add), THROWS},
		{env.const_any, env.const_any, env.any, ADDR(op_add), THROWS},
	});
	operator_("+=", {
		{env.const_any, env.const_any, env.any, ADDR((void*) ls_add_eq), THROWS, { new ConvertMutator() }, true}
	});
	operator_("-", {
		{env.const_any, env.const_any, env.any, ADDR((void*) ls_sub), THROWS},
		{env.const_any, env.const_any, env.any, ADDR(op_sub), THROWS},
	});
	operator_("-=", {
		{env.const_any, env.const_any, env.any, ADDR((void*) ls_sub_eq), THROWS, {}, true}
	});
	operator_("*", {
		{env.const_any, env.const_any, env.any, ADDR((void*) ls_mul), THROWS},
		{env.const_any, env.const_any, env.any, ADDR(op_mul), THROWS}
	});
	operator_("*=", {
		{env.const_any, env.const_any, env.any, ADDR((void*) ls_mul_eq), THROWS, {}, true}
	});
	operator_("**", {
		{env.const_any, env.const_any, env.any, ADDR((void*) ls_pow), THROWS},
	});
	operator_("**=", {
		{env.const_any, env.const_any, env.any, ADDR((void*) ls_pow_eq), THROWS, {}, true}
	});
	operator_("/", {
		{env.const_any, env.const_any, env.any, ADDR((void*) ls_div), THROWS},
	});
	operator_("/=", {
		{env.const_any, env.const_any, env.any, ADDR((void*) ls_div_eq), THROWS, {}, true}
	});
	operator_("\\", {
		{env.const_any, env.const_any, env.any, ADDR((void*) ls_int_div), THROWS}
	});
	operator_("\\=", {
		{env.const_any, env.const_any, env.any, ADDR((void*) ls_int_div_eq), THROWS, {}, true}
	});
	operator_("%", {
		{env.const_any, env.const_any, env.any, ADDR((void*) ls_mod), THROWS}
	});
	operator_("%=", {
		{env.const_any, env.const_any, env.any, ADDR((void*) ls_mod_eq), THROWS, {}, true}
	});
	operator_("%%", {
		{env.const_any, env.const_any, env.any, ADDR((void*) ls_double_mod)},
	});
	operator_("%%=", {
		{env.const_any, env.const_any, env.any, ADDR((void*) ls_double_mod_eq), THROWS, {}, true}
	});
	operator_("&", {
		{env.const_any, env.const_any, env.integer, ADDR((void*) ls_bit_and), THROWS}
	});
	operator_("&=", {
		{env.any, env.const_any, env.integer, ADDR((void*) ls_bit_and_eq), THROWS, {}, true}
	});
	operator_("|", {
		{env.const_any, env.const_any, env.integer, ADDR((void*) ls_bit_or), THROWS}
	});
	operator_("|=", {
		{env.any, env.const_any, env.integer, ADDR((void*) ls_bit_or_eq), THROWS, {}, true}
	});
	operator_("^", {
		{env.const_any, env.const_any, env.integer, ADDR((void*) ls_bit_xor), THROWS}
	});
	operator_("^=", {
		{env.any, env.const_any, env.integer, ADDR((void*) ls_bit_xor_eq), THROWS, {}, true}
	});
	operator_("<<", {
		{env.const_any, env.const_any, env.integer, ADDR(bit_shift_left), THROWS}
	});
	operator_("<<=", {
		{env.const_any, env.const_any, env.integer, ADDR((void*) ls_bit_shift_left_eq), 0, {}, true},
		{env.integer, env.const_integer, env.integer, ADDR(bit_shift_left_eq), 0, {}, true},
	});
	operator_(">>", {
		{env.const_integer, env.const_integer, env.integer, ADDR(bit_shift_right), THROWS}
	});
	operator_(">>=", {
		{env.const_any, env.const_any, env.integer, ADDR((void*) ls_bit_shift_right_eq), 0, {}, true},
		{env.const_integer, env.const_integer, env.integer, ADDR(bit_shift_right_eq), 0, {}, true},
	});
	operator_(">>>", {
		{env.const_integer, env.const_integer, env.integer, ADDR(bit_shift_uright), THROWS}
	});
	operator_(">>>=", {
		{env.const_any, env.const_any, env.integer, ADDR((void*) ls_bit_shift_uright_eq), 0, {}, true},
		{env.const_integer, env.const_integer, env.integer, ADDR(bit_shift_uright_eq), 0, {}, true}
	});
	operator_("in", {
		{env.const_any, env.const_any, env.boolean, ADDR((void*) in)},
		{env.const_any, env.const_any, env.boolean, ADDR(op_in)},
	});
	operator_("<=>", {
		{env.any, env.any, env.any, ADDR((void*) &op_swap_ptr), 0, {}, true, true},
		{env.integer, env.integer, env.integer, ADDR(op_swap_val), 0, {}, true, true},
	});
	auto T = env.template_("T");
	auto R = env.template_("R");
	template_(T, R).
	operator_("~", {
		{T, Type::fun(R, {T}), R, ADDR(apply)},
	});

	/*
	 * Methods
	 */
	auto aT = env.template_("T");
	auto aR = env.template_("R");
	template_(aT, aR).
	method("apply", {
		{aR, {aT, Type::fun(aR, {aT})}, ADDR(apply)}
	});

	auto cT = env.template_("T");
	template_(cT).
	method("copy", {
		{Type::meta_temporary(cT), {cT}, ADDR(copy)}
	});

	method("string", {
		{env.string, {env.const_any}, ADDR(to_string)}
	});
	method("json", {
		{env.tmp_string, {env.const_any}, ADDR((void*) &LSValue::ls_json)},
		{env.tmp_string, {env.const_any}, ADDR(JsonSTD::encode)},
	});

	/*
	 * Internal
	 */
	method("typeID", {
		{env.integer, {env.const_any}, ADDR(typeID)}
	}, PRIVATE);
	method("move", {
		{env.any, {env.const_any}, ADDR((void*) &LSValue::move)}
	}, PRIVATE);
	method("move_inc", {
		{env.any, {env.const_any}, ADDR((void*) &LSValue::move_inc)}
	}, PRIVATE);
	method("ptr", {
		{env.tmp_any, {env.const_any}, ADDR((void*) &LSValue::move)}
	}, PRIVATE);
	method("absolute", {
		{env.integer, {env.const_any}, ADDR((void*) absolute), THROWS}
	}, PRIVATE);
	method("clone", {
		{env.any, {env.const_any}, ADDR((void*) clone)}
	}, PRIVATE);
	method("delete", {
		{env.void_, {env.const_any}, ADDR((void*) &LSValue::free)}
	}, PRIVATE);
	method("delete_tmp", {
		{env.void_, {env.const_any}, ADDR((void*) &LSValue::delete_temporary)}
	}, PRIVATE);
	method("dec_refs", {
		{env.void_, {env.const_any}, ADDR((void*) &LSValue::delete_ref)}
	}, PRIVATE);
	method("delete_ref", {
		{env.void_, {env.const_any}, ADDR((void*) &LSValue::delete_ref2)}
	}, PRIVATE);
	method("not", {
		{env.boolean, {env.const_any}, ADDR((void*) ls_not)}
	}, PRIVATE);
	method("minus", {
		{env.any, {env.const_any}, ADDR((void*) ls_minus)}
	}, PRIVATE);
	method("dec", {
		{env.any, {env.const_any}, ADDR((void*) ls_dec)}
	}, PRIVATE);
	method("pre_dec", {
		{env.any, {env.const_any}, ADDR((void*) ls_pre_dec)}
	}, PRIVATE);
	method("decl", {
		{env.any, {env.const_any}, ADDR((void*) ls_decl)}
	}, PRIVATE);
	method("pre_decl", {
		{env.any, {env.const_any}, ADDR((void*) ls_pre_decl)}
	}, PRIVATE);
	method("inc", {
		{env.any, {env.const_any}, ADDR((void*) ls_inc)}
	}, PRIVATE);
	method("pre_inc", {
		{env.any, {env.const_any}, ADDR((void*) ls_pre_inc)}
	}, PRIVATE);
	method("incl", {
		{env.any, {env.const_any}, ADDR((void*) ls_incl)}
	}, PRIVATE);
	method("pre_incl", {
		{env.any, {env.const_any}, ADDR((void*) ls_pre_incl)}
	}, PRIVATE);
	method("pre_tilde", {
		{env.any, {env.const_any}, ADDR((void*) ls_pre_tilde)}
	}, PRIVATE);
	method("attr", {
		{env.any, {env.i8_ptr, env.any, env.i8_ptr}, ADDR((void*) attr)},
	}, PRIVATE);
	method("attrL", {
		{env.any, {env.any, env.i8_ptr}, ADDR((void*) attrL)},
	}, PRIVATE);
	method("int", {
		{env.integer, {env.const_any}, ADDR((void*) integer)}
	}, PRIVATE);
	method("real", {
		{env.real, {env.const_any}, ADDR((void*) real)}
	}, PRIVATE);
	method("real_delete", {
		{env.real, {env.const_any}, ADDR((void*) real_delete)}
	}, PRIVATE);
	method("long", {
		{env.long_, {env.const_any}, ADDR((void*) long_)}
	}, PRIVATE);
	method("range", {
		{env.any, {env.const_any, env.integer, env.integer}, ADDR((void*) range)}
	}, PRIVATE);
	method("at", {
		{env.any, {env.const_any, env.const_any}, ADDR((void*) at)}
	}, PRIVATE);
	method("atl", {
		{env.any, {env.const_any, env.const_any}, ADDR((void*) atl)}
	}, PRIVATE);
	method("in_i", {
		{env.boolean, {env.const_any, env.integer}, ADDR((void*) in_i)}
	}, PRIVATE);
	method("in", {
		{env.boolean, {env.const_any, env.const_any}, ADDR((void*) in)}
	}, PRIVATE);
	method("is_null", {
		{env.boolean, {env.const_any}, ADDR((void*) is_null)}
	}, PRIVATE);
	method("to_bool", {
		{env.boolean, {env.const_any}, ADDR((void*) to_bool)}
	}, PRIVATE);
	method("type", {
		{env.integer, {env.const_any}, ADDR((void*) type)}
	}, PRIVATE);
	method("delete_previous", {
		{env.void_, {env.any}, ADDR((void*) delete_previous)}
	}, PRIVATE);
	method("get_int", {
		{env.integer, {env.any}, ADDR((void*) get_int)}
	}, PRIVATE);
	method("get_class", {
		{env.clazz(), {env.any}, ADDR((void*) get_class)}
	}, PRIVATE);
	method("export_ctx_var", {
		{env.void_, {env.i8_ptr, env.any}, ADDR((void*) export_context_variable)},
		{env.void_, {env.i8_ptr, env.integer}, ADDR((void*) export_context_variable_int)},
		{env.void_, {env.i8_ptr, env.long_}, ADDR((void*) export_context_variable_long)},
		{env.void_, {env.i8_ptr, env.real}, ADDR((void*) export_context_variable_real)},
	}, PRIVATE);
}

#if COMPILER
/*
 * Static attributes
 */
Compiler::value ValueSTD::unknown(Compiler& c) {
	return c.insn_call(c.env.any, {c.new_real(floor(1 + ((double) rand() / RAND_MAX) * 100))}, "Number.new");
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
		return { c.env };
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
		auto res = c.insn_call(c.env.boolean, {ap, bp}, "Value.operator<");
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
		auto res = c.insn_call(c.env.boolean, {ap, bp}, "Value.operator<");
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
		auto res = c.insn_call(c.env.boolean, {ap, bp}, "Value.operator>");
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
		auto res = c.insn_call(c.env.boolean, {c.insn_to_any(args[0]), c.insn_to_any(args[1])}, "Value.operator>=");
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
		return c.insn_invoke(c.env.integer, {args[0], c.insn_to_any(args[1])}, "Value.operator<<=");
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
		return c.insn_invoke(c.env.integer, {args[0], c.insn_to_any(args[1])}, "Value.operator>>=");
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
		return c.insn_invoke(c.env.integer, {args[0], c.insn_to_any(args[1])}, "Value.operator>>>=");
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
		return c.insn_invoke(c.env.boolean, args, "Value.in_i");
	} else {
		return c.insn_invoke(c.env.boolean, args, "Value.in");
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

Compiler::value ValueSTD::apply(Compiler& c, std::vector<Compiler::value> args, bool) {
	auto value = args[0];
	auto function = args[1];
	c.insn_inc_refs(value);
	auto r = c.insn_call(function, {value});
	c.insn_delete(value);
	return r;
}

Compiler::value ValueSTD::copy(Compiler& c, std::vector<Compiler::value> args, bool) {
	if (args[0].t->temporary) {
		return args[0];
	}
	return c.clone(args[0]);
}

Compiler::value ValueSTD::to_string(Compiler& c, std::vector<Compiler::value> args, bool) {
	if (args[0].t->is_bool()) {
		return c.insn_call(c.env.tmp_string, args, "Boolean.to_string");
	} else if (args[0].t->is_integer()) {
		return c.insn_call(c.env.tmp_string, args, "Number.int_to_string");
	} else if (args[0].t->is_long()) {
		return c.insn_call(c.env.tmp_string, args, "Number.long_to_string");
	} else if (args[0].t->is_mpz_ptr()) {
		auto s = c.insn_call(c.env.tmp_string, args, "Number.mpz_ptr_to_string");
		c.insn_delete_temporary(args[0]);
		return s;
	} else if (args[0].t->is_mpz()) {
		auto s = c.insn_call(c.env.tmp_string, args, "Number.mpz_to_string");
		c.insn_delete_temporary(args[0]);
		return s;
	} else if (args[0].t->is_real()) {
		return c.insn_call(c.env.tmp_string, args, "Number.real_to_string");
	} else {
		// Default type : pointer
		return c.insn_call(c.env.tmp_string, args, "Value.json");
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
LSValue* ValueSTD::attr(VM* vm, LSValue* v, char* field) {
	return v->attr(vm, field);
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
LSValue* ValueSTD::get_class(VM* vm, LSValue* x) {
	return x->getClass(vm);
}
void ValueSTD::export_context_variable_int(VM* vm, char* name, int v) {
	vm->context->add_variable(name, reinterpret_cast<void*&>(v), vm->env.integer);
}
void ValueSTD::export_context_variable_long(VM* vm, char* name, long v) {
	vm->context->add_variable(name, (void*) v, vm->env.long_);
}
void ValueSTD::export_context_variable_real(VM* vm, char* name, double v) {
	vm->context->add_variable(name, reinterpret_cast<void*&>(v), vm->env.real);
}
void ValueSTD::export_context_variable(VM* vm, char* name, LSValue* v) {
	// std::cout << "export context variable " << name << " " << (void*) v << " " << v->refs << std::endl;
	auto n = LSValue::obj_count;
	v = v->move_inc();
	// Don't count the object cloned
	if (LSValue::obj_count > n) {
		LSValue::obj_count = n;
	}
	vm->context->add_variable(name, v, vm->env.any);
}

#endif

}
