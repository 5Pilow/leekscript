#ifndef CONTEXT_HPP
#define CONTEXT_HPP

#include <map>
#include <string>
#include <unordered_map>
#include "../analyzer/semantic/Variable.hpp"
#include "../environment/Environment.hpp"

namespace ls {

class ContextVar {
public:
	void* value;
	const Type* type;
	Variable* variable;
};

class Context {
public:

	Environment& env;
	std::unordered_map<std::string, ContextVar> vars;

	Context(Environment& env);
	Context(Environment& env, std::string ctx);
	virtual ~Context();

	void add_variable(char* name, void* v, const Type* type);
};

}

namespace std {
	std::ostream& operator << (std::ostream&, const ls::Context*);
}

#endif
