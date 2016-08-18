#ifndef VM_HPP
#define VM_HPP

#include <vector>
#include <string>
#include <jit/jit.h>

#define OPERATION_LIMIT 10000000

/*
 * 0 : no debug,
 * 1 : print types + #leaks
 * 2 : print leak details
 */
#define DEBUG 0

#define LS_INTEGER jit_type_int
#define LS_LONG jit_type_long
#define LS_REAL jit_type_float64
#define LS_POINTER jit_type_void_ptr

#define LS_CREATE_INTEGER(F, X) jit_value_create_nint_constant((F), LS_INTEGER, (X))
#define LS_CREATE_LONG(F, X) jit_value_create_long_constant((F), LS_LONG, (X))
#define LS_CREATE_REAL(F, X) jit_value_create_float64_constant((F), LS_REAL, (X))
#define LS_CREATE_POINTER(F, X) jit_value_create_constant((F), new jit_constant_t { LS_POINTER, {(X)} })

namespace ls {

class Type;
class Module;
class Program;

enum class ExecMode {
	NORMAL, TOP_LEVEL, COMMAND_JSON, TEST, TEST_OPS, FILE_JSON
};

class vm_operation_exception : public std::exception {
public:
	virtual const char* what() const throw (){
	   return "too much operations!";
	}
};

class VM {
public:

	static unsigned int operations;
	static const bool enable_operations;
	static const unsigned int operation_limit;

	std::vector<Module*> modules;

	VM();
	virtual ~VM();

	void add_module(Module* m);

	std::string execute(const std::string code, std::string ctx, ExecMode mode);

	static jit_type_t get_jit_type(const Type& type);
	static jit_value_t value_to_pointer(jit_function_t, jit_value_t, Type);
	static jit_value_t pointer_to_value(jit_function_t, jit_value_t, Type);
	// static bool is_number(void* v);

	static jit_value_t get_refs(jit_function_t F, jit_value_t obj);
	static void inc_refs(jit_function_t F, jit_value_t obj);
	static void inc_refs_if_not_temp(jit_function_t F, jit_value_t obj);
	static void dec_refs(jit_function_t F, jit_value_t obj);
	static void delete_ref(jit_function_t F, jit_value_t obj);
	static void delete_temporary(jit_function_t F, jit_value_t obj);
	static void inc_ops(jit_function_t F, int add);
	static void get_operations(jit_function_t F);
	static void print_int(jit_function_t F, jit_value_t val);
	static jit_value_t get_null(jit_function_t F);
	static jit_value_t create_object(jit_function_t F);
	static jit_value_t create_array(jit_function_t F, const Type& element_type, int cap = 0);
	static void push_move_array(jit_function_t F, const Type& element_type, jit_value_t array, jit_value_t value);
	static jit_value_t move_obj(jit_function_t F, jit_value_t ptr);
	static jit_value_t move_inc_obj(jit_function_t F, jit_value_t ptr);
	static jit_value_t clone_obj(jit_function_t F, jit_value_t ptr);
	static jit_value_t is_true(jit_function_t F, jit_value_t ptr);
};

}

#endif
