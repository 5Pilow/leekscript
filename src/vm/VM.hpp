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
#include "../compiler/lexical/LexicalError.hpp"
#include "../compiler/syntaxic/SyntaxicalError.hpp"
#include "../compiler/semantic/SemanticError.hpp"
#include "../compiler/Compiler.hpp"
#include "Exception.hpp"
#include "OutputStream.hpp"

#define OPERATION_LIMIT 10000000

namespace ls {

class Type;
class Module;
class Program;
class LSValue;
class SemanticVar;
class LSNull;
class LSBoolean;
class LSFunction;
class Callable;

class VM {
public:

	static const unsigned long int DEFAULT_OPERATION_LIMIT;
	static VM* current_vm;
	static OutputStream* default_output;

	struct Result {
		bool compilation_success = false;
		bool execution_success = false;
		std::vector<LexicalError> lexical_errors;
		std::vector<SyntaxicalError> syntaxical_errors;
		std::vector<SemanticError> semantical_errors;
		vm::ExceptionObj exception;
		std::string program = "";
		std::string value = "";
		Context* context = nullptr;
		long compilation_time = 0;
		long compilation_time_ms = 0;
		long execution_time = 0;
		long execution_time_ms = 0;
		long operations = 0;
		int objects_created = 0;
		int objects_deleted = 0;
		int mpz_objects_created = 0;
		int mpz_objects_deleted = 0;
		std::string assembly;
		std::string pseudo_code;
		std::string instructions_log;
		Type type;
	};

	std::vector<Module*> modules;
	std::vector<LSValue*> system_vars;
	std::vector<LSValue*> function_created;
	std::map<std::string, std::shared_ptr<SemanticVar>> internal_vars;
	std::map<std::string, Compiler::value> internals;
	Compiler compiler;
	LSNull* null_value;
	LSBoolean* true_value;
	LSBoolean* false_value;
	unsigned int operations = 0;
	bool enable_operations = true;
	unsigned int operation_limit;
	OutputStream* output = default_output;
	long mpz_created = 0;
	long mpz_deleted = 0;
	std::string file_name;
	bool legacy;
	Context* context = nullptr;

	VM(bool v1 = false);
	virtual ~VM();

	static VM* current();
	static void static_init();

	/** Main execution function **/
	Result execute(const std::string code, Context* ctx, std::string file_name, bool debug = false, bool ops = true, bool assembly = false, bool pseudo_code = false, bool log_instructions = false, bool execute_ir = false);

	/** Add a module **/
	void add_module(Module* m);
	void add_internal_var(std::string name, Type type, LSValue* value, Callable* callable = nullptr);
	void add_internal_var(std::string name, Type type, Function* function);
	
	void* resolve_symbol(std::string name);
};

}

#endif
