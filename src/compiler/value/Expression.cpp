#include "../../compiler/value/Expression.hpp"

#include "../../compiler/value/ArrayAccess.hpp"
#include "../../compiler/value/ObjectAccess.hpp"
#include "../../compiler/value/Function.hpp"
#include "../../compiler/value/LeftValue.hpp"
#include "../../compiler/value/Number.hpp"
#include "../../compiler/value/VariableValue.hpp"
#include "../../vm/LSValue.hpp"
#include "../../vm/value/LSBoolean.hpp"
#include "../../vm/value/LSArray.hpp"

using namespace std;

namespace ls {

Expression::Expression() : Expression(nullptr) {}

Expression::Expression(Value* v) :
	v1(v), v2(nullptr), op(nullptr), ignorev2(false), no_op(false), operations(0) {
	type = Type::VALUE;
	operations = 1;
}

Expression::~Expression() {
	if (v1 != nullptr) {
		delete v1;
	}
	if (op != nullptr) {
		delete op;
	}
	if (v2 != nullptr) {
		delete v2;
	}
}

void Expression::append(Operator* op, Value* exp, bool ref_v2) {

	/*
	 * Single expression (2, 'hello', ...), just add the operator
	 */
	if (this->op == nullptr) {
		this->op = op;
		v2 = exp;
		reference_v2 = ref_v2;
		return;
	}

	if (!parenthesis and (op->priority < this->op->priority
		   or (op->priority == this->op->priority and op->character == "="))) {
		/*
		 * We already have '5 + 2' for example,
		 * and try to add a operator with a higher priority,
		 * such as : '× 7' => '5 + (2 × 7)'
		 */
		Expression* ex = new Expression();
		ex->v1 = v2;
		ex->op = op;
		ex->reference_v2 = ref_v2;
		ex->v2 = exp;
		this->v2 = ex;

	} else {
		/*
		 * We already have '5 + 2' for example,
		 * and try to add a operator with a lower priority,
		 * such as : '< 7' => '(5 + 2) < 7'
		 */
		Expression* newV1 = new Expression();
		newV1->v1 = this->v1;
		newV1->op = this->op;
		newV1->v2 = this->v2;
		this->v1 = newV1;
		this->op = op;
		this->reference_v2 = ref_v2;
		v2 = exp;
	}
}

void Expression::print(std::ostream& os, int indent, bool debug) const {

	if (parenthesis or debug) {
		os << "(";
	}

	if (v1 != nullptr) {

		v1->print(os, indent, debug);

		if (op != nullptr) {
			os << " ";
			op->print(os);
			os << " ";
			if (reference_v2) os << "@";
			v2->print(os, indent, debug);
		}
	}
	if (parenthesis or debug) {
		os << ")";
	}
	if (debug) {
		os << " " << type;
	}
}

unsigned Expression::line() const {
	return 0;
}

void Expression::analyse(SemanticAnalyser* analyser, const Type& req_type) {

	// No operator : just analyse v1 and return
	if (op == nullptr) {
		v1->analyse(analyser, req_type);
		type = v1->type;
		return;
	}

	// Require float type for a division
	Type v1_type = Type::UNKNOWN;
	Type v2_type = Type::UNKNOWN;

	if (op->type == TokenType::DIVIDE) {
		type.raw_type = RawType::FLOAT;
		v1_type = Type::FLOAT;
		v2_type = Type::FLOAT;
	}

	// First analyse of v1 and v2
	v1->analyse(analyser, v1_type);
	if (v1->type.nature == Nature::POINTER) {
		type.nature = Nature::POINTER;
	}
	v2->analyse(analyser, v2_type);
	if (v2->type.nature == Nature::POINTER) {
		type.nature = Nature::POINTER;
	}
	constant = v1->constant and v2->constant;

	// Array += element
	if (op->type == TokenType::PLUS_EQUAL && v1->type.raw_type == RawType::ARRAY) {
		VariableValue* vv = dynamic_cast<VariableValue*>(v1);
		if (vv->type.raw_type == RawType::ARRAY) {
			vv->var->will_take_element(analyser, v2->type);
		}
	}

	// A = B, A += B, etc. mix types
	if (op->type == TokenType::EQUAL or op->type == TokenType::XOR
		or op->type == TokenType::PLUS or op->type == TokenType::PLUS_EQUAL
		or op->type == TokenType::TIMES or op->type == TokenType::TIMES_EQUAL
		or op->type == TokenType::DIVIDE or op->type == TokenType::DIVIDE_EQUAL
		or op->type == TokenType::MINUS or op->type == TokenType::MINUS_EQUAL
		or op->type == TokenType::POWER or op->type == TokenType::POWER_EQUAL
		or op->type == TokenType::MODULO or op->type == TokenType::MODULO_EQUAL
		or op->type == TokenType::LOWER or op->type == TokenType::LOWER_EQUALS
		or op->type == TokenType::GREATER or op->type == TokenType::GREATER_EQUALS
		or op->type == TokenType::SWAP) {

		type = v1->type.mix(v2->type);

		// Set the correct type nature for the two members
		if (v2->type.nature == Nature::POINTER and v1->type.nature != Nature::POINTER) {
			v1->analyse(analyser, Type::POINTER);
			type.nature = Nature::POINTER;
		}
		if (v1->type.nature == Nature::POINTER and v2->type.nature != Nature::POINTER) {
			v2->analyse(analyser, Type::POINTER);
			type.nature = Nature::POINTER;
		}

		// String / String => Array<String>
		if (op->type == TokenType::DIVIDE and v1->type == Type::STRING and v2->type == Type::STRING) {
			type = Type::STRING_ARRAY;
		}
	}

	// Boolean operators : result is a boolean
	if (op->type == TokenType::AND or op->type == TokenType::OR or op->type == TokenType::XOR or
		op->type == TokenType::GREATER or op->type == TokenType::DOUBLE_EQUAL or
		op->type == TokenType::LOWER or op->type == TokenType::LOWER_EQUALS or
		op->type == TokenType::GREATER_EQUALS or op->type == TokenType::TRIPLE_EQUAL or
		op->type == TokenType::DIFFERENT or op->type == TokenType::TRIPLE_DIFFERENT) {

		type = Type::BOOLEAN;
	}

	// Bitwise operators : result is a integer
	if (op->type == TokenType::BIT_AND or op->type == TokenType::PIPE
		or op->type == TokenType::BIT_XOR or op->type == TokenType::BIT_AND_EQUALS
		or op->type == TokenType::BIT_OR_EQUALS or op->type == TokenType::BIT_XOR_EQUALS
		or op->type == TokenType::BIT_SHIFT_LEFT or op->type == TokenType::BIT_SHIFT_LEFT_EQUALS
		or op->type == TokenType::BIT_SHIFT_RIGHT or op->type == TokenType::BIT_SHIFT_RIGHT_EQUALS
		or op->type == TokenType::BIT_SHIFT_RIGHT_UNSIGNED or op->type == TokenType::BIT_SHIFT_RIGHT_UNSIGNED_EQUALS) {

		type = Type::INTEGER;
	}

	// A = B, A += B, etc. A must be a l-value
	if (op->type == TokenType::EQUAL or op->type == TokenType::PLUS_EQUAL
		or op->type == TokenType::MINUS_EQUAL or op->type == TokenType::TIMES_EQUAL
		or op->type == TokenType::DIVIDE_EQUAL or op->type == TokenType::MODULO_EQUAL
		or op->type == TokenType::POWER_EQUAL) {
		// TODO other operators like |= ^= &=

		// Check if A is a l-value
		bool is_left_value = true;
		if (dynamic_cast<LeftValue*>(v1) == nullptr) {
			std::string c = "<v>";
			analyser->add_error({SemanticException::Type::VALUE_MUST_BE_A_LVALUE, v1->line(), c});
			is_left_value = false;
		}

		// A += B, A -= B
		if (is_left_value and (op->type == TokenType::PLUS_EQUAL or op->type == TokenType::MINUS_EQUAL)) {
			if (v1->type == Type::INTEGER and v2->type == Type::FLOAT) {
				((LeftValue*) v1)->change_type(analyser, Type::FLOAT);
			}
		}
	}

	// [1, 2, 3] ~~ x -> x ^ 2
	if (op->type == TokenType::TILDE_TILDE) {
		v2->will_take(analyser, { v1->type.getElementType() });
		type = Type::ARRAY;
		type.setElementType(v2->type.getReturnType());
	}

	// object ?? default
	if (op->type == TokenType::DOUBLE_QUESTION_MARK) {
		type = Type::POINTER;
		if (v1->type == v2->type) type = v1->type;
	}

	if (op->type == TokenType::INSTANCEOF) {
		v1->analyse(analyser, Type::POINTER);
	}
	if (op->type == TokenType::IN) {
		v1->analyse(analyser, Type::POINTER);
	}

	// Merge operations count
	// (2 + 3) × 4    ->  2 ops for the × directly
	if (op->type != TokenType::OR or op->type == TokenType::AND) {
		if (Expression* ex1 = dynamic_cast<Expression*>(v1)) {
			operations += ex1->operations;
			ex1->operations = 0;
		}
		if (Expression* ex2 = dynamic_cast<Expression*>(v2)) {
			operations += ex2->operations;
			ex2->operations = 0;
		}
	}

	// At the end the require nature is taken into account
	if (req_type.nature != Nature::UNKNOWN) {
		type.nature = req_type.nature;
	}
}


LSValue* jit_add(LSValue* x, LSValue* y) {
	return y->operator + (x);
}
LSValue* jit_int_array_add(LSArray<int>* x, LSArray<int>* y) {
	return x->operator + (y);
}
LSValue* jit_sub(LSValue* x, LSValue* y) {
	return y->operator - (x);
}
LSValue* jit_mul(LSValue* x, LSValue* y) {
	return y->operator * (x);
}
LSValue* jit_div(LSValue* x, LSValue* y) {
	return y->operator / (x);
}
LSValue* jit_pow(LSValue* x, LSValue* y) {
	return y->poww(x);
}
LSValue* jit_mod(LSValue* x, LSValue* y) {
	return y->operator % (x);
}
LSValue* jit_and(LSValue* x, LSValue* y) {
	return LSBoolean::get(x->isTrue() and y->isTrue());
}
LSValue* jit_or(LSValue* x, LSValue* y) {
	return LSBoolean::get(x->isTrue() or y->isTrue());
}
LSValue* jit_xor(LSValue* x, LSValue* y) {
	return LSBoolean::get(x->isTrue() xor y->isTrue());
}
LSValue* jit_inc(LSValue* x) {
	return x->operator ++ (1);
}
LSValue* jit_dec(LSValue* x) {
	return x->operator -- (1);
}

LSValue* jit_equals(LSValue* x, LSValue* y) {
	return LSBoolean::get(x->operator == (y));
}
LSValue* jit_not_equals(LSValue* x, LSValue* y) {
	return LSBoolean::get(x->operator != (y));
}
LSValue* jit_lt(LSValue* x, LSValue* y) {

	return LSBoolean::get(y->operator < (x));
}
LSValue* jit_le(LSValue* x, LSValue* y) {
	return LSBoolean::get(y->operator <= (x));
}
LSValue* jit_gt(LSValue* x, LSValue* y) {
	return LSBoolean::get(y->operator > (x));
}
LSValue* jit_ge(LSValue* x, LSValue* y) {
	return LSBoolean::get(y->operator >= (x));
}

LSValue* jit_store(LSValue** x, LSValue* y) {
	y->refs++;
	LSValue* r = *x = y;
	LSValue::delete_ref(y);
	return r;
}

int jit_store_value(int* x, int y) {
	return *x = y;
}

LSValue* jit_swap(LSValue** x, LSValue** y) {
	LSValue* tmp = *x;
	*x = *y;
	*y = tmp;
	return *x;
}

LSValue* jit_add_equal(LSValue* x, LSValue* y) {
	return y->operator += (x);
}
int jit_add_equal_value(int* x, int y) {
	return *x += y;
}
LSValue* jit_sub_equal(LSValue* x, LSValue* y) {
	return y->operator -= (x);
}
LSValue* jit_mul_equal(LSValue* x, LSValue* y) {
	return y->operator *= (x);
}
LSValue* jit_div_equal(LSValue* x, LSValue* y) {
	return y->operator /= (x);
}
LSValue* jit_mod_equal(LSValue* x, LSValue* y) {
	return y->operator %= (x);
}
LSValue* jit_pow_equal(LSValue* x, LSValue* y) {
	return y->pow_eq(x);
}
int jit_array_add_value(LSArray<int>* x, int v) {
	x->push_clone(v);
	return v;
}

LSValue* jit_in(LSValue* x, LSValue* y) {
	return LSBoolean::get(y->in(x));
}

LSValue* jit_instanceof(LSValue* x, LSValue* y) {
	return LSBoolean::get(((LSClass*) x->getClass())->name == ((LSClass*) y)->name);
}

LSValue* jit_bit_and(LSValue*, LSValue*) {
	return LSNull::get();
}
LSValue* jit_bit_and_equal(LSValue*, LSValue*) {
	return LSNull::get();
}
LSValue* jit_bit_or(LSValue*, LSValue*) {
	return LSNull::get();
}
LSValue* jit_bit_or_equal(LSValue*, LSValue*) {
	return LSNull::get();
}
LSValue* jit_bit_xor(LSValue*, LSValue*) {
	return LSNull::get();
}
LSValue* jit_bit_xor_equal(LSValue*, LSValue*) {
	return LSNull::get();
}
LSValue* jit_bit_shl(LSValue*, LSValue*) {
	return LSNull::get();
}
LSValue* jit_bit_shl_equal(LSValue*, LSValue*) {
	return LSNull::get();
}
LSValue* jit_bit_shr(LSValue*, LSValue*) {
	return LSNull::get();
}
LSValue* jit_bit_shr_equal(LSValue*, LSValue*) {
	return LSNull::get();
}
LSValue* jit_bit_shr_unsigned(LSValue*, LSValue*) {
	return LSNull::get();
}
LSValue* jit_bit_shr_unsigned_equal(LSValue*, LSValue*) {
	return LSNull::get();
}
bool jit_is_null(LSValue* v) {
	return v->typeID() == 1;
}

jit_value_t Expression::compile(Compiler& c) const {

	// No operator : compile v1 and return
	if (op == nullptr) {
		return v1->compile(c);
	}

	// Increment operations
	if (operations > 0) {
		VM::inc_ops(c.F, operations);
	}

//	cout << "v1 : " << v1->type << ", v2 : " << v2->type << endl;

	jit_value_t (*jit_func)(jit_function_t, jit_value_t, jit_value_t) = nullptr;
	void* ls_func;
	bool use_jit_func = v1->type.nature == Nature::VALUE and v2->type.nature == Nature::VALUE;
	vector<jit_value_t> args;
	Type conv_info = Type::UNKNOWN;
	Type v1_conv = Type::POINTER;
	Type v2_conv = Type::POINTER;

	switch (op->type) {
		case TokenType::EQUAL: {

			if (v1->type.nature == Nature::VALUE and v2->type.nature == Nature::VALUE) {

				if (ArrayAccess* l1 = dynamic_cast<ArrayAccess*>(v1)) {

					args.push_back(l1->compile_l(c));
					args.push_back(v2->compile(c));

					jit_type_t args_types[2] = {LS_POINTER, LS_POINTER};
					jit_type_t sig = jit_type_create_signature(jit_abi_cdecl, LS_INTEGER, args_types, 2, 0);
					jit_value_t v = jit_insn_call_native(c.F, "", (void*) jit_store_value, sig, args.data(), 2, JIT_CALL_NOTHROW);

					if (type.nature == Nature::POINTER) {
						return VM::value_to_pointer(c.F, v, type);
					}
					return v;
				} else {

					jit_value_t x = v1->compile(c);
					jit_value_t y = v2->compile(c);
					jit_insn_store(c.F, x, y);
					if (v2->type.nature != Nature::POINTER and type.nature == Nature::POINTER) {
						return VM::value_to_pointer(c.F, y, type);
					}
					return y;
				}
			} else if (v1->type.nature == Nature::POINTER) {

				if (dynamic_cast<ArrayAccess*>(v1)) {

					args.push_back(((LeftValue*) v1)->compile_l(c));
					args.push_back(v2->compile(c));

					if (v1->type.must_manage_memory()) {
						if (reference_v2) {
							VM::inc_refs(c.F, args[1]);
						} else {
							args[1] = VM::move_inc_obj(c.F, args[1]);
						}
					}

					ls_func = (void*) &jit_store;

				} else if (dynamic_cast<VariableValue*>(v1)) {

					jit_value_t x = v1->compile(c);
					jit_value_t y = v2->compile(c);

					if (v2->type.must_manage_memory()) {
						if (reference_v2) {
							VM::inc_refs(c.F, y);
						} else {
							y = VM::move_inc_obj(c.F, y);
						}
					}
					if (v1->type.must_manage_memory()) {
						VM::delete_ref(c.F, x);
					}
					jit_insn_store(c.F, x, y);
					return y;

				} else {
					args.push_back(((LeftValue*) v1)->compile_l(c));
					args.push_back(v2->compile(c));
					ls_func = (void*) &jit_store;
				}
			} else {
				cout << "type : " << type << endl;
				cout << "v1: " << v1->type << endl;
				cout << "v2: " << v2->type << endl;
				throw new runtime_error("value = pointer !");
			}
			break;
		}
		case TokenType::SWAP: {
			if (v1->type.nature == Nature::VALUE and v2->type.nature == Nature::VALUE) {
				jit_value_t x = v1->compile(c);
				jit_value_t y = v2->compile(c);
				jit_value_t t = jit_insn_load(c.F, x);
				jit_insn_store(c.F, x, y);
				jit_insn_store(c.F, y, t);
				if (v2->type.nature != Nature::POINTER and type.nature == Nature::POINTER) {
					return VM::value_to_pointer(c.F, x, type);
				}
				return x;
			} else {
				args.push_back(((LeftValue*) v1)->compile_l(c));
				args.push_back(((LeftValue*) v2)->compile_l(c));
				ls_func = (void*) &jit_swap;
			}
			break;
		}
		case TokenType::PLUS_EQUAL: {

			if (v1->type.raw_type == RawType::ARRAY) {
//				cout << "Array add " << endl;
				ls_func = (void*) jit_array_add_value;
				break;
			}

			if (ArrayAccess* l1 = dynamic_cast<ArrayAccess*>(v1)) {

				args.push_back(l1->compile_l(c));
				args.push_back(v2->compile(c));

				jit_type_t args_types[2] = {LS_POINTER, LS_POINTER};
				jit_type_t sig = jit_type_create_signature(jit_abi_cdecl, LS_INTEGER, args_types, 2, 0);
				jit_value_t v = jit_insn_call_native(c.F, "", (void*) jit_add_equal_value, sig, args.data(), 2, JIT_CALL_NOTHROW);

				if (type.nature == Nature::POINTER) {
					return VM::value_to_pointer(c.F, v, type);
				}
				return v;
			}

			if (v1->type.nature == Nature::VALUE and v2->type.nature == Nature::VALUE) {
				jit_value_t x = v1->compile(c);
				jit_value_t y = v2->compile(c);
				jit_value_t sum = jit_insn_add(c.F, x, y);
				jit_insn_store(c.F, x, sum);
				if (v2->type.nature != Nature::POINTER and type.nature == Nature::POINTER) {
					return VM::value_to_pointer(c.F, sum, type);
				}
				return sum;
			} else {
				ls_func = (void*) &jit_add_equal;
			}
			break;
		}
		case TokenType::MINUS_EQUAL: {

			if (v1->type.nature == Nature::VALUE and v2->type.nature == Nature::VALUE) {
				jit_value_t x = v1->compile(c);
				jit_value_t y = v2->compile(c);
				jit_value_t sum = jit_insn_sub(c.F, x, y);
				jit_insn_store(c.F, x, sum);
				if (v2->type.nature != Nature::POINTER and type.nature == Nature::POINTER) {
					return VM::value_to_pointer(c.F, sum, type);
				}
				return sum;
			} else {
				ls_func = (void*) &jit_sub_equal;
			}
			break;
		}
		case TokenType::TIMES_EQUAL: {

			if (v1->type.nature == Nature::VALUE and v2->type.nature == Nature::VALUE) {

				jit_value_t x = v1->compile(c);
				jit_value_t y = v2->compile(c);
				jit_value_t sum = jit_insn_mul(c.F, x, y);
				jit_insn_store(c.F, x, sum);
				if (v2->type.nature != Nature::POINTER and type.nature == Nature::POINTER) {
					return VM::value_to_pointer(c.F, sum, type);
				}
				return sum;
			} else {
				ls_func = (void*) &jit_mul_equal;
			}
			break;
		}
		case TokenType::DIVIDE_EQUAL: {

			if (v1->type.nature == Nature::VALUE and v2->type.nature == Nature::VALUE) {
				jit_value_t x = v1->compile(c);
				jit_value_t y = v2->compile(c);
				jit_value_t xf = jit_value_create(c.F, LS_REAL);
				jit_insn_store(c.F, xf, x);
				jit_value_t sum = jit_insn_div(c.F, xf, y);
				jit_insn_store(c.F, x, sum);
				if (v2->type.nature != Nature::POINTER and type.nature == Nature::POINTER) {
					return VM::value_to_pointer(c.F, sum, type);
				}
				return sum;
			} else {
				ls_func = (void*) &jit_div_equal;
			}
			break;
		}
		case TokenType::MODULO_EQUAL: {

			if (v1->type.nature == Nature::VALUE and v2->type.nature == Nature::VALUE) {
				jit_value_t x = v1->compile(c);
				jit_value_t y = v2->compile(c);
				jit_value_t sum = jit_insn_rem(c.F, x, y);
				jit_insn_store(c.F, x, sum);
				if (v2->type.nature != Nature::POINTER and type.nature == Nature::POINTER) {
					return VM::value_to_pointer(c.F, sum, type);
				}
				return sum;
			} else {
				ls_func = (void*) &jit_mod_equal;
			}
			break;
		}
		case TokenType::POWER_EQUAL: {

			if (v1->type.nature == Nature::VALUE and v2->type.nature == Nature::VALUE) {
				jit_value_t x = v1->compile(c);
				jit_value_t y = v2->compile(c);
				jit_value_t sum = jit_insn_pow(c.F, x, y);
				jit_insn_store(c.F, x, sum);
				if (v2->type.nature != Nature::POINTER and type.nature == Nature::POINTER) {
					return VM::value_to_pointer(c.F, sum, type);
				}
				return sum;
			} else {
				ls_func = (void*) &jit_pow_equal;
			}
			break;
		}
		case TokenType::AND: {
			jit_func = &jit_insn_and;
			ls_func = (void*) &jit_and;
			conv_info = Type::BOOLEAN;
			break;
		}
		case TokenType::OR: {
			jit_func = &jit_insn_or;
			ls_func = (void*) &jit_or;
			conv_info = Type::BOOLEAN;
			break;
		}
		case TokenType::XOR: {
			if (use_jit_func) {
				jit_value_t x = v1->compile(c);
				jit_value_t y = v2->compile(c);
				x = jit_insn_to_not_bool(c.F, x);
				y = jit_insn_to_not_bool(c.F, y);
				jit_value_t r = jit_insn_or(c.F,
					jit_insn_and(c.F, x, jit_insn_not(c.F, y)),
					jit_insn_and(c.F, y, jit_insn_not(c.F, x))
				);
				if (type.nature == Nature::POINTER) {
					return VM::value_to_pointer(c.F, r, Type::BOOLEAN);
				}
				return r;
			} else {
				ls_func = (void*) &jit_xor;
				conv_info = Type::BOOLEAN;
			}
			break;
		}
		case TokenType::PLUS: {
			jit_func = &jit_insn_add;
			if (v1->type == Type::INT_ARRAY and v2->type == Type::INT_ARRAY) {
				ls_func = (void*) &jit_int_array_add;
			} else {
				ls_func = (void*) &jit_add;
			}
			break;
		}
		case TokenType::MINUS: {
			jit_func = &jit_insn_sub;
			ls_func = (void*) &jit_sub;
			break;
		}
		case TokenType::TIMES: {
			jit_func = &jit_insn_mul;
			ls_func = (void*) &jit_mul;
			break;
		}
		case TokenType::DIVIDE: {
			jit_func = &jit_insn_div;
			ls_func = (void*) &jit_div;
			conv_info = Type::FLOAT;
			break;
		}
		case TokenType::MODULO: {
			jit_func = &jit_insn_rem;
			ls_func = (void*) &jit_mod;
			break;
		}
		case TokenType::POWER: {
			jit_func = &jit_insn_pow;
			ls_func = (void*) &jit_pow;
			break;
		}
		case TokenType::DOUBLE_EQUAL: {
			jit_func = &jit_insn_eq;
			ls_func = (void*) &jit_equals;
			conv_info = Type::BOOLEAN;
			break;
		}
		case TokenType::DIFFERENT: {
			jit_func = &jit_insn_ne;
			ls_func = (void*) &jit_not_equals;
			conv_info = Type::BOOLEAN;
			break;
		}
		case TokenType::LOWER: {
			jit_func = &jit_insn_lt;
			ls_func = (void*) &jit_lt;
			conv_info = Type::BOOLEAN;
			break;
		}
		case TokenType::LOWER_EQUALS: {
			jit_func = &jit_insn_le;
			ls_func = (void*) &jit_le;
			conv_info = Type::BOOLEAN;
			break;
		}
		case TokenType::GREATER: {
			jit_func = &jit_insn_gt;
			ls_func = (void*) &jit_gt;
			conv_info = Type::BOOLEAN;
			break;
		}
		case TokenType::GREATER_EQUALS: {
			jit_func = &jit_insn_ge;
			ls_func = (void*) &jit_ge;
			conv_info = Type::BOOLEAN;
			break;
		}
		case TokenType::TILDE_TILDE: {
			if (v1->type.getElementType() == Type::INTEGER) {
				if (type.getElementType() == Type::INTEGER) {
					ls_func = (void*) &LSArray<int>::ls_map_int;
				} else if (type.getElementType() == Type::FLOAT) {
					ls_func = (void*) &LSArray<int>::ls_map_real;
				} else {
					ls_func = (void*) &LSArray<int>::ls_map;
				}
			} else if (v1->type.getElementType() == Type::FLOAT) {
				if (type.getElementType() == Type::FLOAT) {
					ls_func = (void*) &LSArray<double>::ls_map_real;
				} else if (type.getElementType() == Type::INTEGER) {
					ls_func = (void*) &LSArray<double>::ls_map_int;
				} else {
					ls_func = (void*) &LSArray<double>::ls_map;
				}
			} else {
				ls_func = (void*) &LSArray<LSValue*>::ls_map;
			}
			break;
		}
		case TokenType::IN: {
			use_jit_func = false;
			ls_func = (void*) &jit_in;
			break;
		}
		case TokenType::INSTANCEOF: {
			use_jit_func = false;
			ls_func = (void*) &jit_instanceof;
			break;
		}
		case TokenType::BIT_AND: {
			jit_func = &jit_insn_and;
			ls_func = (void*) &jit_bit_and;
			break;
		}
		case TokenType::BIT_AND_EQUALS: {
			if (v1->type.nature == Nature::VALUE and v2->type.nature == Nature::VALUE) {
				jit_value_t x = v1->compile(c);
				jit_value_t y = v2->compile(c);
				jit_value_t a = jit_insn_and(c.F, x, y);
				jit_insn_store(c.F, x, a);
				if (v2->type.nature != Nature::POINTER and type.nature == Nature::POINTER) {
					return VM::value_to_pointer(c.F, a, type);
				}
				return a;
			} else {
				ls_func = (void*) &jit_bit_and_equal;
			}
			break;
		}
		case TokenType::PIPE: {
			jit_func = &jit_insn_or;
			ls_func = (void*) &jit_bit_or;
			break;
		}
		case TokenType::BIT_OR_EQUALS: {
			if (v1->type.nature == Nature::VALUE and v2->type.nature == Nature::VALUE) {
				jit_value_t x = v1->compile(c);
				jit_value_t y = v2->compile(c);
				jit_value_t o = jit_insn_or(c.F, x, y);
				jit_insn_store(c.F, x, o);
				if (v2->type.nature != Nature::POINTER and type.nature == Nature::POINTER) {
					return VM::value_to_pointer(c.F, o, type);
				}
				return o;
			} else {
				ls_func = (void*) &jit_bit_or_equal;
			}
			break;
		}
		case TokenType::BIT_XOR: {
			jit_func = &jit_insn_xor;
			ls_func = (void*) &jit_bit_xor;
			break;
		}
		case TokenType::BIT_XOR_EQUALS: {
			if (v1->type.nature == Nature::VALUE and v2->type.nature == Nature::VALUE) {
				jit_value_t x = v1->compile(c);
				jit_value_t y = v2->compile(c);
				jit_value_t a = jit_insn_xor(c.F, x, y);
				jit_insn_store(c.F, x, a);
				if (v2->type.nature != Nature::POINTER and type.nature == Nature::POINTER) {
					return VM::value_to_pointer(c.F, a, type);
				}
				return a;
			} else {
				ls_func = (void*) &jit_bit_xor_equal;
			}
			break;
		}
		case TokenType::BIT_SHIFT_LEFT : {
			jit_func = &jit_insn_shl;
			ls_func = (void*) &jit_bit_shl;
			break;
		}
		case TokenType::BIT_SHIFT_LEFT_EQUALS : {
			if (v1->type.nature == Nature::VALUE and v2->type.nature == Nature::VALUE) {
				jit_value_t x = v1->compile(c);
				jit_value_t y = v2->compile(c);
				jit_value_t a = jit_insn_shl(c.F, x, y);
				jit_insn_store(c.F, x, a);
				if (v2->type.nature != Nature::POINTER and type.nature == Nature::POINTER) {
					return VM::value_to_pointer(c.F, a, type);
				}
				return a;
			} else {
				ls_func = (void*) &jit_bit_shl_equal;
			}
			break;
		}
		case TokenType::BIT_SHIFT_RIGHT : {
			jit_func = &jit_insn_shr;
			ls_func = (void*) &jit_bit_shr;
			break;
		}
		case TokenType::BIT_SHIFT_RIGHT_EQUALS : {
			if (v1->type.nature == Nature::VALUE and v2->type.nature == Nature::VALUE) {
				jit_value_t x = v1->compile(c);
				jit_value_t y = v2->compile(c);
				jit_value_t a = jit_insn_shr(c.F, x, y);
				jit_insn_store(c.F, x, a);
				if (v2->type.nature != Nature::POINTER and type.nature == Nature::POINTER) {
					return VM::value_to_pointer(c.F, a, type);
				}
				return a;
			} else {
				ls_func = (void*) &jit_bit_shr_equal;
			}
			break;
		}
		case TokenType::BIT_SHIFT_RIGHT_UNSIGNED : {
			jit_func = &jit_insn_ushr;
			ls_func = (void*) &jit_bit_shr_unsigned;
			break;
		}
		case TokenType::BIT_SHIFT_RIGHT_UNSIGNED_EQUALS : {
			if (v1->type.nature == Nature::VALUE and v2->type.nature == Nature::VALUE) {
				jit_value_t x = v1->compile(c);
				jit_value_t y = v2->compile(c);
				jit_value_t a = jit_insn_ushr(c.F, x, y);
				jit_insn_store(c.F, x, a);
				if (v2->type.nature != Nature::POINTER and type.nature == Nature::POINTER) {
					return VM::value_to_pointer(c.F, a, type);
				}
				return a;
			} else {
				ls_func = (void*) &jit_bit_shr_unsigned_equal;
			}
			break;
		}
		case TokenType::DOUBLE_QUESTION_MARK: {

			// x ?? y ==> if (x != null) { x } else { y }

			jit_label_t label_end = jit_label_undefined;
			jit_label_t label_else = jit_label_undefined;
			jit_value_t v = jit_value_create(c.F, LS_POINTER);

			jit_type_t args_types[2] = {LS_POINTER};
			jit_value_t x = v1->compile(c);
			jit_type_t sig = jit_type_create_signature(jit_abi_cdecl, LS_INTEGER, args_types, 1, 0);
			jit_value_t r = jit_insn_call_native(c.F, "is_null", (void*) jit_is_null, sig, &x, 1, JIT_CALL_NOTHROW);

			jit_insn_branch_if(c.F, r, &label_else);

			// then {
			jit_insn_store(c.F, v, x);
//			VM::inc_refs(c.F, x);

			// else
			jit_insn_branch(c.F, &label_end);
			jit_insn_label(c.F, &label_else);
			// {

			jit_value_t y = v2->compile(c);
			jit_insn_store(c.F, v, y);
//			VM::inc_refs(c.F, y);

			jit_insn_label(c.F, &label_end);

			return v;
			break;
		}
		default: {
			throw new exception();
		}
		}

	if (use_jit_func) {

		jit_value_t x = v1->compile(c);
		jit_value_t y = v2->compile(c);
		jit_value_t r = jit_func(c.F, x, y);

		if (type.nature == Nature::POINTER) {
			return VM::value_to_pointer(c.F, r, conv_info);
		}
		return r;

	} else {

		jit_type_t args_types[2] = {LS_POINTER, LS_POINTER};
		jit_type_t sig = jit_type_create_signature(jit_abi_cdecl, LS_POINTER, args_types, 2, 0);

		if (args.size() == 0) {
			args.push_back(v1->compile(c));
			args.push_back(v2->compile(c));
		}
		jit_value_t v = jit_insn_call_native(c.F, "", ls_func, sig, args.data(), 2, JIT_CALL_NOTHROW);
		if (v1->type.nature == Nature::VALUE and op->type == TokenType::PLUS_EQUAL) {
			jit_insn_store(c.F, args[0], v);
		}

		// Delete operands
		if (v1->type.must_manage_memory()) {
			VM::delete_temporary(c.F, args[0]);
		}
		if (v2->type.must_manage_memory()) {
			VM::delete_temporary(c.F, args[1]);
		}

		// Convert to value
		if (type == Type::BOOLEAN) {
			return VM::pointer_to_value(c.F, v, Type::BOOLEAN);
		}
		return v;
	}
}

}

