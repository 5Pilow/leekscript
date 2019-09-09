#ifndef CALL_HPP_
#define CALL_HPP_

#include <vector>
#include "../Compiler.hpp"

namespace ls {

class SemanticAnalyzer;
class CallableVersion;
class Callable;
class Type;
class Value;

class Call {
public:
	Callable* callable = nullptr;
	Value* object = nullptr;

	Call() {}
	Call(Callable* callable) : callable(callable) {}
	Call(Callable* callable, Value* object) : callable(callable), object(object) {}
	Call(std::vector<const CallableVersion*> versions);
	Call(std::initializer_list<const CallableVersion*> versions);
	void add_version(const CallableVersion* v);

	const CallableVersion* resolve(SemanticAnalyzer* analyzer, std::vector<const Type*> arguments) const;
	void apply_mutators(SemanticAnalyzer* analyzer, const CallableVersion* version, std::vector<Value*> arguments) const;
	Compiler::value pre_compile_call(Compiler& c) const;
	Compiler::value compile_call(Compiler& c, const CallableVersion* version, std::vector<Compiler::value> args, int flags) const;
	
};

}

#endif