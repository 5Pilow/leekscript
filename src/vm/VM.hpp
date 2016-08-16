#ifndef VM_HPP
#define VM_HPP

#include <vector>
#include <string>
#include <jit/jit.h>

#define JIT_INTEGER			jit_type_sys_int
#define JIT_INTEGER_LONG	jit_type_sys_long
#define JIT_FLOAT			jit_type_sys_double
#define JIT_BOOLEAN			jit_type_sys_bool
#define JIT_POINTER			jit_type_void_ptr
#define JIT_VOID			jit_type_void

#define JIT_CREATE_CONST jit_value_create_nint_constant
#define JIT_CREATE_CONST_LONG jit_value_create_long_constant
#define JIT_CREATE_CONST_FLOAT jit_value_create_float64_constant
#define JIT_CREATE_CONST_POINTER(F, X) jit_value_create_constant((F), new jit_constant_t{ JIT_POINTER, {(X)} })

#define OPERATION_LIMIT 10000000

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
	static jit_value_t new_array(jit_function_t);
	// static bool is_number(void* v);
	static void push_array_value(jit_function_t, jit_value_t, jit_value_t);
	static void push_array_pointer(jit_function_t, jit_value_t, jit_value_t);

	static jit_value_t get_refs(jit_function_t F, jit_value_t obj);
	static void inc_refs(jit_function_t F, jit_value_t obj);
	static void inc_refs_if_not_temp(jit_function_t F, jit_value_t obj);
	static void dec_refs(jit_function_t F, jit_value_t obj);
	static void delete_obj(jit_function_t F, jit_value_t obj);
	static void delete_temporary(jit_function_t F, jit_value_t obj);
	static void inc_ops(jit_function_t F, int add);
	static void get_operations(jit_function_t F);
	static void print_int(jit_function_t F, jit_value_t val);
	static jit_value_t get_null(jit_function_t F);
	static jit_value_t create_object(jit_function_t F);
	static jit_value_t move_obj(jit_function_t F, jit_value_t obj);
	static jit_value_t is_true(jit_function_t F, jit_value_t ptr);
};

}

#endif
