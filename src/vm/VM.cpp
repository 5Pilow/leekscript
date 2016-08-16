#include <sstream>
#include <chrono>

#include "VM.hpp"

#include "../compiler/lexical/LexicalAnalyser.hpp"
#include "../compiler/syntaxic/SyntaxicAnalyser.hpp"
#include "Context.hpp"
#include "../compiler/semantic/SemanticAnalyser.hpp"
#include "../compiler/semantic/SemanticException.hpp"
#include "value/LSNumber.hpp"
#include "value/LSArray.hpp"
#include "Program.hpp"
#include "value/LSObject.hpp"

using namespace std;

namespace ls {

VM::VM() {}

VM::~VM() {}

unsigned int VM::operations = 0;
const bool VM::enable_operations = true;
const unsigned int VM::operation_limit = 2000000000;

map<string, jit_value_t> internals;

void VM::add_module(Module* m) {
	modules.push_back(m);
}

extern std::map<LSValue*, LSValue*> objs;

string VM::execute(const std::string code, std::string ctx, ExecMode mode) {

	LSValue::obj_count = 0;
	LSValue::obj_deleted = 0;
	objs = {};

	auto compile_start = chrono::high_resolution_clock::now();

	LexicalAnalyser lex;
	vector<Token> tokens = lex.analyse(code);

	if (lex.errors.size()) {
		if (mode == ExecMode::TEST) {
			throw lex.errors[0];
		}
		for (auto error : lex.errors) {
			cout << "Line " << error.line << " : " <<  error.message() << endl;
		}
		return ctx;
	}

	SyntaxicAnalyser syn;
	Program* program = syn.analyse(tokens);

	if (syn.getErrors().size() > 0) {
		if (mode == ExecMode::COMMAND_JSON) {

			cout << "{\"success\":false,\"errors\":[";
			for (auto error : syn.getErrors()) {
				cout << "{\"line\":" << error->token->line << ",\"message\":\"" << error->message << "\"}";
			}
			cout << "]}" << endl;
			return ctx;

		} else {
			for (auto error : syn.getErrors()) {
				cout << "Line " << error->token->line << " : " <<  error->message << endl;
			}
			return ctx;
		}
	}

	Context context { ctx };

	SemanticAnalyser sem;
	sem.analyse(program, &context, modules);

	/*
	 * Debug
	 */
	cout << "Program: "; program->print(cout, true);

	if (sem.errors.size()) {

		if (mode == ExecMode::COMMAND_JSON) {
			cout << "{\"success\":false,\"errors\":[]}" << endl;
		} else if (mode == ExecMode::TEST) {
			delete program;
			throw sem.errors[0];
		} else {
			for (auto e : sem.errors) {
				cout << "Line " << e.line << " : " << e.message() << endl;
			}
		}
		return ctx;
	}


	// Compilation
	internals.clear();

	program->compile(context);

	auto compile_end = chrono::high_resolution_clock::now();

	/*
	 * Execute
	 */
	operations = 0;

	auto exe_start = chrono::high_resolution_clock::now();
	LSValue* res = program->execute();
	auto exe_end = chrono::high_resolution_clock::now();

	long exe_time_ns = chrono::duration_cast<chrono::nanoseconds>(exe_end - exe_start).count();
	long compile_time_ns = chrono::duration_cast<chrono::nanoseconds>(compile_end - compile_start).count();

	double exe_time_ms = (((double) exe_time_ns / 1000) / 1000);
	double compile_time_ms = (((double) compile_time_ns / 1000) / 1000);

	/*
	 * Return results
	 */
	string result;

	if (mode == ExecMode::COMMAND_JSON || mode == ExecMode::TOP_LEVEL) {

		ostringstream oss;
		res->print(oss);
		result = oss.str();

		string ctx = "{";

//		unsigned i = 0;
/*
		for (auto g : globals) {
			if (globals_ref[g.first]) continue;
			LSValue* v = res_array->operator[] (i + 1);
			ctx += "\"" + g.first + "\":" + v->to_json();
			if (i < globals.size() - 1) ctx += ",";
			i++;
		}
		*/
		ctx += "}";
		LSValue::delete_val(res);

		if (mode == ExecMode::TOP_LEVEL) {
			cout << result << endl;
			cout << "(" << VM::operations << " ops, " << compile_time_ms << " ms + " << exe_time_ms << " ms)" << endl;
			result = ctx;
		} else {
			cout << "{\"success\":true,\"ops\":" << VM::operations << ",\"time\":" << exe_time_ns << ",\"ctx\":" << ctx << ",\"res\":\""
					<< result << "\"}" << endl;
			result = ctx;
		}

	} else if (mode == ExecMode::FILE_JSON) {

		LSArray<LSValue*>* res_array = (LSArray<LSValue*>*) res;

		ostringstream oss;
		res_array->operator[] (0)->print(oss);
		result = oss.str();

		LSValue::delete_val(res);

		cout << "{\"success\":true,\"ops\":" << VM::operations << ",\"time\":" << exe_time_ns
			 << ",\"ctx\":" << ctx << ",\"res\":\"" << result << "\"}" << endl;


	} else if (mode == ExecMode::NORMAL) {

		ostringstream oss;
		res->print(oss);
		LSValue::delete_val(res);
		string res_string = oss.str();

		cout << res_string << endl;
		cout << "(" << VM::operations << " ops, " << compile_time_ms << "ms + " << exe_time_ms << " ms)" << endl;

		result = ctx;

	} else if (mode == ExecMode::TEST) {

		ostringstream oss;
		res->print(oss);
		result = oss.str();

		LSValue::delete_val(res);

	} else if (mode == ExecMode::TEST_OPS) {

		LSValue::delete_val(res);
		result = to_string(VM::operations);
	}

	/*
	 * Cleaning
	 */
	delete program;

	if (ls::LSValue::obj_deleted != ls::LSValue::obj_count) {
		cout << "/!\\ " << LSValue::obj_deleted << " / " << LSValue::obj_count << " (" << (LSValue::obj_count - LSValue::obj_deleted) << " leaked)" << endl;
		for (auto o : objs) {
			o.second->print(cout);
			cout << " (" << o.second->refs << " refs)" << endl;
		}
	}

	return result;
}

jit_type_t VM::get_jit_type(const Type& type) {
	if (type.nature == Nature::POINTER) {
		return JIT_POINTER;
	}
	if (type.raw_type == RawType::INTEGER || type.raw_type == RawType::BOOLEAN) {
		return JIT_INTEGER;
	}
	if (type.raw_type == RawType::LONG or type.raw_type == RawType::FUNCTION) {
		return JIT_INTEGER_LONG;
	}
	if (type.raw_type == RawType::FLOAT) {
		return JIT_FLOAT;
	}
	return JIT_INTEGER;
}

LSValue* create_null_object(int) {
	return LSNull::get();
}
LSValue* create_number_object_int(int n) {
	return LSNumber::get(n);
}
LSValue* create_number_object_long(long n) {
	return LSNumber::get(n);
}
LSValue* create_bool_object(bool n) {
	return LSBoolean::get(n);
}
LSValue* create_func_object(void* f) {
	return new LSFunction(f);
}
LSValue* create_float_object(double n) {
	return LSNumber::get(n);
}

void* get_conv_fun(Type type) {
	if (type.raw_type == RawType::NULLL) {
		return (void*) &create_null_object;
	}
	if (type.raw_type == RawType::INTEGER) {
		return (void*) &create_number_object_int;
	}
	if (type.raw_type == RawType::LONG) {
		return (void*) &create_number_object_long;
	}
	if (type.raw_type == RawType::FLOAT) {
		return (void*) &create_float_object;
	}
	if (type.raw_type == RawType::BOOLEAN) {
		return (void*) &create_bool_object;
	}
	if (type.raw_type == RawType::FUNCTION) {
		return (void*) &create_func_object;
	}
	return (void*) &create_number_object_int;
}

jit_value_t VM::value_to_pointer(jit_function_t F, jit_value_t v, Type type) {

	void* fun = get_conv_fun(type);

	bool floatt = jit_type_get_kind(jit_value_get_type(v)) == JIT_TYPE_FLOAT64;
	if (floatt) {
		fun = (void*) &create_float_object;
	}

	jit_type_t args_types[1] = {
		(type.raw_type == RawType::FUNCTION) ? JIT_POINTER :
		(type.raw_type == RawType::LONG) ? JIT_INTEGER_LONG :
		(type.raw_type == RawType::FLOAT) ?	JIT_FLOAT :
			JIT_INTEGER
	};
	jit_type_t sig = jit_type_create_signature(jit_abi_cdecl, JIT_POINTER, args_types, 1, 0);
	return jit_insn_call_native(F, "convert", (void*) fun, sig, &v, 1, JIT_CALL_NOTHROW);
}

int boolean_to_value(LSBoolean* b) {
	return b->value;
}

jit_value_t VM::pointer_to_value(jit_function_t F, jit_value_t v, Type type) {

	if (type == Type::BOOLEAN) {
		jit_type_t args_types[1] = {JIT_POINTER};
		jit_type_t sig = jit_type_create_signature(jit_abi_cdecl, JIT_INTEGER, args_types, 1, 0);
		return jit_insn_call_native(F, "convert", (void*) boolean_to_value, sig, &v, 1, JIT_CALL_NOTHROW);
	}
	return JIT_CREATE_CONST(F, JIT_INTEGER, 0);
}

/*
bool VM::get_number(jit_function_t F, jit_value_t val) {

	// x & (1 << 31) == 0

	jit_value_t is_int = jit_insn_eq(F,
		jit_insn_and(F, val, jit_value_create_nint_constant(F, JIT_INTEGER, 2147483648)),
		jit_value_create_nint_constant(F, JIT_INTEGER, 0)
	);

	jit_value_t res;

	jit_label_t label_else;
	jit_insn_branch_if_not(F, is_int, &label_else);

	return false;
}*/

LSArray<LSValue*>* new_array() {
	return new LSArray<LSValue*>();
}

void push_array_value(LSArray<LSValue*>* array, int value) {
	array->push_clone(LSNumber::get(value));
}

void push_array_pointer(LSArray<LSValue*>* array, LSValue* value) {
	array->push_clone(value);
}

jit_value_t VM::new_array(jit_function_t F) {
	jit_type_t args_t[0] = {};
	jit_value_t args[0] = {};
	jit_type_t sig = jit_type_create_signature(jit_abi_cdecl, JIT_INTEGER, args_t, 0, 0);
	return jit_insn_call_native(F, "new", (void*) new_array, sig, args, 0, JIT_CALL_NOTHROW);
}

void VM::push_array_value(jit_function_t F, jit_value_t array, jit_value_t value) {
	jit_type_t args[2] = {JIT_INTEGER, JIT_INTEGER};
	jit_type_t sig = jit_type_create_signature(jit_abi_cdecl, JIT_VOID, args, 2, 0);
	jit_value_t args_v[] = {array, value};
	jit_insn_call_native(F, "push", (void*) push_array_value, sig, args_v, 2, JIT_CALL_NOTHROW);
}

void VM::push_array_pointer(jit_function_t F, jit_value_t array, jit_value_t value) {
	jit_type_t args[2] = {JIT_INTEGER, JIT_INTEGER};
	jit_type_t sig = jit_type_create_signature(jit_abi_cdecl, JIT_VOID, args, 2, 0);
	jit_value_t args_v[] = {array, value};
	jit_insn_call_native(F, "push", (void*) push_array_pointer, sig, args_v, 2, JIT_CALL_NOTHROW);
}

int VM_get_refs(LSValue* val) {
	return val->refs;
}

jit_value_t VM::get_refs(jit_function_t F, jit_value_t obj) {
	jit_type_t args[1] = {JIT_POINTER};
	jit_type_t sig = jit_type_create_signature(jit_abi_cdecl, JIT_POINTER, args, 1, 0);
	return jit_insn_call_native(F, "get_refs", (void*) VM_get_refs, sig, &obj, 1, JIT_CALL_NOTHROW);
}

void VM_inc_refs(LSValue* val) {
	val->refs++;
}

void VM::inc_refs(jit_function_t F, jit_value_t obj) {
	jit_type_t args[1] = {JIT_POINTER};
	jit_type_t sig = jit_type_create_signature(jit_abi_cdecl, JIT_VOID, args, 1, 0);
	jit_insn_call_native(F, "inc_refs", (void*) VM_inc_refs, sig, &obj, 1, JIT_CALL_NOTHROW);
}

void VM_inc_refs_if_not_temp(LSValue* val) {
	if (val->refs != 0) {
		val->refs++;
	}
}

void VM::inc_refs_if_not_temp(jit_function_t F, jit_value_t obj) {
	jit_type_t args[1] = {JIT_POINTER};
	jit_type_t sig = jit_type_create_signature(jit_abi_cdecl, JIT_VOID, args, 1, 0);
	jit_insn_call_native(F, "inc_refs_not_temp", (void*) VM_inc_refs_if_not_temp, sig, &obj, 1, JIT_CALL_NOTHROW);
}

void VM_dec_refs(LSValue* val) {
	val->refs--;
}

void VM::dec_refs(jit_function_t F, jit_value_t obj) {
	jit_type_t args[1] = {JIT_POINTER};
	jit_type_t sig = jit_type_create_signature(jit_abi_cdecl, JIT_VOID, args, 1, 0);
	jit_insn_call_native(F, "dec_refs", (void*) VM_dec_refs, sig, &obj, 1, JIT_CALL_NOTHROW);
}

void VM::delete_obj(jit_function_t F, jit_value_t obj) {
	jit_type_t args[1] = {JIT_POINTER};
	jit_type_t sig = jit_type_create_signature(jit_abi_cdecl, JIT_VOID, args, 1, 0);
	jit_insn_call_native(F, "delete", (void*) &LSValue::delete_val, sig, &obj, 1, JIT_CALL_NOTHROW);
}

void VM_delete_temporary(LSValue* val) {
	if (val->refs == 0) {
		delete val;
	}
}

void VM::delete_temporary(jit_function_t F, jit_value_t obj) {
	jit_type_t args[1] = {JIT_POINTER};
	jit_type_t sig = jit_type_create_signature(jit_abi_cdecl, JIT_VOID, args, 1, 0);
	jit_insn_call_native(F, "delete_temporary", (void*) VM_delete_temporary, sig, &obj, 1, JIT_CALL_NOTHROW);
}


void VM_operation_exception() {
	throw vm_operation_exception();
}

void VM::inc_ops(jit_function_t F, int add) {

	if (not enable_operations) return;

	// Variable counter pointer
	jit_value_t jit_ops_ptr = jit_value_create_long_constant(F, JIT_POINTER, (long int) &VM::operations);

	// Increment counter
	jit_value_t jit_ops = jit_insn_load_relative(F, jit_ops_ptr, 0, jit_type_uint);
	jit_insn_store_relative(F, jit_ops_ptr, 0, jit_insn_add(F, jit_ops, jit_value_create_nint_constant(F, jit_type_uint, add)));

	// Compare to the limit
	jit_value_t compare = jit_insn_gt(F, jit_ops, jit_value_create_nint_constant(F, jit_type_uint, VM::operation_limit));
	jit_label_t label_end = jit_label_undefined;
	jit_insn_branch_if_not(F, compare, &label_end);

	// If greater than the limit, throw exception
//	jit_type_t args[1] = {JIT_INTEGER};
//	jit_type_t sig = jit_type_create_signature(jit_abi_cdecl, JIT_VOID, args, 1, 0);
//	jit_insn_call_native(F, "throw_exception", (void*) VM_operation_exception, sig, &jit_ops, 1, JIT_CALL_NOTHROW);
	jit_insn_throw(F, jit_value_create_nint_constant(F, JIT_INTEGER, 12));

	// End
	jit_insn_label(F, &label_end);
}

void VM_print_int(int val) {
//	cout << val << endl;
	cout << "Execution ended, too much operations: " << VM::operations << " (" << val << ")" << endl;
}

void VM::print_int(jit_function_t F, jit_value_t val) {
	jit_type_t args[1] = {JIT_INTEGER};
	jit_type_t sig = jit_type_create_signature(jit_abi_cdecl, JIT_VOID, args, 1, 0);
	jit_insn_call_native(F, "print_int", (void*) VM_print_int, sig, &val, 1, JIT_CALL_NOTHROW);
}

jit_value_t VM::get_null(jit_function_t F) {
	return JIT_CREATE_CONST_POINTER(F, LSNull::get());
}

LSObject* VM_create_object() {
	return new LSObject();
}

jit_value_t VM::create_object(jit_function_t F) {
	jit_type_t sig = jit_type_create_signature(jit_abi_cdecl, JIT_POINTER, {}, 0, 0);
	return jit_insn_call_native(F, "create_object", (void*) VM_create_object, sig, {}, 0, JIT_CALL_NOTHROW);
}

LSValue* VM_move(LSValue* val) {
	return val->move();
}

jit_value_t VM::move_obj(jit_function_t F, jit_value_t obj) {
	jit_type_t args[1] = {JIT_POINTER};
	jit_type_t sig = jit_type_create_signature(jit_abi_cdecl, JIT_POINTER, args, 1, 0);
	return jit_insn_call_native(F, "move", (void*) VM_move, sig, &obj, 1, JIT_CALL_NOTHROW);
}

bool VM_is_true(LSValue* val) {
	return val->isTrue();
}

jit_value_t VM::is_true(jit_function_t F, jit_value_t ptr) {
	jit_type_t args[1] = {JIT_POINTER};
	jit_type_t sig = jit_type_create_signature(jit_abi_cdecl, JIT_BOOLEAN, args, 1, 0);
	return jit_insn_call_native(F, "is_true", (void*) VM_is_true, sig, &ptr, 1, JIT_CALL_NOTHROW);
}

}
