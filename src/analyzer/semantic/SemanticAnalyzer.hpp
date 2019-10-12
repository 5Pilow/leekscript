#ifndef SEMANTIC_ANALYZER_H_
#define SEMANTIC_ANALYZER_H_

#include <stack>
#include <vector>
#include <map>
#include "../error/Error.hpp"
#include "../semantic/Call.hpp"
#include "../../standard/StandardLibrary.hpp"
#include "../../environment/Environment.hpp"
#include "Variable.hpp"

namespace ls {

class Program;
class Module;
class Function;
class VariableValue;
class Context;
class Value;
class SemanticAnalyzer;
class Token;
class Callable;
class Call;
class Block;
class Section;
class Instruction;

class SemanticAnalyzer {
public:

	Environment& env;
	Program* program;
	std::vector<Function*> functions;
	std::vector<std::vector<Block*>> blocks;
	std::vector<std::vector<Section*>> sections;
	std::vector<std::vector<Instruction*>> loops;
	std::vector<FunctionVersion*> functions_stack;
	std::vector<Error> errors;
	std::unordered_map<std::string, std::unique_ptr<Variable>> globals;

	SemanticAnalyzer(Environment& env);

	void analyze(Program*);

	void enter_function(FunctionVersion*);
	void leave_function();
	void enter_block(Block* block);
	void leave_block();
	void enter_section(Section* section);
	void leave_section();
	void add_function(Function*);
	FunctionVersion* current_function() const;
	Block* current_block() const;
	Section* current_section() const;
	Instruction* current_loop() const;

	void enter_loop(Instruction* loop);
	void leave_loop();
	bool in_loop(int deepness) const;

	Variable* add_var(Token*, const Type*, Value*);
	Variable* add_global_var(Token*, const Type*, Value*);
	Variable* get_var(const std::string& name);
	Variable* convert_var_to_any(Variable* var);
	Variable* update_var(Variable* variable, bool add_mutation = true);

	void add_error(Error ex);

};

}

#endif
