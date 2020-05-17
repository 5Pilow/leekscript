#ifndef PROGRAM_HPP
#define PROGRAM_HPP

#include "../analyzer/value/Function.hpp"
#include "PrintOptions.hpp"
#include "Result.hpp"
#include <unordered_set>
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
	std::unique_ptr<Token> main_token;
	const Type* type;
	std::vector<Function*> functions;
	std::string file_name;
	std::unordered_map<std::string, std::unique_ptr<Variable>> globals;
	std::unordered_map<std::string, std::unique_ptr<Variable>> operators;
	File* main_file;
	std::unordered_set<File*> included_files;
	std::vector<std::unique_ptr<Token>> operators_tokens;
	std::vector<std::unique_ptr<Function>> operators_functions;
	Context* context = nullptr;
	Result result;
	#if COMPILER
	Compiler* compiler; // Keep compiler pointer to free module handle
	bool handle_created = false;
	llvm::Module* module = nullptr;
	llvm::orc::VModuleKey module_handle;
	#endif

	Program(Environment& env, const std::string& code, const std::string& file_name);
	Program(Environment& env, const std::string& code, File* file);
	virtual ~Program();

	void analyze(SyntaxicAnalyzer& syn, SemanticAnalyzer& sem, bool format, bool debug, bool sections);
	Completion autocomplete(SemanticAnalyzer& analyzer, size_t position);
	Hover hover(SemanticAnalyzer& analyzer, File* file, size_t position);

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
