#ifndef FUNCTION_VERSION_HPP
#define FUNCTION_VERSION_HPP

#include <unordered_map>
#include <memory>
#include <vector>
#include "../../constants.h"
#include "../PrintOptions.hpp"
#if COMPILER
#include "../../compiler/Compiler.hpp"
#endif

namespace ls {

class Block;
class Function;
class SemanticAnalyzer;
class Variable;
class Function_type;
class Type;

class FunctionVersion {
public:
	Function* parent;
	std::unique_ptr<Block> body;
	const Type* type;
	const Type* placeholder_type = nullptr;
	bool recursive = false;
	std::unordered_map<std::string, Variable*> initial_arguments;
	std::unordered_map<std::string, Variable*> arguments;
	std::vector<Variable*> captures_inside;
	bool pre_analyzed = false;
	#if COMPILER
	Compiler::value fun;
	Compiler::value value;
	llvm::BasicBlock* block = nullptr;
	#endif

	FunctionVersion(std::unique_ptr<Block> body);

	bool is_compiled() const;

	void print(std::ostream& os, int indent, PrintOptions options) const;

	const Type* getReturnType();
	Variable* capture(SemanticAnalyzer* analyzer, Variable* var);
	void analyze_global_functions(SemanticAnalyzer* analyzer);
	void pre_analyze(SemanticAnalyzer* analyzer, const std::vector<const Type*>& args);
	void analyze(SemanticAnalyzer* analyzer, const std::vector<const Type*>& args);

	#if COMPILER
	void create_function(Compiler& c);
	Compiler::value compile(Compiler& c, bool compile_body = true);
	void compile_return(Compiler& c, Compiler::value v, bool delete_variables = false) const;
	llvm::BasicBlock* get_landing_pad(Compiler& c);
	#endif
};

}

#endif