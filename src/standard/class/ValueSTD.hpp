#ifndef VALUE_STD_HPP
#define VALUE_STD_HPP

#include "../Module.hpp"

namespace ls {

class ValueSTD : public Module {
public:
	ValueSTD(VM* vm);

	/*
	 * Static fields
	 */
	static Compiler::value unknown(Compiler& c);

	/*
	 * Fields
	 */
	static Compiler::value attr_class(Compiler& c, Compiler::value a);

	/*
	 * Operators
	 */
	static Compiler::value op_store(Compiler& c, std::vector<Compiler::value> args, int);
	static Compiler::value op_instanceof(Compiler& c, std::vector<Compiler::value> args, bool);
	static Compiler::value op_equals(Compiler& c, std::vector<Compiler::value> args, bool);
	static Compiler::value op_triple_equals(Compiler& c, std::vector<Compiler::value> args, bool);
	static Compiler::value op_not_equals(Compiler& c, std::vector<Compiler::value> args, bool);
	static Compiler::value op_lt(Compiler& c, std::vector<Compiler::value> args, bool);
	static Compiler::value op_le(Compiler& c, std::vector<Compiler::value> args, bool);
	static Compiler::value op_gt(Compiler& c, std::vector<Compiler::value> args, bool);
	static Compiler::value op_ge(Compiler& c, std::vector<Compiler::value> args, bool);
	static Compiler::value op_and(Compiler&, std::vector<Compiler::value>, bool);
	static Compiler::value op_or(Compiler&, std::vector<Compiler::value>, bool);
	static Compiler::value op_xor(Compiler&, std::vector<Compiler::value>, bool);
	static Compiler::value op_add(Compiler&, std::vector<Compiler::value>, bool);
	static Compiler::value op_sub(Compiler&, std::vector<Compiler::value>, bool);
	static Compiler::value op_mul(Compiler&, std::vector<Compiler::value>, bool);
	static Compiler::value op_div(Compiler&, std::vector<Compiler::value>, bool);
	static Compiler::value op_pow(Compiler&, std::vector<Compiler::value>, bool);
	static Compiler::value op_int_div(Compiler& c, std::vector<Compiler::value> args, bool);
	static int ls_bit_and(LSValue*, LSValue*);
	static int ls_bit_and_eq(LSValue**, LSValue*);
	static int ls_bit_or(LSValue*, LSValue*);
	static int ls_bit_or_eq(LSValue**, LSValue*);
	static int ls_bit_xor(LSValue*, LSValue*);
	static int ls_bit_xor_eq(LSValue**, LSValue*);
	static Compiler::value bit_shift_left(Compiler& c, std::vector<Compiler::value> args, bool);
	static Compiler::value bit_shift_left_eq(Compiler& c, std::vector<Compiler::value> args, bool);
	static int ls_bit_shift_left_eq(LSValue** x, LSValue* y);
	static Compiler::value bit_shift_right(Compiler& c, std::vector<Compiler::value> args, bool);
	static Compiler::value bit_shift_right_eq(Compiler& c, std::vector<Compiler::value> args, bool);
	static int ls_bit_shift_right_eq(LSValue** x, LSValue* y);
	static Compiler::value bit_shift_uright(Compiler& c, std::vector<Compiler::value> args, bool);
	static Compiler::value bit_shift_uright_eq(Compiler& c, std::vector<Compiler::value> args, bool);
	static int ls_bit_shift_uright_eq(LSValue**, LSValue*);
	static Compiler::value op_in(Compiler&, std::vector<Compiler::value>, bool);
	static Compiler::value op_swap_val(Compiler&, std::vector<Compiler::value>, bool);
	static LSValue* op_swap_ptr(LSValue** x, LSValue** y);
	static Compiler::value op_call(Compiler&, std::vector<Compiler::value>, bool);

	/*
	 * Methods
	 */
	static Compiler::value copy(Compiler& c, std::vector<Compiler::value> args, bool);
	static Compiler::value to_string(Compiler& c, std::vector<Compiler::value> args, bool);

	// Hidden functions
	static Compiler::value typeID(Compiler& c, std::vector<Compiler::value> args, bool);
	static int absolute(LSValue* v);
	static LSValue* clone(LSValue* v);
	static LSValue* attr(LSValue* v, char* field);
	static LSValue** attrL(LSValue* v, char* field);
	static bool ls_not(LSValue* x);
	static LSValue* ls_minus(LSValue* x);
	static LSValue* ls_inc(LSValue* x);
	static LSValue* ls_pre_inc(LSValue* x);
	static LSValue* ls_incl(LSValue** x);
	static LSValue* ls_pre_incl(LSValue** x);
	static LSValue* ls_dec(LSValue* x);
	static LSValue* ls_pre_dec(LSValue* x);
	static LSValue* ls_decl(LSValue** x);
	static LSValue* ls_pre_decl(LSValue** x);
	static LSValue* ls_pre_tilde(LSValue* v);
	static LSValue* ls_add(LSValue*, LSValue*);
	static LSValue* ls_add_eq(LSValue**, LSValue*);
	static LSValue* ls_sub(LSValue*, LSValue*);
	static LSValue* ls_sub_eq(LSValue**, LSValue*);
	static LSValue* ls_mul(LSValue*, LSValue*);
	static LSValue* ls_mul_eq(LSValue**, LSValue*);
	static LSValue* ls_div(LSValue*, LSValue*);
	static LSValue* ls_div_eq(LSValue**, LSValue*);
	static LSValue* ls_int_div(LSValue*, LSValue*);
	static LSValue* ls_int_div_eq(LSValue**, LSValue*);
	static LSValue* ls_mod(LSValue*, LSValue*);
	static LSValue* ls_mod_eq(LSValue**, LSValue*);
	static LSValue* ls_double_mod(LSValue*, LSValue*);
	static LSValue* ls_double_mod_eq(LSValue**, LSValue*);
	static LSValue* ls_pow(LSValue*, LSValue*);
	static LSValue* ls_pow_eq(LSValue**, LSValue*);
	static int integer(const LSValue* x);
	static double real(const LSValue* x);
	static double real_delete(const LSValue* x);
	static long long_(const LSValue* x);
	static LSValue* range(LSValue* a, int start, int end);
	static LSValue* at(LSValue* a, LSValue* k);
	static LSValue** atl(LSValue* a, LSValue* k);
	static bool in_i(LSValue* x, int k);
	static bool in(LSValue* x, LSValue* y);
	static int type(LSValue* x);
	static bool is_null(LSValue* x);
	static bool to_bool(LSValue* x);
	static bool eq(LSValue* x, LSValue* y);
	static bool triple_eq(LSValue* x, LSValue* y);
	static bool lt(LSValue* x, LSValue* y);
	static bool le(LSValue* x, LSValue* y);
	static bool gt(LSValue* x, LSValue* y);
	static bool ge(LSValue* x, LSValue* y);
	static void delete_previous(LSValue* x);
	static int get_int(LSNumber* x);
	static LSValue* get_class(LSValue* x);
	static void export_context_variable(char* name, LSValue* v);
	static void export_context_variable_int(char* name, int v);
	static void export_context_variable_long(char* name, long v);
	static void export_context_variable_real(char* name, double v);
};

}

#endif
