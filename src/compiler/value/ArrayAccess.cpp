#include "../../compiler/value/ArrayAccess.hpp"

#include "../../compiler/value/Array.hpp"
#include "../../vm/value/LSNull.hpp"
#include "../../vm/value/LSArray.hpp"
#include "../../vm/value/LSInterval.hpp"
#include "../semantic/SemanticAnalyser.hpp"
#include "../semantic/SemanticException.hpp"

using namespace std;

namespace ls {

ArrayAccess::ArrayAccess() {
	array = nullptr;
	key = nullptr;
	key2 = nullptr;
	type = Type::POINTER;
}

ArrayAccess::~ArrayAccess() {
	delete array;
	delete key;
	if (key2 != nullptr) {
		delete key2;
	}
}

void ArrayAccess::print(std::ostream& os, int indent, bool debug) const {
	array->print(os, indent, debug);
	os << "[";
	key->print(os, indent, debug);
	if (key2 != nullptr) {
		os << ":";
		key2->print(os, indent, debug);
	}
	os << "]";
	if (debug) {
		os << " " << type;
	}
}

unsigned ArrayAccess::line() const {
	return 0;
}

void ArrayAccess::analyse(SemanticAnalyser* analyser, const Type& req_type) {

	array->analyse(analyser, Type::UNKNOWN);
	key->analyse(analyser, Type::UNKNOWN);
	constant = array->constant && key->constant;

	if (array->type.raw_type == RawType::ARRAY || array->type.raw_type == RawType::INTERVAL) {
		array_element_type = array->type.getElementType();
		type = array_element_type;
	}

	if (array->type == Type::INTERVAL) {
		key->analyse(analyser, Type::INTEGER);
	}

	// Range array access : array[4:12], check if the values are numbers
	if (key != nullptr and key2 != nullptr) {

		key->analyse(analyser, Type::INTEGER);
		key2->analyse(analyser, Type::INTEGER);

		if (not key->type.isNumber()) {
			std::string k = "<key 1>";
			analyser->add_error({SemanticException::Type::ARRAY_ACCESS_RANGE_KEY_MUST_BE_NUMBER, 0, k});
		}
		if (not key2->type.isNumber()) {
			std::string k = "<key 2>";
			analyser->add_error({SemanticException::Type::ARRAY_ACCESS_RANGE_KEY_MUST_BE_NUMBER, 0, k});
		}

		type = array->type;

	} else {

		if (not key->type.isNumber()) {
			std::string k = "<key 1>";
			analyser->add_error({SemanticException::Type::ARRAY_ACCESS_KEY_MUST_BE_NUMBER, 0, k});
		}

		if (array_element_type == Type::INTEGER) {
			key->analyse(analyser, Type::INTEGER);
		} else {
			key->analyse(analyser, Type::POINTER);
		}
	}

	if (req_type.nature != Nature::UNKNOWN) {
		type.nature = req_type.nature;
	}

//	cout << "array access " << type << endl;
}

bool ArrayAccess::will_take(SemanticAnalyser* analyser, const std::vector<Type>& args_type) {

//	cout << "ArrayAccess::will_take " << arg_type << " at " << pos << endl;

	type.will_take(args_type);

	if (Array* arr = dynamic_cast<Array*>(array)) {
		arr->elements_will_take(analyser, args_type, 1);
	}
	if (ArrayAccess* arr = dynamic_cast<ArrayAccess*>(array)) {
		arr->array_access_will_take(analyser, args_type, 1);
	}

	type = array->type.getElementType();

	return false;
}

bool ArrayAccess::array_access_will_take(SemanticAnalyser* analyser, const vector<Type>& args_type, int level) {

	type.will_take(args_type);

	if (Array* arr = dynamic_cast<Array*>(array)) {
		arr->elements_will_take(analyser, args_type, level);
	}
	if (ArrayAccess* arr = dynamic_cast<ArrayAccess*>(array)) {
		arr->array_access_will_take(analyser, args_type, level + 1);
	}

	type = array->type.getElementType();

	return false;
}

void ArrayAccess::change_type(SemanticAnalyser*, const Type&) {
	// TODO
}

LSValue* access_temp(LSArray<LSValue*>* array, LSValue* key) {
	return array->at(key);
}

int access_temp_value(LSArray<int>* array, int key) {
	return array->atv(key);
}

LSValue** access_l(LSArray<LSValue*>* array, LSValue* key) {
	return array->atL(key);
}

int* access_l_value(LSArray<int>* array, int key) {
	return array->atLv(key);
}

LSValue* range(LSArray<LSValue*>* array, int start, int end) {
	return array->range(start, end);
}

int interval_access(const LSInterval* interval, int pos) {
	return interval->atv(pos);
}

jit_value_t ArrayAccess::compile(Compiler& c) const {

//	cout << "aa " << type << endl;

	jit_value_t a = array->compile(c);

	if (key2 == nullptr) {

		if (array->type == Type::INTERVAL) {

			jit_type_t args_types[2] = {JIT_POINTER, JIT_INTEGER};
			jit_type_t sig = jit_type_create_signature(jit_abi_cdecl, JIT_INTEGER, args_types, 2, 0);

			jit_value_t k = key->compile(c);

			jit_value_t args[] = {a, k};
			jit_value_t res = jit_insn_call_native(c.F, "access", (void*) interval_access, sig, args, 2, JIT_CALL_NOTHROW);

			VM::delete_temporary(c.F, a);

			// Array access : 2 operations
			VM::inc_ops(c.F, 2);

			if (type.nature == Nature::POINTER) {
				return VM::value_to_pointer(c.F, res, type);
			}
			return res;

		} else {

			jit_type_t args_types[2] = {JIT_POINTER, JIT_POINTER};
			jit_type_t sig = jit_type_create_signature(jit_abi_cdecl, JIT_POINTER, args_types, 2, 0);

			jit_value_t k = key->compile(c);

			void* func = array_element_type == Type::INTEGER ? (void*) access_temp_value : (void*) access_temp;

			jit_value_t args[] = {a, k};
			jit_value_t res = jit_insn_call_native(c.F, "access", func, sig, args, 2, JIT_CALL_NOTHROW);

			if (key->type.nature == Nature::POINTER) {
				VM::delete_temporary(c.F, k);
			}
			VM::delete_temporary(c.F, a);

			// Array access : 2 operations
			VM::inc_ops(c.F, 2);

			if (array_element_type.nature == Nature::VALUE and type.nature == Nature::POINTER) {
				return VM::value_to_pointer(c.F, res, type);
			}
			return res;
		}

	} else {

		jit_type_t args_types[3] = {JIT_POINTER, JIT_INTEGER, JIT_INTEGER};
		jit_type_t sig = jit_type_create_signature(jit_abi_cdecl, JIT_POINTER, args_types, 3, 0);

		jit_value_t start = key->compile(c);
		jit_value_t end = key2->compile(c);
		jit_value_t args[] = {a, start, end};

		jit_value_t result = jit_insn_call_native(c.F, "range", (void*) range, sig, args, 3, JIT_CALL_NOTHROW);

		VM::delete_temporary(c.F, a);

		return result;
	}
}

jit_value_t ArrayAccess::compile_l(Compiler& c) const {

	jit_value_t a = array->compile(c);

	jit_type_t args_types[2] = {JIT_POINTER, JIT_POINTER};
	jit_type_t sig = jit_type_create_signature(jit_abi_cdecl, JIT_POINTER, args_types, 2, 0);

	jit_value_t k = key->compile(c);

	jit_value_t args[] = {a, k};
	return jit_insn_call_native(c.F, "access_l", (void*) access_l_value, sig, args, 2, JIT_CALL_NOTHROW);
}

}
