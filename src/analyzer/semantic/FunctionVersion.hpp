#ifndef FUNCTION_VERSION_HPP
#define FUNCTION_VERSION_HPP

#include <string>
#include <unordered_map>
#include <memory>
#include <vector>
#include "../../constants.h"
#include "../../util/json.hpp"
#include "../PrintOptions.hpp"
#include "Completion.hpp"
#include "Hover.hpp"
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
class Environment;

class FunctionVersion {
public:
	Function* parent = nullptr;
	std::unique_ptr<Block> body;
	const Type* type = nullptr;
	const Type* placeholder_type = nullptr;
	bool recursive = false;
	std::unordered_map<std::string, std::unique_ptr<Variable>> initial_arguments;
	std::unordered_map<std::string, Variable*> arguments;
	std::vector<Variable*> captures_inside;
	bool pre_analyzed = false;
	#if COMPILER
	Compiler::value fun;
	Compiler::value value;
	llvm::BasicBlock* block = nullptr;
	#endif

	FunctionVersion(Environment& env, std::unique_ptr<Block> body);

	bool is_compiled() const;

	void print(std::ostream& os, int indent, PrintOptions options) const;

	const Type* getReturnType(Environment& env);
	Variable* capture(SemanticAnalyzer* analyzer, Variable* var);
	void analyze_global_functions(SemanticAnalyzer* analyzer);
	void pre_analyze(SemanticAnalyzer* analyzer, const std::vector<const Type*>& args);
	void analyze(SemanticAnalyzer* analyzer, const std::vector<const Type*>& args);
	Completion autocomplete(SemanticAnalyzer& analyzer, size_t position);
	Hover hover(SemanticAnalyzer& analyzer, size_t position);

	#if COMPILER
	void create_function(Compiler& c);
	Compiler::value compile(Compiler& c, bool compile_body = true);
	void compile_return(Compiler& c, Compiler::value v, bool delete_variables = false) const;
	llvm::BasicBlock* get_landing_pad(Compiler& c);
	#endif
};

}

#endif