#include "Array.hpp"
#include "../../vm/VM.hpp"
#include "../../vm/value/LSArray.hpp"
#include "../../vm/value/LSInterval.hpp"
#include <math.h>

using namespace std;

namespace ls {

Array::Array() {
	type = Type::ARRAY;
}

Array::~Array() {
	for (auto ex : expressions) {
		delete ex;
	}
}

void Array::print(std::ostream& os, int indent, bool debug) const {
	os << "array[";
	for (size_t i = 0; i < expressions.size(); ++i) {
		expressions[i]->print(os, indent, debug);
		if (i < expressions.size() - 1) {
			os << ", ";
		}
	}
	os << "]";

	if (debug) {
		os << " " << type;
	}
}

unsigned Array::line() const {
	return 0;
}

void Array::analyse(SemanticAnalyser* analyser, const Type&) {

	constant = true;

	if (interval) {

		type = Type::INTERVAL;

	} else {

		if (expressions.size() > 0) {

			Type element_type;

			for (size_t i = 0; i < expressions.size(); ++i) {

				Value* ex = expressions[i];
				ex->analyse(analyser);

				if (ex->constant == false) {
					constant = false;
				}
				if (i == 0) {
					element_type = ex->type;
				} else {
					element_type = Type::get_compatible_type(element_type, ex->type);
				}
			}

			// Native elements types supported : integer, double
			if (element_type == Type::INTEGER || element_type == Type::FLOAT) {
				supported_type = element_type;
			}
			// For function, we store them as pointers
			else if (element_type.raw_type == RawType::FUNCTION) {
				element_type.nature = Nature::POINTER;
				supported_type = element_type;
			} else {
				supported_type = Type::POINTER;
				// If there are some functions, they types will be lost, so tell them to return pointers
				supported_type.setReturnType(Type::POINTER);
			}

			// Re-analyze expressions with the supported type
			for (size_t i = 0; i < expressions.size(); ++i) {
				expressions[i]->analyse(analyser, supported_type);
			}

			// Second computation of the array type
			for (unsigned i = 0; i < expressions.size(); ++i) {
				Value* ex = expressions[i];
				if (i == 0) {
					element_type = ex->type;
				} else {
					element_type = Type::get_compatible_type(element_type, ex->type);
				}
			}
			type.setElementType(element_type);
		}
	}
}

void Array::elements_will_take(SemanticAnalyser* analyser, const unsigned pos, const Type& type, int level) {

//	cout << "Array::elements_will_take " << type << " at " << pos << endl;

	for (size_t i = 0; i < expressions.size(); ++i) {

		Array* arr = dynamic_cast<Array*>(expressions[i]);
		if (arr != nullptr && level > 0) {
			arr->elements_will_take(analyser, pos, type, level - 1);
		} else {
			expressions[i]->will_take(analyser, pos, type);
		}
	}

	// Computation of the new array type
	Type element_type;
	for (unsigned i = 0; i < expressions.size(); ++i) {
		Value* ex = expressions[i];
		if (i == 0) {
			element_type = ex->type;
		} else {
			element_type = Type::get_compatible_type(element_type, ex->type);
		}
	}
	this->type.setElementType(element_type);

//	cout << "Array::elements_will_take type after " << this->type << endl;
}

LSArray<LSValue*>* LSArray_create(int cap) {
	LSArray<LSValue*>* array = new LSArray<LSValue*>();
	array->reserve(cap);
	return array;
}

LSArray<int>* LSArray_create_integer(int cap) {
	LSArray<int>* array = new LSArray<int>();
	array->reserve(cap);
	return array;
}

LSArray<double>* LSArray_create_float(int cap) {
	LSArray<double>* array = new LSArray<double>();
	array->reserve(cap);
	return array;
}

LSInterval* LSArray_create_interval(int a, int b) {
	LSInterval* interval = new LSInterval();
	interval->a = a;
	interval->b = b;
	return interval;
}

void LSArray_push(LSArray<LSValue*>* array, LSValue* value) {
	array->push_clone(value);
	LSValue::delete_val(value);
}

void LSArray_push_integer(LSArray<int>* array, int value) {
	array->push_clone(value);
}

void LSArray_push_float(LSArray<double>* array, double value) {
	array->push_clone(value);
}

jit_value_t Array::compile(Compiler& c) const {

	if (interval) {

		jit_value_t a = expressions[0]->compile(c);
		jit_value_t b = expressions[1]->compile(c);

		jit_type_t args[2] = {JIT_INTEGER, JIT_INTEGER};
		jit_type_t sig = jit_type_create_signature(jit_abi_cdecl, JIT_POINTER, args, 2, 0);
		jit_value_t args_v[] = {a, b};

		jit_value_t interval = jit_insn_call_native(c.F, "new", (void*) LSArray_create_interval, sig, args_v, 2, JIT_CALL_NOTHROW);

		return interval;
	}

	jit_type_t args[1] = {JIT_INTEGER};
	jit_type_t sig = jit_type_create_signature(jit_abi_cdecl, JIT_POINTER, args, 1, 0);

	void* create = type.getElementType() == Type::INTEGER ?
			(void*) LSArray_create_integer :
			type.getElementType() == Type::FLOAT ? (void*) LSArray_create_float :
			(void*) LSArray_create;

	void* push = type.getElementType() == Type::INTEGER ?
			(void*) LSArray_push_integer :
			type.getElementType() == Type::FLOAT ? (void*) LSArray_push_float :
			(void*) LSArray_push;

	jit_type_t elem_type = type.getElementType() == Type::INTEGER ? JIT_INTEGER :
			type.getElementType() == Type::FLOAT ? JIT_FLOAT : JIT_POINTER;

	jit_value_t s = JIT_CREATE_CONST(c.F, ls_jit_integer, expressions.size());
	jit_value_t args_v[] = {s};
	jit_value_t array = jit_insn_call_native(c.F, "new", create, sig, args_v, 1, JIT_CALL_NOTHROW);

	for (Value* val : expressions) {

		jit_value_t v = val->compile(c);

		if (supported_type.must_manage_memory()) {
			VM::inc_refs(c.F, v);
		}

		jit_type_t args[2] = {JIT_POINTER, elem_type};
		jit_type_t sig = jit_type_create_signature(jit_abi_cdecl, jit_type_void, args, 2, 0);
		jit_value_t args_v[] = {array, v};
		jit_insn_call_native(c.F, "push", push, sig, args_v, 2, JIT_CALL_NOTHROW);
	}

	// size of the array + 1 operations
	VM::inc_ops(c.F, expressions.size() + 1);

	return array;
}

}
