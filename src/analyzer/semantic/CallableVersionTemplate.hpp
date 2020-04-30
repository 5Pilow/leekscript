#ifndef CALLABLE_VERSION_TEMPLATE_HPP
#define CALLABLE_VERSION_TEMPLATE_HPP

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

class CallableVersionTemplate {
public:
	std::string name;
	const Type* type;
	bool object = false;
	bool symbol = false;
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
	#endif

	CallableVersionTemplate(std::string name, const Type* type, std::vector<const TypeMutator*> mutators = {}, std::vector<const Type*> templates = {}, bool object = false, bool v1_addr = false, bool v2_addr = false, int flags = 0);
	#if COMPILER
	CallableVersionTemplate(std::string name, const Type* type, std::function<Compiler::value(Compiler&, std::vector<Compiler::value>, int)> func, std::vector<const TypeMutator*> mutators = {}, std::vector<const Type*> templates = {}, bool object = false, bool v1_addr = false, bool v2_addr = false, int flags = 0);
	#endif
	CallableVersionTemplate(std::string name, const Type* type, void* addr, std::vector<const TypeMutator*> mutators = {}, std::vector<const Type*> templates = {}, bool object = false, bool v1_addr = false, bool v2_addr = false, int flags = 0);

	CallableVersionTemplate(std::string name, const Type* type, bool unknown, std::vector<const TypeMutator*> mutators = {}, std::vector<const Type*> templates = {}, bool object = false, bool v1_addr = false, bool v2_addr = false, int flags = 0);

	CallableVersionTemplate(std::string name, const Type* type, FunctionVersion* f, std::vector<const TypeMutator*> mutators = {}, std::vector<const Type*> templates = {}, bool object = false, bool v1_addr = false, bool v2_addr = false, int flags = 0);

	CallableVersionTemplate(const Type* return_type, std::initializer_list<const Type*> arguments, void* addr, int flags = 0, std::vector<const TypeMutator*> mutators = {});
	#if COMPILER
	CallableVersionTemplate(const Type* return_type, std::initializer_list<const Type*> arguments, std::function<Compiler::value(Compiler&, std::vector<Compiler::value>, int)> func, int flags = 0, std::vector<const TypeMutator*> mutators = {});
	#endif

	CallableVersionTemplate(const Type* v1_type, const Type* v2_type, const Type* return_type, void* addr, int flags = 0, std::vector<const TypeMutator*> mutators = {}, bool v1_addr = false, bool v2_addr = false);
	#if COMPILER
	CallableVersionTemplate(const Type* v1_type, const Type* v2_type, const Type* return_type, std::function<Compiler::value(Compiler&, std::vector<Compiler::value>, int)> func, int flags = 0, std::vector<const TypeMutator*> mutators = {}, bool v1_addr = false, bool v2_addr = false);
	#endif

	std::pair<int, CallableVersion> get_score(SemanticAnalyzer* analyzer, std::vector<const Type*> arguments) const;
	void resolve_templates(SemanticAnalyzer* analyzer, std::vector<const Type*> arguments) const;
};

}

namespace std {
	std::ostream& operator << (std::ostream&, const ls::CallableVersionTemplate&);
	std::ostream& operator << (std::ostream&, const ls::CallableVersionTemplate*);
}

#endif