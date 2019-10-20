#ifndef PROGRAM_HPP
#define PROGRAM_HPP

#include "../analyzer/value/Function.hpp"
#include "PrintOptions.hpp"
#include "Result.hpp"
#if COMPILER
#include "../vm/VM.hpp"
#endif

namespace ls {

class File;
class StandardLibrary;
class SyntaxicAnalyzer;
class Context;

class Program {
private:
	#if COMPILER
	void* closure = nullptr;
	#endif
public:

	Environment& env;
	std::string code; // The program code
	std::unique_ptr<Function> main;
	const Type* type;
	std::vector<Function*> functions;
	std::string file_name;
	std::unordered_map<std::string, Variable*> operators;
	File* main_file = nullptr;
	Context* context = nullptr;
	Result result;
	#if COMPILER
	Compiler* compiler; // Keep compiler pointer to free module handle
	bool handle_created = false;
	llvm::Module* module = nullptr;
	llvm::orc::VModuleKey module_handle;
	#endif

	Program(Environment& env, const std::string& code, const std::string& file_name);
	virtual ~Program();

	void analyze(SyntaxicAnalyzer& syn, SemanticAnalyzer& sem, bool format, bool debug, bool sections);
	std::vector<std::string> autocomplete(SemanticAnalyzer& analyzer, size_t position);

	/*
	 * Compile the program with a VM and a context (json)
	 */
	#if COMPILER
	void compile(Compiler& c, bool format = false, bool debug = false, bool assembly = false, bool pseudo_code = false, bool optimized_ir = false, bool ir = false, bool bitcode = false);
	void compile_leekscript(Compiler& c, bool format, bool debug, bool assembly, bool pseudo_code, bool optimized_ir);
	void compile_ir_file(Compiler& c);
	void compile_bitcode_file(Compiler& c);
	#endif

	Variable* get_operator(const std::string& name);

	/*
	 * Execute the program and get a std::string result
	 */
	#if COMPILER
	std::string execute(VM& vm);
	#endif

	void print(std::ostream& os, bool debug = false, bool sections = false) const;

	std::string underline_code(Location location, Location focus) const;
};

std::ostream& operator << (std::ostream& os, const Program* program);

}

#endif
