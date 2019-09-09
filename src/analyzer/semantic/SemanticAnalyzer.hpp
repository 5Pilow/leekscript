#ifndef SEMANTIC_ANALYZER_H_
#define SEMANTIC_ANALYZER_H_

#include <stack>
#include <vector>
#include <map>
#include "../../vm/VM.hpp"
#include "../error/Error.hpp"
#include "../semantic/Call.hpp"

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

class SemanticAnalyzer {
public:

	Program* program;
	VM* vm;
	std::vector<Function*> functions;
	std::vector<std::vector<Block*>> blocks;
	std::vector<FunctionVersion*> functions_stack;
	std::stack<int> loops;
	std::vector<Error> errors;

	SemanticAnalyzer();

	void analyze(Program*, Context*);

	void enter_function(FunctionVersion*);
	void leave_function();
	void enter_block(Block* block);
	void leave_block();
	void add_function(Function*);
	FunctionVersion* current_function() const;
	Block* current_block() const;

	void enter_loop();
	void leave_loop();
	bool in_loop(int deepness) const;

	Variable* add_var(Token*, const Type*, Value*);
	Variable* add_global_var(Token*, const Type*, Value*);
	Variable* get_var(const std::string& name);
	Variable* convert_var_to_any(Variable* var);
	Variable* update_var(Variable* variable);

	void add_error(Error ex);

};

}

#endif
