#ifndef VARIABLE_HPP
#define VARIABLE_HPP

#include <string>
#include "../../constants.h"
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
class Block;
class FunctionVersion;
class Class;
class Environment;
class Section;

enum class VarScope {
	INTERNAL, LOCAL, PARAMETER, CAPTURE
};

class Variable {
public:
	const std::string name;
	Token* token;
	VarScope scope;
	int index;
	int parent_index;
	Value* value = nullptr;
	FunctionVersion* function = nullptr;
	Block* block = nullptr;
	Section* section = nullptr;
	const Type* type;
	std::vector<const Type*> version;
	Call call;
	int id = 0;
	int generator = 0;
	Variable* root = nullptr;
	Variable* parent = nullptr;
	bool assignment = false;
	Class* clazz = nullptr;
	Value* array = nullptr;
	bool injected = false;
	bool global = false;
	bool loop_variable = false;
	#if COMPILER
	Compiler::value entry; // Entry containing the value
	Compiler::value addr_val;
	Compiler::value val; // Direct access to value
	LSClass* lsclass = nullptr;
	#endif

	Variable(std::string name, Token* token, VarScope scope, const Type* type, int index, Value* value, FunctionVersion* function, Block* block, Section* section, Class* clazz, Call call = {}, bool global = false);
	#if COMPILER
	Variable(std::string name, Token* token, VarScope scope, const Type* type, int index, Value* value, FunctionVersion* function, Block* block, Section* section, Class* clazz, LSClass* lsclass, Call call = {});
	#endif

	Variable* get_root() const;
	const Type* get_entry_type(Environment& env) const;

	#if COMPILER
	Compiler::value get_value(Compiler& c) const;
	Compiler::value get_address(Compiler& c) const;
	void create_entry(Compiler& c);
	void create_addr_entry(Compiler& c, Compiler::value);
	void store_value(Compiler& c, Compiler::value);
	void delete_value(Compiler& c);
	#endif

	static Variable new_temporary(std::string name, const Type* type);
	static const Type* get_type_for_variable_from_expression(Environment& env, const Value* value);
};

}

namespace ls {
	std::ostream& operator << (std::ostream& os, const Variable&);
	std::ostream& operator << (std::ostream& os, const Variable*);
}

#endif