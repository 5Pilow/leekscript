#include "NumberSTD.hpp"
#include "ValueSTD.hpp"
#include "StringSTD.hpp"
#include "../../util/utf8.h"
#include "../../type/Type.hpp"
#include "../../environment/Environment.hpp"
#if COMPILER
#include "../../compiler/Compiler.hpp"
#include "../../vm/VM.hpp"
#include "../../vm/value/LSNumber.hpp"
#include "../../vm/value/LSMpz.hpp"
#include "../../vm/value/LSString.hpp"
#include "../../vm/value/LSBoolean.hpp"
#endif

namespace ls {

#if COMPILER
int mpz_log(__mpz_struct* n) {
	// std::cout << "mpz_log size " << n._mp_size << std::endl;
	// std::cout << "mpz_log alloc " << n._mp_alloc << std::endl;
	// std::cout << "mpz_log d " << n._mp_d << std::endl;
	int l = abs(n->_mp_size) - 1;
	unsigned long s = n->_mp_d[l];
	unsigned char r = 0;
	while (s >>= 1) r++;
	return 64 * l + r + 1;
}
#endif

NumberSTD::NumberSTD(Environment& env) : Module(env, "Number") {

	#if COMPILER
	env.number_class = std::make_unique<LSClass>(clazz.get());
	lsclass = env.number_class.get();
	#endif

	static_field("pi", env.real, ADDR(([](Compiler& c) { return c.new_real(3.14159265358979323846); })));
	static_field("e", env.real, ADDR(([](Compiler& c) { return c.new_real(M_E); })));
	static_field("phi", env.real, ADDR(([](Compiler& c) { return c.new_real(1.61803398874989484820); })));
	static_field("epsilon", env.real, ADDR(([](Compiler& c) { return c.new_real(std::numeric_limits<double>::epsilon()); })));

	/*
	 * Constructors
	 */
	constructor_({
		{env.any, {env.real}, ADDR((void*) &LSNumber::get)},
		{env.any, {env.i8->pointer(), env.tmp_mpz}, ADDR((void*) &LSMpz::get_from_tmp)},
		{env.any, {env.i8->pointer(), env.mpz}, ADDR((void*) &LSMpz::get_from_mpz)},
	});

	/*
	 * Operators
	 */
	operator_("==", {
		{env.mpz_ptr, env.mpz_ptr, env.boolean, ADDR(eq_mpz_mpz)},
		{env.mpz_ptr, env.integer, env.boolean, ADDR(eq_mpz_int)},
		{env.integer, env.mpz_ptr, env.boolean, ADDR(eq_int_mpz)},
	});
	operator_("+", {
		{env.mpz_ptr, env.mpz_ptr, env.tmp_mpz_ptr, ADDR(add_mpz_mpz)},
		{env.mpz_ptr, env.integer, env.tmp_mpz_ptr, ADDR(add_mpz_int)},
		{env.real, env.real, env.real, ADDR(add_real_real)},
		{env.long_, env.long_, env.long_, ADDR(add_real_real)},
		{env.const_integer, env.const_integer, env.integer, ADDR(add_real_real)},
		{env.const_integer, env.const_string, env.tmp_string, ADDR((void*) &StringSTD::add_int_r)}
	});
	operator_("+=", {
		{env.mpz_ptr, env.mpz_ptr, env.tmp_mpz_ptr, ADDR(add_eq_mpz_mpz), 0, { new ConvertMutator() }, true},
		{env.mpz_ptr, env.integer, env.tmp_mpz_ptr, ADDR(add_eq_mpz_int), 0, { new ConvertMutator() }, true},
		{env.real, env.real, env.real, ADDR(add_eq_real), 0, { new ConvertMutator() }, true},
		{env.long_, env.long_, env.long_, ADDR(add_eq_real), 0, { new ConvertMutator() }, true},
		{env.integer, env.integer, env.integer, ADDR(add_eq_real), 0, { new ConvertMutator() }, true}
	});
	operator_("-", {
		{env.mpz_ptr, env.mpz_ptr, env.tmp_mpz_ptr, ADDR(sub_mpz_mpz)},
		{env.mpz_ptr, env.integer, env.tmp_mpz_ptr, ADDR(sub_mpz_int)},
		{env.real, env.real, env.real, ADDR(sub_real_real)},
		{env.long_, env.long_, env.long_, ADDR(sub_real_real)},
		{env.const_integer, env.const_integer, env.integer, ADDR(sub_real_real)},
	});
	operator_("-=", {
		{env.mpz_ptr, env.mpz_ptr, env.tmp_mpz_ptr, ADDR(sub_eq_mpz_mpz)},
		{env.real, env.real, env.real, ADDR(sub_eq_real), 0, {}, true},
		{env.integer, env.integer, env.integer, ADDR(sub_eq_real), 0, {}, true}
	});
	operator_("*", {
		{env.real, env.real, env.real, ADDR(mul_real_real)},
		{env.long_, env.long_, env.long_, ADDR(mul_real_real)},
		{env.const_integer, env.mpz_ptr, env.tmp_mpz_ptr, ADDR(mul_int_mpz)},
		{env.mpz_ptr, env.integer, env.tmp_mpz_ptr, ADDR(mul_mpz_int)},
		{env.const_integer, env.const_integer, env.integer, ADDR(mul_real_real)},
		{env.const_integer, env.const_string, env.string, ADDR((void*) mul_int_string)},
		{env.mpz, env.mpz_ptr, env.tmp_mpz_ptr, ADDR(mul_tmp_mpz_mpz)},
		{env.mpz_ptr, env.mpz, env.tmp_mpz_ptr, ADDR(mul_mpz_tmp_mpz)},
		{env.mpz_ptr, env.mpz_ptr, env.tmp_mpz_ptr, ADDR(mul_mpz_mpz)},
		{env.mpz, env.integer, env.tmp_mpz_ptr, ADDR(mul_tmp_mpz_int)},
	});
	operator_("*=", {
		{env.mpz_ptr, env.mpz_ptr, env.tmp_mpz, ADDR(mul_eq_mpz_mpz)},
		{env.mpz_ptr, env.integer, env.tmp_mpz_ptr, ADDR(mul_eq_mpz_int)},
		{env.real, env.real, env.real, ADDR(mul_eq_real), 0, {}, true},
		{env.integer, env.integer, env.integer, ADDR(mul_eq_real), 0, {}, true}
	});

	auto pX = env.template_("X");
	auto pY = env.template_("Y");
	template_(pX, pY).
	operator_("**", {
		{pX, pY, Type::meta_mul(pX, pY), ADDR(pow_real_real)},
		{env.mpz_ptr, env.mpz_ptr, env.tmp_mpz_ptr, ADDR(pow_mpz_mpz), THROWS},
		{env.mpz_ptr, env.integer, env.tmp_mpz_ptr, ADDR(pow_mpz_int), THROWS},
	});

	operator_("**=", {
		{env.mpz_ptr, env.mpz_ptr, env.tmp_mpz_ptr, ADDR(pow_eq_mpz_mpz)},
		{env.real, env.real, env.real, ADDR(pow_eq_real), 0, {}, true},
		{env.integer, env.integer, env.integer, ADDR(pow_eq_real), 0, {}, true}
	});
	operator_("/", {
		{env.number, env.number, env.real, ADDR(div_val_val)}
	});
	operator_("/=", {
		{env.mpz, env.mpz, env.tmp_mpz, ADDR(div_eq_mpz_mpz)},
		{env.real, env.real, env.real, ADDR(div_eq_real), THROWS, {}, true}
	});
	operator_("\\", {
		{env.long_, env.long_ , env.long_, ADDR(int_div_val_val)},
		{env.integer, env.integer, env.integer, ADDR(int_div_val_val)},
	});
	operator_("\\=", {
		{env.number, env.number, env.integer, ADDR(int_div_eq_val_val), 0, {}, true}
	});
	operator_("<", {
		{env.number, env.number, env.boolean, ADDR(lt)}
	});
	operator_("<=", {
		{env.number, env.number, env.boolean, ADDR(le)}
	});
	operator_(">", {
		{env.number, env.number, env.boolean, ADDR(gt)}
	});
	operator_(">=", {
		{env.number, env.number, env.boolean, ADDR(ge)}
	});
	operator_("%", {
		{env.real, env.real, env.real, ADDR(mod), THROWS},
		{env.long_, env.long_, env.long_, ADDR(mod), THROWS},
		{env.const_integer, env.const_integer, env.integer, ADDR(mod), THROWS},
		{env.mpz_ptr, env.mpz_ptr, env.tmp_mpz_ptr, ADDR(mod_mpz_mpz), THROWS},
	});
	operator_("%%", {
		{env.real, env.real, env.real, ADDR(double_mod)},
		{env.long_, env.long_, env.long_, ADDR(double_mod)},
		{env.const_integer, env.const_integer, env.integer, ADDR(double_mod)},
	});
	operator_("%%=", {
		{env.real, env.real, env.real, ADDR(double_mod_eq), 0, {}, true},
		{env.long_, env.long_, env.long_, ADDR(double_mod_eq), 0, {}, true},
		{env.const_integer, env.const_integer, env.integer, ADDR(double_mod_eq), 0, {}, true},
	});
	operator_("%=", {
		{env.mpz, env.mpz, env.tmp_mpz, ADDR(mod_eq_mpz_mpz), THROWS},
		{env.real, env.real, env.real, ADDR(mod_eq_real), THROWS, {}, true},
		{env.integer, env.integer, env.integer, ADDR(mod_eq_real), THROWS, {}, true},
		{env.integer, env.long_, env.integer, ADDR(mod_eq_real), THROWS, {}, true},
	});
	operator_("&", {
		{env.const_integer, env.const_integer, env.integer, ADDR(bit_and)}
	});
	operator_("&=", {
		{env.integer, env.const_integer, env.integer, ADDR(bit_and_eq), 0, {}, true}
	});
	operator_("|", {
		{env.const_integer, env.const_integer, env.integer, ADDR(bit_or)}
	});
	operator_("|=", {
		{env.integer, env.const_integer, env.integer, ADDR(bit_or_eq), 0, {}, true}
	});
	operator_("^", {
		{env.const_integer, env.const_integer, env.integer, ADDR(bit_xor)}
	});
	operator_("^=", {
		{env.integer, env.const_integer, env.integer, ADDR(bit_xor_eq), 0, {}, true}
	});

	/*
	 * Methods
	 */
	method("copy", {
		{env.real, {env.real}, ADDR(ValueSTD::copy)},
		{env.long_, {env.long_}, ADDR(ValueSTD::copy)},
		{env.integer, {env.const_integer}, ADDR(ValueSTD::copy)},
		{env.mpz, {env.mpz}, ADDR(ValueSTD::copy)},
	});

	auto intT = env.template_("T");
	template_(intT).
	method("int", {
		{env.integer, {intT}, ADDR(_int)},
	});

	auto longT = env.template_("T");
	template_(longT).
	method("long", {
		{env.long_, {longT}, ADDR(_long)},
	});

	auto absT = env.template_("T");
	template_(absT).
	method("abs", {
		{env.any, {env.any}, ADDR((void*) abs_ptr)},
		{absT, {absT}, ADDR(abs)}
	});
	int (*abs_int)(int) = std::abs;
	long (*abs_long)(long) = std::abs;
	double (*abs_real)(double) = std::fabs;
	method("m_abs", {
		{env.integer, {env.integer}, (void*) abs_int},
		{env.long_, {env.long_}, (void*) abs_long},
		{env.real, {env.real}, (void*) abs_real},
	});
	method("acos", {
		{env.real, {env.any}, ADDR((void*) acos_ptr)},
		{env.real, {env.real}, ADDR(acos_real)},
	});
	method("asin", {
		{env.real, {env.any}, ADDR((void*) asin_ptr)},
		{env.real, {env.real}, ADDR(asin_real)},
	});
	method("atan", {
		{env.real, {env.any}, ADDR((void*) atan_ptr)},
		{env.real, {env.real}, ADDR(atan_real)},
	});

	// template<T1 : number, T2 : number>
	// T1 × T2 atan2(T1 x, T2 y)
	auto atan2T1 = Type::meta_base_of(env.template_("T1"), env.number);
	auto atan2T2 = Type::meta_base_of(env.template_("T2"), env.number);
	template_(atan2T1, atan2T2).
	method("atan2", {
		{env.any, {env.any, env.any}, ADDR((void*) atan2_ptr_ptr), DEFAULT},
		{env.real, {atan2T1, atan2T2}, ADDR(atan2)},
	});

	double (*cbrtreal)(double) = std::cbrt;
	double (*cbrtint)(int) = std::cbrt;
	method("cbrt", {
		{env.any, {env.any}, ADDR((void*) cbrt_ptr)},
		{env.real, {env.real}, (void*) cbrtreal},
		{env.real, {env.integer}, (void*) cbrtint},
	});
	method("ceil", {
		{env.integer, {env.any}, ADDR((void*) ceil_ptr)},
		{env.integer, {env.real}, ADDR(ceil_real)},
		{env.integer, {env.integer}, ADDR(ceil_int)},
	});
	method("char", {
		{env.tmp_string, {env.const_any}, ADDR((void*) char_ptr)},
		{env.tmp_string, {env.real}, ADDR((void*) char_real)},
		{env.tmp_string, {env.const_integer}, ADDR((void*) char_int)},
	});
	method("cos", {
		{env.any, {env.any}, ADDR((void*) cos_ptr)},
		{env.real, {env.real}, ADDR(cos_real)},
	});
	method("exp", {
		{env.real, {env.any}, ADDR((void*) exp_ptr)},
		{env.real, {env.real}, ADDR(exp_real)},
	});
	auto fold_fun_type = Type::fun_object(env.any, {env.any, env.integer});
	auto fold_fun = ADDR(&LSNumber::ls_fold<LSFunction*>);
	method("fold", {
		{env.any, {env.any, fold_fun_type, env.any}, (void*) fold_fun},
		{env.any, {env.any, fold_fun_type, env.any}, ADDR(fold)}
	});

	method("floor", {
		{env.long_, {env.any}, ADDR((void*) floor_ptr)},
		{env.long_, {env.real}, ADDR(floor_real)},
		{env.long_, {env.long_}, ADDR(floor_real)},
		{env.integer, {env.integer}, ADDR(floor_real)},
	});
	method("hypot", {
		{env.real, {env.any, env.any}, ADDR(hypot_ptr_ptr)},
		{env.real, {env.integer, env.integer}, (void*) std::hypot<int, int>},
		{env.real, {env.real, env.real}, (void*) std::hypot<double, double>},
	});
	method("log", {
		{env.real, {env.any}, ADDR((void*) log_ptr)},
		{env.real, {env.real}, ADDR(log_real)},
	});
	method("log10", {
		{env.real, {env.any}, ADDR((void*) log10_ptr)},
		{env.real, {env.long_}, ADDR(log10_real)},
		{env.real, {env.real}, ADDR(log10_real)},
	});

	// template<T1 : number, T2 : number>
	// T1 × T2 max(T1 x, T2 y)
	auto maxT1 = Type::meta_base_of(env.template_("T1"), env.number);
	auto maxT2 = Type::meta_base_of(env.template_("T2"), env.number);
	template_(maxT1, maxT2).
	method("max", {
		{env.any, {env.any, env.any}, ADDR((void*) max_ptr_ptr), DEFAULT},
		{Type::meta_mul(maxT1, maxT2), {maxT1, maxT2}, ADDR(max)},
	});

	// template<T1 : number, T2 : number>
	// T1 × T2 min(T1 x, T2 y)
	auto minT1 = Type::meta_base_of(env.template_("T1"), env.number);
	auto minT2 = Type::meta_base_of(env.template_("T2"), env.number);
	template_(minT1, minT2).
	method("min", {
		{env.real, {env.any, env.any}, ADDR((void*) min_ptr_ptr), DEFAULT},
		{Type::meta_mul(minT1, minT2), {minT1, minT2}, ADDR(min)},
	});

	method("pow", {
		{env.real, {env.any, env.any}, ADDR((void*) pow_ptr)},
		{env.long_, {env.long_, env.integer}, ADDR(pow_int)},
		{env.real, {env.long_, env.long_}, ADDR(pow_int)},
	});
	method("round", {
		{env.integer, {env.any}, ADDR((void*) round_ptr)},
		{env.integer, {env.any}, ADDR((void*) round_ptr)},
		{env.integer, {env.real}, ADDR(round_real)},
		{env.integer, {env.integer}, ADDR(round_int)}
	});
	method("rand", {
		{env.real, {}, ADDR((void*) rand01)},
	});
	method("randInt", {
		{env.integer, {env.integer, env.integer}, ADDR((void*) rand_int)},
	});
	method("randFloat", {
		{env.real, {env.real, env.real}, ADDR((void*) rand_real)},
	});
	method("signum", {
		{env.integer, {env.any}, ADDR((void*) signum_ptr)},
		{env.integer, {env.number}, ADDR(signum)},
	});
	method("sin", {
		{env.real, {env.any}, ADDR((void*) sin_ptr)},
		{env.real, {env.any}, ADDR((void*) sin_ptr)},
		{env.real, {env.real}, ADDR(sin_real)},
	});
	double (*sqrt_real)(double) = std::sqrt;
	method("sqrt", {
		{env.real, {env.any}, ADDR((void*) sqrt_ptr)},
		{env.tmp_mpz_ptr, {env.mpz_ptr}, ADDR(sqrt_mpz)},
		{env.real, {env.real}, (void*) sqrt_real},
		{env.real, {env.integer}, ADDR((void*) std::sqrt<int>)},
	});
	method("tan", {
		{env.real, {env.any}, ADDR((void*) tan_ptr)},
		{env.real, {env.any}, ADDR((void*) tan_ptr)},
		{env.real, {env.real}, ADDR(tan_real)},
	});
	method("toDegrees", {
		{env.any, {env.any}, ADDR((void*) toDegrees_ptr)},
		{env.real, {env.any}, ADDR(toDegrees)},
	});
	method("toRadians", {
		{env.any, {env.any}, ADDR((void*) toRadians_ptr)},
		{env.real, {env.any}, ADDR(toRadians)},
	});
	method("isInteger", {
		{env.any, {env.any}, ADDR((void*) isInteger_ptr)},
		{env.boolean, {env.any}, ADDR(isInteger)},
	});
	method("isPrime", {
		{env.boolean, {env.integer}, ADDR((void*) is_prime_number<int>)},
		{env.integer, {env.mpz_ptr}, ADDR(is_prime)},
		{env.boolean, {env.long_}, ADDR((void*) is_prime_number<long>)},
	});
	method("isPalindrome", {
		{env.boolean, {env.integer}, ADDR((void*) is_palindrome<int>)}
	});

	/** Internal **/
	method("powdd", {
		{env.real, {env.real, env.real}, (void*) std::pow<double, double>}
	});
	method("powli", {
		{env.real, {env.long_, env.integer}, (void*) std::pow<long, int>}
	});
	method("powii", {
		{env.real, {env.integer, env.integer}, (void*) std::pow<int, int>}
	});
	method("mpz_init", {
		{env.void_, {env.mpz_ptr}, ADDR((void*) mpz_init)}
	});
	method("mpz_init_set", {
		{env.void_, {env.mpz_ptr}, ADDR((void*) mpz_init_set)}
	});
	method("mpz_init_str", {
		{env.void_, {env.mpz_ptr, env.i8_ptr, env.integer}, ADDR((void*) mpz_init_set_str)}
	});
	method("mpz_get_ui", {
		{{env.long_}, {env.mpz_ptr}, ADDR((void*) mpz_get_ui)}
	});
	method("mpz_get_si", {
		{{env.long_}, {env.mpz_ptr}, ADDR((void*) mpz_get_si)}
	});
	method("mpz_add", {
		{env.void_, {env.mpz_ptr, env.mpz_ptr, env.mpz_ptr}, ADDR((void*) mpz_add)}
	});
	method("mpz_add_ui", {
		{env.void_, {env.mpz_ptr, env.long_, env.mpz_ptr}, ADDR((void*) mpz_add_ui)}
	});
	method("mpz_sub", {
		{env.void_, {env.mpz_ptr, env.mpz_ptr, env.mpz_ptr}, ADDR((void*) mpz_sub)}
	});
	method("mpz_sub_ui", {
		{env.void_, {env.mpz_ptr, env.long_, env.mpz_ptr}, ADDR((void*) mpz_sub_ui)}
	});
	method("mpz_mul", {
		{env.void_, {env.mpz_ptr, env.mpz_ptr, env.mpz_ptr}, ADDR((void*) mpz_mul)}
	});
	method("mpz_mul_si", {
		{env.void_, {env.mpz_ptr, env.long_, env.mpz_ptr}, ADDR((void*) mpz_mul_si)}
	});
	method("mpz_pow_ui", {
		{env.void_, {env.mpz_ptr, env.mpz_ptr, env.integer}, ADDR((void*) mpz_pow_ui)}
	});
	method("mpz_mod", {
		{env.void_, {env.mpz_ptr, env.mpz_ptr, env.mpz_ptr}, ADDR((void*) mpz_mod)}
	});
	method("mpz_probab_prime_p", {
		{env.integer, {env.mpz_ptr, env.integer}, ADDR((void*) mpz_probab_prime_p)}
	});
	method("mpz_neg", {
		{env.void_, {env.mpz_ptr, env.mpz_ptr}, ADDR((void*) mpz_neg)}
	});
	method("mpz_log", {
		{{env.integer}, {env.mpz_ptr}, ADDR((void*) mpz_log)}
	});
	method("mpz_cmp", {
		{{env.integer}, {env.mpz_ptr, env.mpz_ptr}, ADDR((void*) mpz_cmp)}
	});
	method("_mpz_cmp_si", {
		{{env.integer}, {env.mpz_ptr, env.long_}, ADDR((void*) _mpz_cmp_si)}
	});
	method("mpz_sqrt", {
		{env.void_, {env.mpz_ptr, env.mpz_ptr}, ADDR((void*) mpz_sqrt)}
	});
	method("mpz_clear", {
		{env.void_, {env.mpz_ptr}, ADDR((void*) mpz_clear)}
	});
	method("int_to_string", {
		{env.tmp_string, {env.integer}, ADDR((void*) int_to_string)}
	});
	method("long_to_string", {
		{env.tmp_string, {env.long_}, ADDR((void*) long_to_string)}
	});
	method("real_to_string", {
		{env.tmp_string, {env.real}, ADDR((void*) real_to_string)}
	});
	method("mpz_to_string", {
		{env.tmp_string, {env.mpz_ptr}, ADDR((void*) mpz_to_string)}
	});
	double (*logreal)(double) = std::log;
	method("m_log", {
		{env.real, {env.integer}, (void*) std::log<int>},
		{env.real, {env.long_}, (void*) std::log<long>},
		{env.real, {env.real}, (void*) logreal},
	});
	double (*log10real)(double) = std::log10;
	method("m_log10", {
		{env.real, {env.integer}, (void*) std::log10<int>},
		{env.real, {env.long_}, (void*) std::log10<long>},
		{env.real, {env.real}, (void*) log10real},
	});
	double (*expreal)(double) = std::exp;
	method("m_exp", {
		{env.integer, {env.integer}, (void*) std::exp<int>},
		{env.real, {env.real}, (void*) expreal},
	});
	double (*floorreal)(double) = std::floor;
	method("m_floor", {
		{env.real, {env.real}, (void*) floorreal},
	});
	double (*roundreal)(double) = std::round;
	method("m_round", {
		{env.real, {env.real}, (void*) roundreal},
	});
	double (*ceilreal)(double) = std::ceil;
	method("m_ceil", {
		{env.real, {env.real}, (void*) ceilreal},
	});
	method("m_max", {
		{env.integer, {env.integer, env.integer}, (void*) max_fun<int>},
		{env.long_, {env.long_, env.long_}, (void*) max_fun<long>},
		{env.real, {env.real, env.real}, (void*) max_fun<double>},
	});
	method("m_min", {
		{env.integer, {env.integer, env.integer}, (void*) min_fun<int>},
		{env.long_, {env.long_, env.long_}, (void*) min_fun<long>},
		{env.real, {env.real, env.real}, (void*) min_fun<double>},
	});
	double (*cosreal)(double) = std::cos;
	method("m_cos", {
		{env.real, {env.integer}, (void*) std::cos<int>},
		{env.real, {env.long_}, (void*) std::cos<long>},
		{env.real, {env.real}, (void*) cosreal},
	});
	double (*sinreal)(double) = std::sin;
	method("m_sin", {
		{env.real, {env.integer}, (void*) std::sin<int>},
		{env.real, {env.long_}, (void*) std::sin<long>},
		{env.real, {env.real}, (void*) sinreal},
	});
	double (*tanreal)(double) = std::tan;
	method("m_tan", {
		{env.real, {env.integer}, (void*) std::tan<int>},
		{env.real, {env.long_}, (void*) std::tan<long>},
		{env.real, {env.real}, (void*) tanreal},
	});
	double (*acosreal)(double) = std::acos;
	method("m_acos", {
		{env.real, {env.integer}, (void*) std::acos<int>},
		{env.real, {env.long_}, (void*) std::acos<long>},
		{env.real, {env.real}, (void*) acosreal},
	});
	double (*asinreal)(double) = std::asin;
	method("m_asin", {
		{env.real, {env.integer}, (void*) std::asin<int>},
		{env.real, {env.long_}, (void*) std::asin<long>},
		{env.real, {env.real}, (void*) asinreal},
	});
	double (*atanreal)(double) = std::atan;
	method("m_atan", {
		{env.real, {env.integer}, (void*) std::atan<int>},
		{env.real, {env.long_}, (void*) std::atan<long>},
		{env.real, {env.real}, (void*) atanreal},
	});
	double (*atan2real)(double, double) = std::atan2;
	method("m_atan2", {
		{env.real, {env.integer, env.integer}, (void*) std::atan2<int, int>},
		{env.real, {env.long_, env.long_}, (void*) std::atan2<long, long>},
		{env.real, {env.real, env.real}, (void*) atan2real},
	});
	method("m_isint", {
		{env.boolean, {env.any}, ADDR((void*) isint)},
	});
}

#if COMPILER

Compiler::value NumberSTD::eq_mpz_mpz(Compiler& c, std::vector<Compiler::value> args, int) {
	auto r = c.insn_eq(c.insn_call(c.env.integer, args, "Number.mpz_cmp"), c.new_integer(0));
	c.insn_delete_temporary(args[0]);
	c.insn_delete_temporary(args[1]);
	return r;
}
Compiler::value NumberSTD::eq_int_mpz(Compiler& c, std::vector<Compiler::value> args, int) {
	auto r = c.insn_eq(c.insn_call(c.env.integer, {args[1], args[0]}, "Number._mpz_cmp_si"), c.new_integer(0));
	c.insn_delete_temporary(args[1]);
	return r;
}
Compiler::value NumberSTD::eq_mpz_int(Compiler& c, std::vector<Compiler::value> args, int) {
	auto r = c.insn_eq(c.insn_call(c.env.integer, args, "Number._mpz_cmp_si"), c.new_integer(0));
	c.insn_delete_temporary(args[0]);
	return r;
}

Compiler::value NumberSTD::add_real_real(Compiler& c, std::vector<Compiler::value> args, int) {
	return c.insn_add(args[0], args[1]);
}

LSValue* NumberSTD::add_int_ptr(int a, LSValue* b) {
	return LSNumber::get(a)->add(b);
}

Compiler::value NumberSTD::add_mpz_mpz(Compiler& c, std::vector<Compiler::value> args, int) {
	auto r = [&]() {
		if (args[0].t->temporary) return args[0];
		if (args[1].t->temporary) return args[1];
		return c.new_mpz();
	}();
	c.insn_call(c.env.void_, {r, args[0], args[1]}, "Number.mpz_add");
	if (args[1] != r) c.insn_delete_temporary(args[1]);
	return r;
}

Compiler::value NumberSTD::add_mpz_int(Compiler& c, std::vector<Compiler::value> args, int) {
	auto r = args[0].t->temporary ? args[0] : c.new_mpz();
	c.insn_if(c.insn_gt(args[1], c.new_integer(0)), [&]() {
		c.insn_call(c.env.void_, {r, args[0], args[1]}, "Number.mpz_add_ui");
	}, [&]() {
		c.insn_call(c.env.void_, {r, args[0], c.insn_neg(args[1])}, "Number.mpz_sub_ui");
	});
	return r;
}

Compiler::value NumberSTD::add_eq_mpz_mpz(Compiler& c, std::vector<Compiler::value> args, int flags) {
	c.insn_call(c.env.void_, {args[0], args[0], args[1]}, "Number.mpz_add");
	c.insn_delete_temporary(args[1]);
	return flags & NO_RETURN ? Compiler::value { c.env } : c.insn_clone_mpz(args[0]);
}
Compiler::value NumberSTD::add_eq_mpz_int(Compiler& c, std::vector<Compiler::value> args, int flags) {
	c.insn_if(c.insn_gt(args[1], c.new_integer(0)), [&]() {
		c.insn_call(c.env.void_, {args[0], args[0], args[1]}, "Number.mpz_add_ui");
	}, [&]() {
		c.insn_call(c.env.void_, {args[0], args[0], c.insn_neg(args[1])}, "Number.mpz_sub_ui");
	});
	return flags & NO_RETURN ? Compiler::value { c.env } : c.insn_clone_mpz(args[0]);
}

Compiler::value NumberSTD::add_eq_real(Compiler& c, std::vector<Compiler::value> args, int) {
	auto x = c.insn_load(args[0]);
	auto sum = c.insn_add(x, args[1]);
	c.insn_store(args[0], sum);
	return sum;
}

Compiler::value NumberSTD::sub_real_real(Compiler& c, std::vector<Compiler::value> args, int) {
	return c.insn_sub(args[0], args[1]);
}

Compiler::value NumberSTD::sub_mpz_mpz(Compiler& c, std::vector<Compiler::value> args, int) {
	auto r = [&]() {
		if (args[0].t->temporary) return args[0];
		if (args[1].t->temporary) return args[1];
		return c.new_mpz();
	}();
	c.insn_call(c.env.void_, {r, args[0], args[1]}, "Number.mpz_sub");
	if (args[1] != r) c.insn_delete_temporary(args[1]);
	return r;
}

Compiler::value NumberSTD::sub_mpz_int(Compiler& c, std::vector<Compiler::value> args, int) {
	auto a = args[0];
	auto b = args[1];

	Compiler::label label_then = c.insn_init_label("then");
	Compiler::label label_else = c.insn_init_label("else");
	Compiler::label label_end = c.insn_init_label("end");

	auto cond = c.insn_lt(b, c.new_integer(0));
	c.insn_if_new(cond, &label_then, &label_else);

	c.insn_label(&label_then);
	auto neg_b = c.insn_neg(b);
	auto r1 = c.new_mpz();
	c.insn_call(c.env.void_, {r1, a, neg_b}, "Number.mpz_add_ui");
	c.insn_branch(&label_end);
	label_then.block = c.builder.GetInsertBlock();

	c.insn_label(&label_else);
	auto r2 = c.new_mpz();
	c.insn_call(c.env.void_, {r2, a, b}, "Number.mpz_sub_ui");
	c.insn_branch(&label_end);
	label_else.block = c.builder.GetInsertBlock();

	c.insn_label(&label_end);
	auto PN = c.builder.CreatePHI(c.env.mpz_ptr->llvm(c), 2);
	PN->addIncoming(r1.v, label_then.block);
	PN->addIncoming(r2.v, label_else.block);
	c.insn_delete_temporary(a);
	return {PN, c.env.tmp_mpz_ptr};
}

Compiler::value NumberSTD::sub_eq_mpz_mpz(Compiler& c, std::vector<Compiler::value> args, int) {
	// auto a_addr = c.insn_address_of(args[0]);
	// auto b_addr = c.insn_address_of(args[1]);
	// c.insn_call(env.void_, {a_addr, a_addr, b_addr}, &mpz_sub);
	return c.insn_clone_mpz(args[0]);
}

Compiler::value NumberSTD::sub_eq_real(Compiler& c, std::vector<Compiler::value> args, int) {
	auto x = c.insn_load(args[0]);
	auto sum = c.insn_sub(x, args[1]);
	c.insn_store(args[0], sum);
	return sum;
}

Compiler::value NumberSTD::mul_real_real(Compiler& c, std::vector<Compiler::value> args, int) {
	return c.insn_mul(args[0], args[1]);
}

Compiler::value NumberSTD::mul_int_mpz(Compiler& c, std::vector<Compiler::value> args, int) {
	auto r = args[1].t->temporary ? args[1] : c.new_mpz();
	c.insn_call(c.env.void_, {r, args[1], args[0]}, "Number.mpz_mul_si");
	return r;
}

Compiler::value NumberSTD::mul_mpz_int(Compiler& c, std::vector<Compiler::value> args, int) {
	auto r = args[0].t->temporary ? args[0] : c.new_mpz();
	c.insn_call(c.env.void_, {r, args[0], args[1]}, "Number.mpz_mul_si");
	return r;
}

LSValue* NumberSTD::mul_int_string(int a, LSString* b) {
	return b->mul(LSNumber::get(a));
}

Compiler::value NumberSTD::mul_mpz_mpz(Compiler& c, std::vector<Compiler::value> args, int) {
	auto r = [&]() {
		if (args[0].t->temporary) return args[0];
		if (args[1].t->temporary) return args[1];
		return c.new_mpz();
	}();
	c.insn_call(c.env.void_, {r, args[0], args[1]}, "Number.mpz_mul");
	if (args[1] != r) c.insn_delete_temporary(args[1]);
	return r;
}

Compiler::value NumberSTD::mul_tmp_mpz_mpz(Compiler& c, std::vector<Compiler::value> args, int) {
	auto r = c.create_entry("m", c.env.tmp_mpz);
	c.insn_store(r, args[0]);
	c.insn_call(c.env.void_, {r, r, args[1]}, "Number.mpz_mul");
	c.insn_delete_temporary(args[1]);
	return r;
}
Compiler::value NumberSTD::mul_tmp_mpz_int(Compiler& c, std::vector<Compiler::value> args, int) {
	auto r = c.create_entry("m", c.env.tmp_mpz);
	c.insn_store(r, args[0]);
	c.insn_call(c.env.void_, {r, r, args[1]}, "Number.mpz_mul_si");
	return r;
}
Compiler::value NumberSTD::mul_mpz_tmp_mpz(Compiler& c, std::vector<Compiler::value> args, int) {
	auto r = c.create_entry("m", c.env.tmp_mpz);
	c.insn_store(r, args[1]);
	c.insn_call(c.env.void_, {r, r, args[0]}, "Number.mpz_mul");
	c.insn_delete_temporary(args[0]);
	return r;
}

Compiler::value NumberSTD::mul_eq_mpz_int(Compiler& c, std::vector<Compiler::value> args, int flags) {
	c.insn_call(c.env.void_, {args[0], args[0], args[1]}, "Number.mpz_mul_si");
	if (flags & Module::NO_RETURN) {
		return { c.env };
	} else {
		return c.insn_clone_mpz(args[0]);
	}
}

Compiler::value NumberSTD::mul_eq_mpz_mpz(Compiler& c, std::vector<Compiler::value> args, int flags) {
	c.insn_call(c.env.void_, {args[0], args[0], args[1]}, "Number.mpz_mul");
	if (args[1].t->temporary) {
		c.insn_delete_mpz(args[1]);
	}
	if (flags & Module::NO_RETURN) {
		return { c.env };
	} else {
		return c.insn_clone_mpz(args[0]);
	}
}

Compiler::value NumberSTD::mul_eq_real(Compiler& c, std::vector<Compiler::value> args, int) {
	auto x = c.insn_load(args[0]);
	auto sum = c.insn_mul(x, args[1]);
	c.insn_store(args[0], sum);
	return sum;
}

Compiler::value NumberSTD::div_val_val(Compiler& c, std::vector<Compiler::value> args, int) {
	return c.insn_div(args[0], args[1]);
}

Compiler::value NumberSTD::pow_real_real(Compiler& c, std::vector<Compiler::value> args, int) {
	auto r = c.insn_pow(args[0], args[1]);
	if (args[0].t->is_integer() && args[1].t->is_integer()) {
		r = c.to_int(r);
	}
	return r;
}

Compiler::value NumberSTD::div_eq_mpz_mpz(Compiler& c, std::vector<Compiler::value> args, int) {
	// auto a_addr = c.insn_address_of(args[0]);
	// auto b_addr = c.insn_address_of(args[1]);
	// c.insn_call(c.env.void_, {a_addr, a_addr, b_addr}, &mpz_div);
	return c.insn_clone_mpz(args[0]);
}

Compiler::value NumberSTD::div_eq_real(Compiler& c, std::vector<Compiler::value> args, int) {
	auto x = c.insn_load(args[0]);
	auto sum = c.insn_div(x, args[1]);
	c.insn_store(args[0], c.insn_convert(sum, x.t));
	return sum;
}

Compiler::value NumberSTD::int_div_val_val(Compiler& c, std::vector<Compiler::value> args, int) {
	return c.insn_int_div(args[0], args[1]);
}
Compiler::value NumberSTD::int_div_eq_val_val(Compiler& c, std::vector<Compiler::value> args, int) {
	auto x = c.insn_load(args[0]);
	auto r = c.insn_int_div(x, args[1]);
	c.insn_store(args[0], r);
	return r;
}

Compiler::value NumberSTD::pow_mpz_mpz(Compiler& c, std::vector<Compiler::value> args, int) {
	auto ui = c.insn_call(c.env.long_, {args[1]}, "Number.mpz_get_ui");
	auto r = [&]() {
		if (args[0].t->temporary) return args[0];
		if (args[1].t->temporary) return args[1];
		return c.new_mpz();
	}();
	c.insn_call(c.env.void_, {r, args[0], ui}, "Number.mpz_pow_ui");
	if (args[1] != r) c.insn_delete_temporary(args[1]);
	return r;
}

Compiler::value NumberSTD::pow_mpz_int(Compiler& c, std::vector<Compiler::value> args, int) {
	// Check: mpz_log(a) * b <= 10000
	auto a_size = c.insn_call(c.env.integer, {args[0]}, "Number.mpz_log");
	auto r_size = c.insn_mul(a_size, args[1]);
	auto cond = c.insn_gt(r_size, c.new_integer(10000));
	auto label_then = c.insn_init_label("then");
	auto label_else = c.insn_init_label("else");
	auto label_end = c.insn_init_label("end");
	c.insn_if_new(cond, &label_then, &label_else);

	c.insn_label(&label_then);
	c.insn_delete_temporary(args[0]);
	c.insn_throw_object(vm::Exception::NUMBER_OVERFLOW);
	c.insn_branch(&label_end);

	c.insn_label(&label_else);
	c.insn_branch(&label_end);

	c.insn_label(&label_end);

	// Ops: size of the theorical result
	c.inc_ops_jit(r_size);

	auto r = args[0].t->temporary ? args[0] : c.new_mpz();
	c.insn_call(c.env.void_, {r, args[0], args[1]}, "Number.mpz_pow_ui");
	return r;
}

Compiler::value NumberSTD::lt(Compiler& c, std::vector<Compiler::value> args, int) {
	return c.insn_lt(args[0], args[1]);
}
Compiler::value NumberSTD::le(Compiler& c, std::vector<Compiler::value> args, int) {
	return c.insn_le(args[0], args[1]);
}
Compiler::value NumberSTD::gt(Compiler& c, std::vector<Compiler::value> args, int) {
	return c.insn_gt(args[0], args[1]);
}
Compiler::value NumberSTD::ge(Compiler& c, std::vector<Compiler::value> args, int) {
	return c.insn_ge(args[0], args[1]);
}

Compiler::value NumberSTD::mod(Compiler& c, std::vector<Compiler::value> args, int) {
	return c.insn_mod(args[0], args[1]);
}
Compiler::value NumberSTD::mod_mpz_mpz(Compiler& c, std::vector<Compiler::value> args, int) {
	auto r = [&]() {
		if (args[0].t->temporary) return args[0];
		if (args[1].t->temporary) return args[1];
		return c.new_mpz();
	}();
	c.insn_call(c.env.void_, {r, args[0], args[1]}, "Number.mpz_mod");
	if (args[1] != r) c.insn_delete_temporary(args[1]);
	return r;
}

Compiler::value NumberSTD::mod_eq_mpz_mpz(Compiler& c, std::vector<Compiler::value> args, int) {
	// auto a_addr = c.insn_address_of(args[0]);
	// auto b_addr = c.insn_address_of(args[1]);
	// c.insn_call(c.env.void_, {a_addr, a_addr, b_addr}, &mpz_mod);
	return c.insn_clone_mpz(args[0]);
}

Compiler::value NumberSTD::mod_eq_real(Compiler& c, std::vector<Compiler::value> args, int) {
	// std::cout << "mod " << args[0].t << " " << args[1].t << std::endl;
	auto x = c.insn_load(args[0]);
	auto sum = c.insn_mod(x, args[1]);
	c.insn_store(args[0], sum);
	return sum;
}

Compiler::value NumberSTD::double_mod(Compiler& c, std::vector<Compiler::value> args, int) {
	return c.insn_double_mod(args[0], args[1]);
}
Compiler::value NumberSTD::double_mod_eq(Compiler& c, std::vector<Compiler::value> args, int) {
	auto x = c.insn_load(args[0]);
	auto y = args[1];
	auto r = c.insn_mod(c.insn_add(c.insn_mod(x, y), y), y);
	c.insn_store(args[0], r);
	return r;
}

Compiler::value NumberSTD::bit_and(Compiler& c, std::vector<Compiler::value> args, int) {
	return c.insn_bit_and(args[0], args[1]);
}

Compiler::value NumberSTD::bit_and_eq(Compiler& c, std::vector<Compiler::value> args, int) {
	auto res = c.insn_bit_and(c.insn_load(args[0]), args[1]);
	c.insn_store(args[0], res);
	return res;
}

Compiler::value NumberSTD::bit_or(Compiler& c, std::vector<Compiler::value> args, int) {
	return c.insn_bit_or(args[0], args[1]);
}

Compiler::value NumberSTD::bit_or_eq(Compiler& c, std::vector<Compiler::value> args, int) {
	auto res = c.insn_bit_or(c.insn_load(args[0]), args[1]);
	c.insn_store(args[0], res);
	return res;
}

Compiler::value NumberSTD::bit_xor(Compiler& c, std::vector<Compiler::value> args, int) {
	return c.insn_bit_xor(args[0], args[1]);
}

Compiler::value NumberSTD::bit_xor_eq(Compiler& c, std::vector<Compiler::value> args, int) {
	auto res = c.insn_bit_xor(c.insn_load(args[0]), args[1]);
	c.insn_store(args[0], res);
	return res;
}

/*
 * Methods
 */

Compiler::value NumberSTD::_int(Compiler& c, std::vector<Compiler::value> args, int) {
	return c.to_int(args[0]);
}
Compiler::value NumberSTD::_long(Compiler& c, std::vector<Compiler::value> args, int) {
	return c.to_long(args[0]);
}

LSValue* NumberSTD::abs_ptr(LSValue* x) {
	return LSNumber::get(std::abs(((LSNumber*) x)->value));
}
Compiler::value NumberSTD::abs(Compiler& c, std::vector<Compiler::value> args, int) {
	auto r = c.insn_abs(args[0]);
	c.insn_delete_temporary(args[0]);
	return r;
}

double NumberSTD::acos_ptr(LSNumber* x) {
	double a = acos(x->value);
	LSValue::delete_temporary(x);
	return a;
}

Compiler::value NumberSTD::acos_real(Compiler& c, std::vector<Compiler::value> args, int) {
	return c.insn_acos(args[0]);
}

double NumberSTD::asin_ptr(LSNumber* x) {
	double a = asin(x->value);
	LSValue::delete_temporary(x);
	return a;
}

Compiler::value NumberSTD::asin_real(Compiler& c, std::vector<Compiler::value> args, int) {
	return c.insn_asin(args[0]);
}

double NumberSTD::atan_ptr(LSNumber* x) {
	double a = atan(x->value);
	LSValue::delete_temporary(x);
	return a;
}

Compiler::value NumberSTD::atan_real(Compiler& c, std::vector<Compiler::value> args, int) {
	return c.insn_atan(args[0]);
}

LSValue* NumberSTD::atan2_ptr_ptr(LSNumber* x, LSNumber* y) {
	auto r = LSNumber::get(std::atan2(x->value, y->value));
	LSValue::delete_temporary(x);
	LSValue::delete_temporary(y);
	return r;
}
Compiler::value NumberSTD::atan2(Compiler& c, std::vector<Compiler::value> args, int) {
	if (!args[0].t->is_polymorphic() and !args[1].t->is_polymorphic()) {
		return c.insn_atan2(args[0], args[1]);
	} else {
		auto r = c.insn_call(c.env.real, {c.to_real(args[0]), c.to_real(args[1])}, "Number.m_atan2.2");
		c.insn_delete_temporary(args[0]);
		c.insn_delete_temporary(args[1]);
		return r;
	}
}

LSValue* NumberSTD::char_ptr(LSNumber* x) {
	unsigned int n = x->value;
	LSValue::delete_temporary(x);
	char dest[5];
	u8_toutf8(dest, 5, &n, 1);
	return new LSString(dest);
}

LSValue* NumberSTD::char_real(double x) {
	unsigned int n = x;
	char dest[5];
	u8_toutf8(dest, 5, &n, 1);
	return new LSString(dest);
}

LSValue* NumberSTD::char_int(int x) {
	unsigned int n = x;
	char dest[5];
	u8_toutf8(dest, 5, &n, 1);
	return new LSString(dest);
}

double NumberSTD::exp_ptr(LSNumber* x) {
	double a = exp(x->value);
	LSValue::delete_temporary(x);
	return a;
}

Compiler::value NumberSTD::exp_real(Compiler& c, std::vector<Compiler::value> args, int) {
	return c.insn_exp(args[0]);
}

long NumberSTD::floor_ptr(LSNumber* x) {
	long a = floor(x->value);
	LSValue::delete_temporary(x);
	return a;
}

Compiler::value NumberSTD::floor_real(Compiler& c, std::vector<Compiler::value> args, int) {
	return c.insn_floor(args[0]);
}

int NumberSTD::round_ptr(LSNumber* x) {
	int a = round(x->value);
	LSValue::delete_temporary(x);
	return a;
}

Compiler::value NumberSTD::round_real(Compiler& c, std::vector<Compiler::value> args, int) {
	return c.insn_round(args[0]);
}

Compiler::value NumberSTD::round_int(Compiler&, std::vector<Compiler::value> args, int) {
	return args[0]; // Nothing to do
}

int NumberSTD::ceil_ptr(LSNumber* x) {
	int a = ceil(x->value);
	LSValue::delete_temporary(x);
	return a;
}

Compiler::value NumberSTD::ceil_real(Compiler& c, std::vector<Compiler::value> args, int) {
	return c.insn_ceil(args[0]);
}

Compiler::value NumberSTD::ceil_int(Compiler&, std::vector<Compiler::value> args, int) {
	return args[0]; // Nothing to do
}

LSValue* NumberSTD::max_ptr_ptr(LSNumber* x, LSNumber* y) {
	auto max = LSNumber::get(fmax(x->value, y->value));
	LSValue::delete_temporary(x);
	LSValue::delete_temporary(y);
	return max;
}
Compiler::value NumberSTD::max(Compiler& c, std::vector<Compiler::value> args, int) {
	auto a = c.to_numeric(args[0]);
	auto b = c.to_numeric(args[1]);
	auto t = a.t->operator * (b.t);
	a = c.insn_convert(a, t);
	b = c.insn_convert(b, t);
	return c.insn_max(a, b);
}

double NumberSTD::min_ptr_ptr(LSNumber* x, LSNumber* y) {
	double min = fmin(x->value, y->value);
	LSValue::delete_temporary(x);
	LSValue::delete_temporary(y);
	return min;
}
Compiler::value NumberSTD::min(Compiler& c, std::vector<Compiler::value> args, int) {
	auto a = c.to_numeric(args[0]);
	auto b = c.to_numeric(args[1]);
	auto t = a.t->operator * (b.t);
	a = c.insn_convert(a, t);
	b = c.insn_convert(b, t);
	return c.insn_min(a, b);
}

LSValue* NumberSTD::cos_ptr(LSNumber* x) {
	LSValue* r = LSNumber::get(cos(x->value));
	LSValue::delete_temporary(x);
	return r;
}
Compiler::value NumberSTD::cos_real(Compiler& c, std::vector<Compiler::value> args, int) {
	return c.insn_cos(args[0]);
}

double NumberSTD::sin_ptr(LSNumber* x) {
	double s = sin(x->value);
	LSValue::delete_temporary(x);
	return s;
}
Compiler::value NumberSTD::sin_real(Compiler& c, std::vector<Compiler::value> args, int) {
	return c.insn_sin(args[0]);
}

double NumberSTD::tan_ptr(LSNumber* x) {
	double c = tan(x->value);
	LSValue::delete_temporary(x);
	return c;
}
Compiler::value NumberSTD::tan_real(Compiler& c, std::vector<Compiler::value> args, int) {
	return c.insn_tan(args[0]);
}

double NumberSTD::sqrt_ptr(LSNumber* x) {
	double s = sqrt(x->value);
	LSValue::delete_temporary(x);
	return s;
}

Compiler::value NumberSTD::sqrt_mpz(Compiler& c, std::vector<Compiler::value> args, int) {
	auto r = args[0].t->temporary ? args[0] : c.new_mpz();
	c.insn_call(c.env.void_, {r, args[0]}, "Number.mpz_sqrt");
	return r;
}

LSValue* NumberSTD::cbrt_ptr(LSNumber* x) {
	LSValue* r = LSNumber::get(cbrt(x->value));
	LSValue::delete_temporary(x);
	return r;
}

Compiler::value NumberSTD::pow_int(Compiler& c, std::vector<Compiler::value> args, int) {
	return c.insn_pow(args[0], args[1]);
}

Compiler::value NumberSTD::pow_eq_mpz_mpz(Compiler& c, std::vector<Compiler::value> args, int) {
	c.insn_call(c.env.void_, {args[0], args[0], args[1]}, "Number.mpz_pow_ui");
	return c.insn_clone_mpz(args[0]);
}

Compiler::value NumberSTD::pow_eq_real(Compiler& c, std::vector<Compiler::value> args, int) {
	auto x = c.insn_load(args[0]);
	auto sum = c.insn_pow(x, args[1]);
	c.insn_store(args[0], sum);
	return sum;
}

Compiler::value NumberSTD::is_prime(Compiler& c, std::vector<Compiler::value> args, int) {
	auto reps = c.new_integer(15);
	auto res = c.insn_call(c.env.integer, {args[0], reps}, "Number.mpz_probab_prime_p");
	c.insn_delete_temporary(args[0]);
	return res;
}

template<typename T>
int NumberSTD::is_prime_number(T n) {
	if (n <= 1) return false;
	if (n <= 3) return true;
	if (n % 2 == 0 || n % 3 == 0) return false;
	for (T i = 5; i * i <= n; i = i + 6)
		if (n % i == 0 || n % (i + 2) == 0) return false;
	return true;
}

template<typename T>
bool NumberSTD::is_palindrome(T n) {
	T x = n, a, tmp = 0;
	while (n > 0) {
		a = n % 10;
		n = n / 10;
		tmp = tmp * 10 + a;
	}
	return tmp == x;
}

Compiler::value NumberSTD::hypot_ptr_ptr(Compiler& c, std::vector<Compiler::value> args, int) {
	auto r = c.insn_call(c.env.real, {c.to_real(args[0]), c.to_real(args[1])}, "Number.hypot.2");
	c.insn_delete_temporary(args[0]);
	c.insn_delete_temporary(args[1]);
	return r;
}

double NumberSTD::log_ptr(LSNumber* x) {
	auto res = log(x->value);
	LSValue::delete_temporary(x);
	return res;
}
Compiler::value NumberSTD::log_real(Compiler& c, std::vector<Compiler::value> args, int) {
	return c.insn_log(args[0]);
}

double NumberSTD::log10_ptr(LSNumber* x) {
	auto res = log10(x->value);
	LSValue::delete_temporary(x);
	return res;
}
Compiler::value NumberSTD::log10_real(Compiler& c, std::vector<Compiler::value> args, int) {
	return c.insn_log10(args[0]);
}

double NumberSTD::pow_ptr(LSNumber* x, LSNumber* y) {
	double r = pow(x->value, y->value);
	LSValue::delete_temporary(x);
	LSValue::delete_temporary(y);
	return r;
}

double NumberSTD::rand01() {
	return (double) rand() / RAND_MAX;
}

int NumberSTD::rand_int(int min, int max) {
	return min + floor(((double) rand() / RAND_MAX) * (max - min));
}
double NumberSTD::rand_real(double min, double max) {
	return min + ((double) rand() / RAND_MAX) * (max - min);
}

Compiler::value NumberSTD::signum(Compiler& c, std::vector<Compiler::value> args, int) {
	auto ap = c.insn_to_any(args[0]);
	auto r = c.insn_call(c.env.integer, {ap}, "Number.signum");
	c.insn_delete(ap);
	return r;
}
int NumberSTD::signum_ptr(LSNumber* x) {
	int s = 0;
	if (x->value > 0) s = 1;
	if (x->value < 0) s = -1;
	LSValue::delete_temporary(x);
	return s;
}

double NumberSTD::toDegrees_ptr(LSNumber* x) {
	double d = (x->value * 180) / M_PI;
	LSValue::delete_temporary(x);
	return d;
}
Compiler::value NumberSTD::toDegrees(Compiler& c, std::vector<Compiler::value> args, int) {
	return c.insn_call(c.env.real, {c.insn_to_any(args[0])}, "Number.toDegrees");
}
double NumberSTD::toRadians_ptr(LSNumber* x) {
	double r = (x->value * M_PI) / 180;
	LSValue::delete_temporary(x);
	return r;
}
Compiler::value NumberSTD::toRadians(Compiler& c, std::vector<Compiler::value> args, int) {
	return c.insn_call(c.env.real, {c.insn_to_any(args[0])}, "Number.toRadians");
}

LSValue* NumberSTD::isInteger_ptr(LSNumber* x) {
	auto is = LSBoolean::get(x->value == (int) x->value);
	LSValue::delete_temporary(x);
	return is;
}
Compiler::value NumberSTD::isInteger(Compiler& c, std::vector<Compiler::value> args, int) {
	auto type = args[0].t->fold();
	if (type->is_integer() or type->is_long()) {
		return c.new_bool(true);
	} else if (type->is_primitive()) {
		return c.insn_eq(c.to_int(args[0]), args[0]);
	} else {
		return c.insn_call(c.env.boolean, args, "Number.m_isint");
	}
}

Compiler::value NumberSTD::fold(Compiler& c, std::vector<Compiler::value> args, int) {
	return c.insn_call(c.env.any, {c.insn_to_any(args[0]), args[1], c.insn_to_any(args[2])}, "Number.fold");
}

LSValue* NumberSTD::int_to_string(int x) {
	return new LSString(std::to_string(x));
}
LSValue* NumberSTD::long_to_string(long x) {
	return new LSString(std::to_string(x));
}
LSValue* NumberSTD::real_to_string(double x) {
	return new LSString(LSNumber::print(x));
}
LSValue* NumberSTD::mpz_to_string(mpz_t x) {
	// TODO dynamic buffer size
	char buff[10000];
	mpz_get_str(buff, 10, x);
	return new LSString(buff);
}
bool NumberSTD::isint(LSNumber* x) {
	auto is = x->value == (int) x->value;
	LSValue::delete_temporary(x);
	return is;
}

#endif

}
