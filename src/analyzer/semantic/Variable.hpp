#ifndef VARIABLE_HPP
#define VARIABLE_HPP

#include "Call.hpp"

namespace ls {

class Program;
class Module;
class Function;
class VariableValue;
class Context;
class Value;
class SemanticAnalyzer;
class Token;
class VariableDeclaration;
class Callable;
class Call;
class Phi;

enum class VarScope {
	INTERNAL, LOCAL, PARAMETER, CAPTURE
};

class Variable {
public:
	const std::string name;
	VarScope scope;
	int index;
	int parent_index;
	Value* value;
	FunctionVersion* function;
	Block* block;
	const Type* type;
	std::vector<const Type*> version;
	LSValue* lsvalue = nullptr;
	Call call;
	Compiler::value val;
	Compiler::value addr_val;
	int id = 0;
	int generator = 0;
	Variable* root = nullptr;
	Variable* parent = nullptr;
	Phi* phi = nullptr;
	bool assignment = false;

	Variable(std::string name, VarScope scope, const Type* type, int index, Value* value, FunctionVersion* function, Block* block, LSValue* lsvalue, Call call = {});

	const Type* get_entry_type() const;

	Compiler::value get_value(Compiler& c) const;
	Compiler::value get_address(Compiler& c) const;
	void create_entry(Compiler& c);
	void create_addr_entry(Compiler& c, Compiler::value);
	void store_value(Compiler& c, Compiler::value);

	static Variable* new_temporary(std::string name, const Type* type);
	static const Type* get_type_for_variable_from_expression(const Type* value_type);
};

}

namespace ls {
	std::ostream& operator << (std::ostream& os, const Variable&);
	std::ostream& operator << (std::ostream& os, const Variable*);
}

#endif