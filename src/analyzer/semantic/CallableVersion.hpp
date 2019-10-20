#ifndef CALLABLE_VERSION_HPP
#define CALLABLE_VERSION_HPP

#include <iostream>
#include <vector>
#include "../../constants.h"
#if COMPILER
#include "../../compiler/Compiler.hpp"
#endif

namespace ls {

class TypeMutator;
class SemanticAnalyzer;
class CallableVersion;
class Callable;
class Type;
class FunctionVersion;
class Value;

class CallableVersion {
public:
	std::string name;
	const Type* type;
	bool object = false;
	bool symbol = false;
	const Value* value = nullptr;
	FunctionVersion* user_fun = nullptr;
	std::vector<const TypeMutator*> mutators;
	std::vector<const Type*> templates;
	bool unknown = false;
	bool v1_addr = false;
	bool v2_addr = false;
	int flags = 0;
	void* addr = nullptr;
	#if COMPILER
	std::function<Compiler::value(Compiler&, std::vector<Compiler::value>, int)> func = nullptr;
	Compiler::value extra_arg;
	#endif

	CallableVersion(std::string name, const Type* type, std::vector<const TypeMutator*> mutators = {}, std::vector<const Type*> templates = {}, bool object = false, bool unknown = false, bool v1_addr = false, bool v2_addr = false, int flags = 0);
	#if COMPILER
	CallableVersion(std::string name, const Type* type, std::function<Compiler::value(Compiler&, std::vector<Compiler::value>, int)> func, std::vector<const TypeMutator*> mutators = {}, std::vector<const Type*> templates = {}, bool object = false, bool unknown = false, bool v1_addr = false, bool v2_addr = false, int flags = 0);
	#endif
	CallableVersion(std::string name, const Type* type, void* addr, std::vector<const TypeMutator*> mutators = {}, std::vector<const Type*> templates = {}, bool object = false, bool unknown = false, bool v1_addr = false, bool v2_addr = false, int flags = 0);
	CallableVersion(std::string name, const Type* type, const Value* value, std::vector<const TypeMutator*> mutators = {}, std::vector<const Type*> templates = {}, bool object = false, bool unknown = false, bool v1_addr = false, bool v2_addr = false, int flags = 0);
	CallableVersion(std::string name, const Type* type, FunctionVersion* f, std::vector<const TypeMutator*> mutators = {}, std::vector<const Type*> templates = {}, bool object = false, bool unknown = false, bool v1_addr = false, bool v2_addr = false, int flags = 0);

	CallableVersion(const Type* return_type, std::initializer_list<const Type*> arguments, void* addr, int flags = 0, std::vector<const TypeMutator*> mutators = {});
	#if COMPILER
	CallableVersion(const Type* return_type, std::initializer_list<const Type*> arguments, std::function<Compiler::value(Compiler&, std::vector<Compiler::value>, int)> func, int flags = 0, std::vector<const TypeMutator*> mutators = {});
	#endif

	CallableVersion(const Type* v1_type, const Type* v2_type, const Type* return_type, void* addr, int flags = 0, std::vector<const TypeMutator*> mutators = {}, bool v1_addr = false, bool v2_addr = false);
	#if COMPILER
	CallableVersion(const Type* v1_type, const Type* v2_type, const Type* return_type, std::function<Compiler::value(Compiler&, std::vector<Compiler::value>, int)> func, int flags = 0, std::vector<const TypeMutator*> mutators = {}, bool v1_addr = false, bool v2_addr = false);
	#endif

	std::pair<int, const CallableVersion*> get_score(SemanticAnalyzer* analyzer, std::vector<const Type*> arguments) const;
	void apply_mutators(SemanticAnalyzer* analyzer, std::vector<Value*> arguments) const;
	void resolve_templates(SemanticAnalyzer* analyzer, std::vector<const Type*> arguments) const;

	#if COMPILER
	int compile_mutators(Compiler& c, std::vector<Value*> arguments) const;
	Compiler::value compile_call(Compiler& c, std::vector<Compiler::value> args, int flags) const;
	#endif
};

}

namespace std {
	std::ostream& operator << (std::ostream&, const ls::CallableVersion&);
	std::ostream& operator << (std::ostream&, const ls::CallableVersion*);
}

#endif