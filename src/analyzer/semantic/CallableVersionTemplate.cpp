#include "CallableVersion.hpp"
#include "../../type/Template_type.hpp"
#include "../../type/Meta_add_type.hpp"
#include "../../type/Meta_mul_type.hpp"
#include "../../type/Meta_baseof_type.hpp"
#include "../../type/Meta_temporary_type.hpp"
#include "../../type/Meta_not_temporary_type.hpp"
#include "../../type/Meta_not_void_type.hpp"
#include "../../standard/Module.hpp"
#include "../../colors.h"
#include "../value/ObjectAccess.hpp"
#include "FunctionVersion.hpp"
#include "../value/Function.hpp"
#include "../../type/Function_type.hpp"
#include "../value/VariableValue.hpp"
#include "Variable.hpp"

namespace ls {

CallableVersionTemplate::CallableVersionTemplate(std::string name, const Type* type, std::vector<const TypeMutator*> mutators, std::vector<const Type*> templates, bool object, bool v1_addr, bool v2_addr, int flags)
	: name(name), type(type), object(object), symbol(true), mutators(mutators), templates(templates), v1_addr(v1_addr), v2_addr(v2_addr), flags(flags)
	{
		assert(name.find(".") != std::string::npos);
	}
#if COMPILER
CallableVersionTemplate::CallableVersionTemplate(std::string name, const Type* type, std::function<Compiler::value(Compiler&, std::vector<Compiler::value>, int)> func, std::vector<const TypeMutator*> mutators, std::vector<const Type*> templates, bool object, bool v1_addr, bool v2_addr, int flags)
	: name(name), type(type), object(object), func(func), mutators(mutators), templates(templates), v1_addr(v1_addr), v2_addr(v2_addr), flags(flags)
	{
		// std::cout << "callable version compiler fun" << std::endl;
	}
#endif
CallableVersionTemplate::CallableVersionTemplate(std::string name, const Type* type, void* addr, std::vector<const TypeMutator*> mutators, std::vector<const Type*> templates, bool object, bool v1_addr, bool v2_addr, int flags)
	: name(name), type(type), object(object), mutators(mutators), templates(templates), v1_addr(v1_addr), v2_addr(v2_addr), flags(flags), addr(addr)
	{}

CallableVersionTemplate::CallableVersionTemplate(std::string name, const Type* type, bool unknown, std::vector<const TypeMutator*> mutators, std::vector<const Type*> templates, bool object, bool v1_addr, bool v2_addr, int flags)
	: name(name), type(type), object(object), mutators(mutators), templates(templates), unknown(unknown), v1_addr(v1_addr), v2_addr(v2_addr), flags(flags), addr(addr)
	{}

CallableVersionTemplate::CallableVersionTemplate(std::string name, const Type* type, FunctionVersion* f, std::vector<const TypeMutator*> mutators, std::vector<const Type*> templates, bool object, bool v1_addr, bool v2_addr, int flags)
	: name(name), type(type), object(object), user_fun(f), mutators(mutators), templates(templates), v1_addr(v1_addr), v2_addr(v2_addr), flags(flags)
	{}

CallableVersionTemplate::CallableVersionTemplate(const Type* return_type, std::initializer_list<const Type*> arguments, void* addr, int flags, std::vector<const TypeMutator*> mutators)
	: type(Type::fun(return_type, arguments)), symbol(true), mutators(mutators), flags(flags), addr(addr)
	{}
#if COMPILER
CallableVersionTemplate::CallableVersionTemplate(const Type* return_type, std::initializer_list<const Type*> arguments, std::function<Compiler::value(Compiler&, std::vector<Compiler::value>, int)> func, int flags, std::vector<const TypeMutator*> mutators)
	: type(Type::fun(return_type, arguments)), func(func), mutators(mutators), flags(flags) {}
#endif

CallableVersionTemplate::CallableVersionTemplate(const Type* v1_type, const Type* v2_type, const Type* return_type, void* addr, int flags, std::vector<const TypeMutator*> mutators, bool v1_addr, bool v2_addr)
	: type(Type::fun(return_type, {v1_type, v2_type})), symbol(true), mutators(mutators), v1_addr(v1_addr), v2_addr(v2_addr), flags(flags), addr(addr)
	{}
#if COMPILER
CallableVersionTemplate::CallableVersionTemplate(const Type* v1_type, const Type* v2_type, const Type* return_type, std::function<Compiler::value(Compiler&, std::vector<Compiler::value>, int)> func, int flags, std::vector<const TypeMutator*> mutators, bool v1_addr, bool v2_addr)
	: type(Type::fun(return_type, {v1_type, v2_type})), func(func), mutators(mutators), v1_addr(v1_addr), v2_addr(v2_addr), flags(flags) {}
#endif

const Type* build(const Type* type) {
	if (type->is_template()) return ((Template_type*) type)->_implementation;
	if (type->is_array()) {
		return type->temporary ? Type::tmp_array(build(type->element())) : Type::array(build(type->element()));
	}
	if (type->is_set()) return Type::set(build(type->element()));
	if (type->is_map()) return Type::map(build(type->key()), build(type->element()));
	if (type->is_function()) {
		std::vector<const Type*> args;
		for (const auto& t : type->arguments()) {
			args.push_back(build(t));
		}
		return Type::fun(build(type->return_type()), args);
	}
	if (type->is_function_object()) {
		std::vector<const Type*> args;
		for (const auto& t : type->arguments()) {
			args.push_back(build(t));
		}
		return Type::fun_object(build(type->return_type()), args);
	}
	if (auto tmp = dynamic_cast<const Meta_temporary_type*>(type)) {
		return build(tmp->type)->add_temporary();
	}
	if (auto not_tmp = dynamic_cast<const Meta_not_temporary_type*>(type)) {
		return build(not_tmp->type)->not_temporary();
	}
	if (auto not_void = dynamic_cast<const Meta_not_void_type*>(type)) {
		auto t = build(not_void->type);
		if (t->is_void()) {
			return t->env.null;
		} else {
			return t;
		}
	}
	if (auto mul = dynamic_cast<const Meta_add_type*>(type)) {
		return build(mul->t1)->operator + (build(mul->t2));
	}
	if (auto mul = dynamic_cast<const Meta_mul_type*>(type)) {
		return build(mul->t1)->operator * (build(mul->t2));
	}
	if (auto baseof = dynamic_cast<const Meta_baseof_type*>(type)) {
		if (baseof->result) return baseof->result;
		auto t = build(baseof->type);
		auto d = t->distance(baseof->base);
		if (d == -1) {
			return ((Meta_baseof_type*) baseof)->result = baseof->base;
		} else if (d < 100) {
			return ((Meta_baseof_type*) baseof)->result = t;
		} else {
			return ((Meta_baseof_type*) baseof)->result = [&]() {
				if (t == t->env.boolean) return t->env.integer;
				return t->env.real;
			}();
		}
	}
	return type;
}

std::pair<int, CallableVersion> CallableVersionTemplate::get_score(SemanticAnalyzer* analyzer, std::vector<const Type*> arguments, Callable* callable, size_t index) const {
	// std::cout << "CallableVersion::get_score(" << arguments << ") " << type << std::endl;

	// Template resolution
	CallableVersion new_version { analyzer->env, callable, index, type };
	if (templates.size()) {
		resolve_templates(analyzer, arguments);
		auto version_type = build(type);
		// Reset template implementations
		for (const auto& t : templates) {
			t->reset();
		}
		// std::cout << "Resolved version = " << version_type << std::endl;
		new_version.type = version_type;
	}

	auto f = type->function();
	bool valid = true;
	for (size_t i = 0; i < new_version.type->arguments().size(); ++i) {
		if (i < arguments.size()) {
			const auto& a = arguments.at(i);
			const auto implem_arg = new_version.type->argument(i);
			// std::cout << "argument " << i << " " << implem_arg << std::endl;
			if (a->function() and (implem_arg->is_function() or implem_arg->is_function_pointer() or implem_arg->is_function_object())) {
				arguments.at(i) = ((Value*) a->function())->will_take(analyzer, implem_arg->arguments(), 1);
			}
		} else if (f and ((Function*) f)->defaultValues.at(i)) {
			// Default argument
		} else {
			valid = false;
		}
	}
	if ((!valid or arguments.size() > new_version.type->arguments().size()) and not unknown) {
		return { std::numeric_limits<int>::max(), { analyzer->env } };
	}
	int d = 0;
	if (!unknown) {
		for (size_t i = 0; i < new_version.type->arguments().size(); ++i) {
			auto type = [&]() { if (i < arguments.size()) {
				return arguments.at(i);
			} else if (f and ((Function*) f)->defaultValues.at(i)) {
				return ((Function*) f)->defaultValues.at(i)->type;
			} else {
				assert(false);
			}}();
			auto di = type->distance(new_version.type->arguments().at(i));
			// std::cout << type << " distance to " << new_version.type->arguments().at(i) << " " << di << std::endl;
			if (di < 0) return { std::numeric_limits<int>::max(), { analyzer->env } };
			d += di;
		}
	}
	return { d, new_version };
}

void solve(SemanticAnalyzer* analyzer, const Type* t1, const Type* t2) {
	// std::cout << "Solve " << t1 << " ||| " << t2 << std::endl;
	if (t1->is_template()) {
		t1->implement(t2);
	}
	else if (auto baseof = dynamic_cast<const Meta_baseof_type*>(t1)) {
		solve(analyzer, baseof->type, t2);
	}
	else if (auto not_void = dynamic_cast<const Meta_not_void_type*>(t1)) {
		// std::cout << "not_void " << not_void->type << std::endl;
		solve(analyzer, not_void->type, t2);
	}
	else if (t1->is_array() and t2->is_array()) {
		solve(analyzer, t1->element(), t2->element());
	}
	else if (t1->is_set() and t2->is_set()) {
		solve(analyzer, t1->element(), t2->element());
	}
	else if (t1->is_map() and t2->is_map()) {
		solve(analyzer, t1->key(), t2->key());
		solve(analyzer, t1->element(), t2->element());
	}
	else if ((t1->is_function() or t1->is_function_pointer() or t1->is_function_object()) and (t2->is_function() or t2->is_function_pointer() or t2->is_function_object())) {
		// std::cout << "solve function " << t2 << std::endl;
		auto f = t2->function();
		if (f) {
			auto t1_args = build(t1)->arguments();
			auto type = ((Value*) f)->will_take(analyzer, t1_args, 1);
			solve(analyzer, t1->return_type(), type->return_type());
		}
	}
}

void CallableVersionTemplate::resolve_templates(SemanticAnalyzer* analyzer, std::vector<const Type*> arguments) const {
	// std::cout << "CallableVersion::resolve_templates(" << arguments << ")" << std::endl;
	// First passage to solve easy types
	for (size_t i = 0; i < arguments.size(); ++i) {
		const auto& t1 = type->argument(i);
		if (t1->is_template()) {
			const auto& t2 = arguments.at(i);
			// std::cout << t1 << " <=> " << t2 << std::endl;
			solve(analyzer, t1, t2);
		}
	}
	for (size_t i = 0; i < arguments.size(); ++i) {
		const auto& t1 = type->argument(i);
		if (not t1->is_template()) {
			const auto& t2 = arguments.at(i);
			solve(analyzer, t1, t2);
		}
	}
}

}

namespace std {
	std::ostream& operator << (std::ostream& os, const ls::CallableVersionTemplate& v) {
		if (v.templates.size()) {
			os << "template<";
			for (size_t i = 0; i < v.templates.size(); ++i) {
				if (i != 0) std::cout << ", ";
				std::cout << v.templates.at(i);
			}
			os << "> ";
		}
		os << v.name << " ";
		// if (v.object) os << "★ " << v.object << ":" << v.object->type << " ";
		os << v.type->arguments() << BLUE_BOLD << " => " << END_COLOR << v.type->return_type();
		if (v.user_fun) {
			os << " (user func " << v.user_fun << ")";
		} else if (v.symbol) {
			os << " (symbol " << v.name << ")";
		#if COMPILER
		} else if (v.func) {
			os << " (compiler func)";
		#endif
		}
		if (v.unknown) os << " (unknown)";
		if (v.flags) os << " " << v.flags;
		return os;
	}
	std::ostream& operator << (std::ostream& os, const ls::CallableVersionTemplate* v) {
		os << *v;
		return os;
	}
}