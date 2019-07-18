#ifndef VM_HPP
#define VM_HPP

#include <vector>
#include <string>
#include <gmp.h>
#include <gmpxx.h>
#include <stdio.h>
#include <execinfo.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include "../analyzer/error/Error.hpp"
#include "../compiler/Compiler.hpp"
#include "Exception.hpp"
#include "OutputStream.hpp"
#include "../analyzer/semantic/Call.hpp"

#define OPERATION_LIMIT 10000000

namespace ls {

class Type;
class Module;
class Program;
class LSValue;
class Variable;
class LSNull;
class LSBoolean;
class LSFunction;
class Callable;
class Call;

class VM {
public:

	static const unsigned long int DEFAULT_OPERATION_LIMIT;
	static VM* current_vm;
	static OutputStream* default_output;

	struct Result {
		bool compilation_success = false;
		bool execution_success = false;
		std::vector<Error> errors;
		vm::ExceptionObj exception;
		std::string program = "";
		std::string value = "";
		double parse_time = 0;
		double compilation_time = 0;
		double execution_time = 0;
		long operations = 0;
		int objects_created = 0;
		int objects_deleted = 0;
		int mpz_objects_created = 0;
		int mpz_objects_deleted = 0;
		std::string assembly;
		std::string pseudo_code;
		const Type* type;
	};

	std::vector<std::unique_ptr<Module>> modules;
	std::vector<LSValue*> function_created;
	std::unordered_map<std::string, std::unique_ptr<Variable>> internal_vars;
	std::unordered_map<std::string, Compiler::value> internals;
	Compiler compiler;
	std::unique_ptr<LSNull> null_value;
	std::unique_ptr<LSBoolean> true_value;
	std::unique_ptr<LSBoolean> false_value;
	unsigned int operations = 0;
	bool enable_operations = true;
	unsigned int operation_limit;
	OutputStream* output = default_output;
	long mpz_created = 0;
	long mpz_deleted = 0;
	std::string file_name;
	bool legacy;
	Context* context = nullptr;

	VM(bool legacy = false);
	~VM();

	static VM* current();
	static void static_init();

	/** Main execution function **/
	Result execute(const std::string code, Context* ctx, std::string file_name = "unamed", bool debug = false, bool ops = true, bool assembly = false, bool pseudo_code = false, bool optimized_ir = false, bool execute_ir = false, bool execute_bitcode = false);

	/** Add a module **/
	void add_module(std::unique_ptr<Module> m);
	void add_internal_var(std::string name, const Type* type, LSValue* value, Call call = {});
	void add_internal_var(std::string name, const Type* type, Function* function);
	
	void* resolve_symbol(std::string name);
};

}

#endif
