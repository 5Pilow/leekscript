#ifndef CONTEXT_HPP
#define CONTEXT_HPP

#include <map>
#include <string>
#include <unordered_map>
#include "../analyzer/semantic/Variable.hpp"
#include "../environment/Environment.hpp"

namespace ls {

union ContextVarValue {
	LSValue* ls_value;
	int int_value;
	double real_value;
	long long_value;
	ContextVarValue() : int_value(0) {}
	ContextVarValue(int v) : int_value(v) {}
	ContextVarValue(LSValue* v) : ls_value(v) {}
	ContextVarValue(double v) : real_value(v) {}
	ContextVarValue(long v) : long_value(v) {}
};

class ContextVar {
public:
	ContextVarValue value;
	const Type* type;
	Variable* variable;
	std::unique_ptr<Token> token;
};

class Context {
public:

	Environment& env;
	std::unordered_map<std::string, ContextVar> vars;

	Context(Environment& env);
	Context(Environment& env, std::string ctx);
	virtual ~Context();

	void add_variable(char* name, ContextVarValue v, const Type* type);
};

}

namespace std {
	std::ostream& operator << (std::ostream&, const ls::Context*);
}

#endif
