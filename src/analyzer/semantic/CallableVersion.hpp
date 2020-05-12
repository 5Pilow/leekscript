#ifndef CALLABLE_VERSION_HPP
#define CALLABLE_VERSION_HPP

#include <iostream>
#include <vector>
#include "../../constants.h"
#if COMPILER
#include "../../compiler/Compiler.hpp"
#endif

namespace ls {

class CallableVersionTemplate;
class SemanticAnalyzer;
class Type;
class Value;
class Callable;
class Environment;

class CallableVersion {
	const Callable* callable = nullptr;
	size_t index = 0;
public:
	const Type* type = nullptr;
	#if COMPILER
	Compiler::value extra_arg;
	#endif

	CallableVersion(Environment&);
	CallableVersion(Environment&, const Callable*, size_t index, const Type*);

	const CallableVersionTemplate* template_() const;
	operator bool() const;

	void apply_mutators(SemanticAnalyzer* analyzer, std::vector<Value*> arguments) const;

	#if COMPILER
	int compile_mutators(Compiler& c, std::vector<Value*> arguments) const;
	Compiler::value compile_call(Compiler& c, std::vector<Compiler::value> args, const Value* value, int flags) const;
	#endif
};

}

namespace std {
	std::ostream& operator << (std::ostream&, const ls::CallableVersion&);
	std::ostream& operator << (std::ostream&, const ls::CallableVersion*);
}

#endif