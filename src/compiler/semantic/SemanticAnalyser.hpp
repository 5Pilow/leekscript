#ifndef SEMANTICANALYSER_H_
#define SEMANTICANALYSER_H_

#include <stack>
#include <vector>
#include <map>

#include "../../vm/VM.hpp"
#include "../../vm/Type.hpp"
#include "SemanticError.hpp"

namespace ls {

class Program;
class Module;
class Function;
class VariableValue;
class Context;
class Value;
class SemanticAnalyser;
class Token;
class VariableDeclaration;

enum class VarScope {
	INTERNAL, LOCAL, PARAMETER, CAPTURE
};

class SemanticVar {
public:
	std::string name;
	VarScope scope;
	Type type;
	std::map<std::string, Type> attr_types;
	int index;
	Value* value;
	VariableDeclaration* vd;
	Function* function; // In which function the variable is declared

	SemanticVar(std::string name, VarScope scope, Type type, int index, Value* value, VariableDeclaration* vd, Function* function) :
		name(name), scope(scope), type(type), index(index), value(value), vd(vd), function(function) {}
	void must_be_pointer(SemanticAnalyser*);
};

class SemanticAnalyser {
public:

	Program* program;
	VM* vm;

	std::vector<std::vector<std::map<std::string, std::shared_ptr<SemanticVar>>>> variables;
	std::vector<std::map<std::string, std::shared_ptr<SemanticVar>>> parameters;

	std::vector<Function*> functions;
	std::stack<Function*> functions_stack;
	std::stack<int> loops;

	std::vector<SemanticError> errors;

	SemanticAnalyser();
	virtual ~SemanticAnalyser();

	void analyse(Program*, Context*);

	void enter_function(Function*);
	void leave_function();
	void enter_block();
	void leave_block();
	void add_function(Function*);
	Function* current_function() const;
	void set_potential_return_type(Type type);

	void enter_loop();
	void leave_loop();
	bool in_loop(int deepness) const;

	std::shared_ptr<SemanticVar> add_var(Token*, Type, Value*, VariableDeclaration*);
	std::shared_ptr<SemanticVar> add_parameter(Token*, Type);

	std::shared_ptr<SemanticVar> get_var(Token* name);
	std::map<std::string, std::shared_ptr<SemanticVar>>& get_local_vars();

	void add_error(SemanticError ex);

};

}

#endif
