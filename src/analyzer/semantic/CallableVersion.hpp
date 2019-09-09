#ifndef CALLABLE_VERSION_HPP
#define CALLABLE_VERSION_HPP

#include <vector>
#include "../Compiler.hpp"

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
	std::function<Compiler::value(Compiler&, std::vector<Compiler::value>, int)> func = nullptr;
	FunctionVersion* user_fun = nullptr;
	std::vector<TypeMutator*> mutators;
	std::vector<const Type*> templates;
	bool unknown = false;
	bool v1_addr = false;
	bool v2_addr = false;
	int flags = 0;
	void* addr = nullptr;
	Compiler::value extra_arg;

	CallableVersion(std::string name, const Type* type, std::vector<TypeMutator*> mutators = {}, std::vector<const Type*> templates = {}, bool object = false, bool unknown = false, bool v1_addr = false, bool v2_addr = false, int flags = 0);
	CallableVersion(std::string name, const Type* type, std::function<Compiler::value(Compiler&, std::vector<Compiler::value>, int)> func, std::vector<TypeMutator*> mutators = {}, std::vector<const Type*> templates = {}, bool object = false, bool unknown = false, bool v1_addr = false, bool v2_addr = false, int flags = 0);
	CallableVersion(std::string name, const Type* type, const Value* value, std::vector<TypeMutator*> mutators = {}, std::vector<const Type*> templates = {}, bool object = false, bool unknown = false, bool v1_addr = false, bool v2_addr = false, int flags = 0);
	CallableVersion(std::string name, const Type* type, FunctionVersion* f, std::vector<TypeMutator*> mutators = {}, std::vector<const Type*> templates = {}, bool object = false, bool unknown = false, bool v1_addr = false, bool v2_addr = false, int flags = 0);
	
	CallableVersion(const Type* return_type, std::initializer_list<const Type*> arguments, void* addr, int flags = 0, std::vector<TypeMutator*> mutators = {});
	CallableVersion(const Type* return_type, std::initializer_list<const Type*> arguments, std::function<Compiler::value(Compiler&, std::vector<Compiler::value>, int)> func, int flags = 0, std::vector<TypeMutator*> mutators = {});

	CallableVersion(const Type* v1_type, const Type* v2_type, const Type* return_type, void* addr, int flags = 0, std::vector<TypeMutator*> mutators = {}, bool v1_addr = false, bool v2_addr = false);
	CallableVersion(const Type* v1_type, const Type* v2_type, const Type* return_type, std::function<Compiler::value(Compiler&, std::vector<Compiler::value>, int)> func, int flags = 0, std::vector<TypeMutator*> mutators = {}, bool v1_addr = false, bool v2_addr = false);

	std::pair<int, const CallableVersion*> get_score(SemanticAnalyzer* analyzer, std::vector<const Type*> arguments) const;
	void apply_mutators(SemanticAnalyzer* analyzer, std::vector<Value*> arguments) const;
	void resolve_templates(SemanticAnalyzer* analyzer, std::vector<const Type*> arguments) const;

	int compile_mutators(Compiler& c, std::vector<Value*> arguments) const;
	Compiler::value compile_call(Compiler& c, std::vector<Compiler::value> args, int flags) const;
};

}

namespace std {
	std::ostream& operator << (std::ostream&, const ls::CallableVersion&);
	std::ostream& operator << (std::ostream&, const ls::CallableVersion*);
}

#endif