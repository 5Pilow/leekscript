#ifndef SEMANTICANALYSER_H_
#define SEMANTICANALYSER_H_

#include <stack>
#include <vector>
#include <map>

#include "../../vm/Type.hpp"
#include "SemanticException.hpp"

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
	INTERNAL, LOCAL, PARAMETER
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

	SemanticVar(std::string name, VarScope scope, Type type, int index, Value* value,
		VariableDeclaration* vd, Function* function) :
		name(name), scope(scope), type(type), index(index), value(value), vd(vd), function(function) {}

	void will_take(SemanticAnalyser*, const std::vector<Type>& args_type);
	void will_take_element(SemanticAnalyser*, const Type&);
	void must_be_pointer(SemanticAnalyser*);
};

class SemanticAnalyser {
public:

	Program* program;
	bool in_program = false;

	std::map<std::string, SemanticVar*> internal_vars;
	std::vector<std::vector<std::map<std::string, SemanticVar*>>> variables;
	std::vector<std::map<std::string, SemanticVar*>> parameters;

	std::vector<Function*> functions;
	std::stack<Function*> functions_stack;
	std::stack<int> loops;

	std::vector<SemanticException> errors;

	SemanticAnalyser();
	virtual ~SemanticAnalyser();

	void analyse(Program*, Context*, std::vector<Module*>&);

	void enter_function(Function*);
	void leave_function();
	void enter_block();
	void leave_block();
	void add_function(Function*);
	Function* current_function() const;

	void enter_loop();
	void leave_loop();
	bool in_loop(int deepness) const;

	SemanticVar* add_var(Token*, Type, Value*, VariableDeclaration*);
	SemanticVar* add_parameter(Token*, Type);

	SemanticVar* get_var(Token* name);
	SemanticVar* get_var_direct(std::string name);
	std::map<std::string, SemanticVar*>& get_local_vars();

	void add_error(SemanticException ex);

};

}

#endif
