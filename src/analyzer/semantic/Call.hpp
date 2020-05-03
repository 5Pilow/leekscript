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
	const Value* value = nullptr;
	Value* object = nullptr;

	Call() {}
	Call(const Value* value) : value(value) {}
	Call(Callable* callable) : callables({ callable }) {}
	Call(Callable* callable, const Value* value, Value* object = nullptr) : callables({ callable }), value(value), object(object) {}
	Call(std::vector<CallableVersionTemplate> versions);
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