#ifndef CONTEXT_HPP
#define CONTEXT_HPP

#include <map>
#include <string>
#include "LSValue.hpp"
#include "../analyzer/semantic/Variable.hpp"

namespace ls {

class ContextVar {
public:
	void* value;
	const Type* type;
	Variable* variable;
};

class Context {
public:

	Context();
	Context(std::string ctx);
	virtual ~Context();

	std::unordered_map<std::string, ContextVar> vars;

	void add_variable(char* name, void* v, const Type* type);
};

}

namespace std {
	std::ostream& operator << (std::ostream&, const ls::Context*);
}

#endif
