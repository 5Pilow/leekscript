#ifndef CALL_HPP_
#define CALL_HPP_

#include <vector>
#include <memory>
#include "../../constants.h"
#if COMPILER
#include "../../compiler/Compiler.hpp"
#endif

namespace ls {

class SemanticAnalyzer;
class CallableVersion;
class CallableVersionTemplate;
class Callable;
class Type;
class Value;

class Call {
public:
	std::vector<Callable*> callables;
	Value* object = nullptr;

	Call() {}
	Call(Callable* callable) : callables({ callable }) {}
	Call(Callable* callable, Value* object) : callables({ callable }), object(object) {}
	Call(std::vector<CallableVersionTemplate> versions);
	Call(std::initializer_list<CallableVersionTemplate> versions);
	Call(std::initializer_list<CallableVersionTemplate> versions, Value* object);
	void add_callable(Callable* callable);

	CallableVersion resolve(SemanticAnalyzer* analyzer, std::vector<const Type*> arguments) const;
	void apply_mutators(SemanticAnalyzer* analyzer, CallableVersion& version, std::vector<Value*> arguments) const;
	#if COMPILER
	Compiler::value pre_compile_call(Compiler& c) const;
	Compiler::value compile_call(Compiler& c, const CallableVersion& version, std::vector<Compiler::value> args, int flags) const;
	#endif
};

}

namespace std {
	std::ostream& operator << (std::ostream&, const ls::Call&);
	std::ostream& operator << (std::ostream&, const ls::Call*);
}

#endif