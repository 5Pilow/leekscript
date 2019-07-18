#ifndef PROGRAM_HPP
#define PROGRAM_HPP

#include "../analyzer/value/Function.hpp"
#include "VM.hpp"

namespace ls {

class File;

class Program {
private:

	std::string code; // The program code
	void* closure;

public:

	std::unique_ptr<Function> main;
	const Type* type;
	std::vector<Function*> functions;
	std::string file_name;
	VM* vm;
	bool handle_created = false;
	llvm::Module* module = nullptr;
	llvm::orc::VModuleKey module_handle;
	std::unordered_map<std::string, Variable*> operators;
	File* main_file;

	Program(const std::string& code, const std::string& file_name);
	virtual ~Program();

	void analyze(SemanticAnalyzer* analyzer);

	/*
	 * Compile the program with a VM and a context (json)
	 */
	VM::Result compile(VM& vm, Context* context = nullptr, bool assembly = false, bool pseudo_code = false, bool optimized_ir = false, bool ir = false, bool bitcode = false);
	VM::Result compile_leekscript(VM& vm, Context* ctx, bool assembly, bool pseudo_code, bool optimized_ir);
	VM::Result compile_ir_file(VM& vm);
	VM::Result compile_bitcode_file(VM& vm);

	Variable* get_operator(const std::string& name);

	/*
	 * Execute the program and get a std::string result
	 */
	std::string execute(VM& vm);

	void print(std::ostream& os, bool debug = false) const;

	std::string underline_code(Location location, Location focus) const;
};

std::ostream& operator << (std::ostream& os, const Program* program);

}

#endif
