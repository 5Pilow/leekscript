#include "CallableVersion.hpp"
#include "../../type/Template_type.hpp"
#include "../../type/Meta_add_type.hpp"
#include "../../type/Meta_mul_type.hpp"
#include "../../type/Meta_baseof_type.hpp"
#include "../../type/Meta_not_temporary_type.hpp"
#include "../../vm/Module.hpp"
#include "../../colors.h"
#include "../value/ObjectAccess.hpp"
#include "FunctionVersion.hpp"
#include "../value/Function.hpp"
#include "../../type/Function_type.hpp"
#include "../value/VariableValue.hpp"
#include "Variable.hpp"

namespace ls {

CallableVersion::CallableVersion(std::string name, const Type* type, std::vector<TypeMutator*> mutators, std::vector<const Type*> templates, bool object, bool unknown, bool v1_addr, bool v2_addr, int flags)
	: name(name), type(type), object(object), symbol(true), mutators(mutators), templates(templates), unknown(unknown), v1_addr(v1_addr), v2_addr(v2_addr), flags(flags) {
		assert(name.find(".") != std::string::npos);
	}
CallableVersion::CallableVersion(std::string name, const Type* type, std::function<Compiler::value(Compiler&, std::vector<Compiler::value>, int)> func, std::vector<TypeMutator*> mutators, std::vector<const Type*> templates, bool object, bool unknown, bool v1_addr, bool v2_addr, int flags)
	: name(name), type(type), object(object), func(func), mutators(mutators), templates(templates), unknown(unknown), v1_addr(v1_addr), v2_addr(v2_addr), flags(flags) {}
CallableVersion::CallableVersion(std::string name, const Type* type, const Value* value, std::vector<TypeMutator*> mutators, std::vector<const Type*> templates, bool object, bool unknown, bool v1_addr, bool v2_addr, int flags)
	: name(name), type(type), object(object), value(value), mutators(mutators), templates(templates), unknown(unknown), v1_addr(v1_addr), v2_addr(v2_addr), flags(flags) {}
CallableVersion::CallableVersion(std::string name, const Type* type, FunctionVersion* f, std::vector<TypeMutator*> mutators, std::vector<const Type*> templates, bool object, bool unknown, bool v1_addr, bool v2_addr, int flags)
	: name(name), type(type), object(object), user_fun(f), mutators(mutators), templates(templates), unknown(unknown), v1_addr(v1_addr), v2_addr(v2_addr), flags(flags) {}

CallableVersion::CallableVersion(const Type* return_type, std::initializer_list<const Type*> arguments, void* addr, int flags, std::vector<TypeMutator*> mutators)
	: type(Type::fun(return_type, arguments)), symbol(true), mutators(mutators), flags(flags), addr(addr) {}
CallableVersion::CallableVersion(const Type* return_type, std::initializer_list<const Type*> arguments, std::function<Compiler::value(Compiler&, std::vector<Compiler::value>, int)> func, int flags, std::vector<TypeMutator*> mutators)
	: type(Type::fun(return_type, arguments)), func(func), mutators(mutators), flags(flags) {}

CallableVersion::CallableVersion(const Type* v1_type, const Type* v2_type, const Type* return_type, void* addr, int flags, std::vector<TypeMutator*> mutators, bool v1_addr, bool v2_addr)
	: type(Type::fun(return_type, {v1_type, v2_type})), symbol(true), mutators(mutators), v1_addr(v1_addr), v2_addr(v2_addr), flags(flags), addr(addr) {}
CallableVersion::CallableVersion(const Type* v1_type, const Type* v2_type, const Type* return_type, std::function<Compiler::value(Compiler&, std::vector<Compiler::value>, int)> func, int flags, std::vector<TypeMutator*> mutators, bool v1_addr, bool v2_addr)
	: type(Type::fun(return_type, {v1_type, v2_type})), func(func), mutators(mutators), v1_addr(v1_addr), v2_addr(v2_addr), flags(flags) {}

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
	if (auto not_tmp = dynamic_cast<const Meta_not_temporary_type*>(type)) {
		return build(not_tmp->type)->not_temporary();
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
				if (t == Type::boolean) return Type::integer;
				return Type::real;
			}();
		}
	}
	return type;
}

std::pair<int, const CallableVersion*> CallableVersion::get_score(SemanticAnalyzer* analyzer, std::vector<const Type*> arguments) const {
	// std::cout << "CallableVersion::get_score(" << arguments << ") " << type << std::endl;
	
	// Template resolution
	const CallableVersion* new_version = this;
	if (templates.size()) {
		resolve_templates(analyzer, arguments);
		auto version_type = build(type);
		// Reset template implementations
		for (const auto& t : templates) {
			t->reset();
		}
		new_version = new CallableVersion(*this);
		((CallableVersion*) new_version)->templates = {};
		((CallableVersion*) new_version)->type = version_type;
		// std::cout << "Resolved version = " << version_type << std::endl;
	}

	auto f = type->function();
	bool valid = true;
	for (size_t i = 0; i < new_version->type->arguments().size(); ++i) {
		if (i < arguments.size()) {
			const auto& a = arguments.at(i);
			const auto implem_arg = new_version->type->argument(i);
			// std::cout << "argument " << i << " " << implem_arg << std::endl;
			if (a->function() and (implem_arg->is_function() or implem_arg->is_function_pointer() or implem_arg->is_function_object())) {
				arguments.at(i) = ((Function*) a->function())->will_take(analyzer, implem_arg->arguments(), 1);
			}
		} else if (f and ((Function*) f)->defaultValues.at(i)) {
			// Default argument
		} else {
			valid = false;
		}
	}
	if ((!valid or arguments.size() > new_version->type->arguments().size()) and not unknown) {
		return { std::numeric_limits<int>::max(), nullptr };
	}
	int d = 0;
	if (!unknown) {
		for (size_t i = 0; i < new_version->type->arguments().size(); ++i) {
			auto type = [&]() { if (i < arguments.size()) {
				return arguments.at(i);
			} else if (f and ((Function*) f)->defaultValues.at(i)) {
				return ((Function*) f)->defaultValues.at(i)->type;
			} else {
				assert(false);
			}}();
			auto di = type->distance(new_version->type->arguments().at(i));
			// std::cout << type << " distance " << new_version->type->arguments().at(i) << " " << di << std::endl;
			if (di < 0) return { std::numeric_limits<int>::max(), nullptr };
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
			auto type = ((Function*) f)->will_take(analyzer, t1_args, 1);
			solve(analyzer, t1->return_type(), type->return_type());
		}
	}
}

void CallableVersion::resolve_templates(SemanticAnalyzer* analyzer, std::vector<const Type*> arguments) const {
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
			// std::cout << t1 << " <=> " << t2 << std::endl;
			solve(analyzer, t1, t2);
		}
	}
}

void CallableVersion::apply_mutators(SemanticAnalyzer* analyzer, std::vector<Value*> values) const {
	// std::cout << "CallableVersion::apply_mutators() mutators : " << mutators.size() << std::endl;
	for (const auto& mutator : mutators) {
		mutator->apply(analyzer, values, type->return_type());
	}
}

int CallableVersion::compile_mutators(Compiler& c, std::vector<Value*> values) const {
	int flags = 0;
	for (const auto& mutator : mutators) {
		flags |= mutator->compile(c, (CallableVersion*) this, values);
	}
	return flags;
}

Compiler::value CallableVersion::compile_call(Compiler& c, std::vector<Compiler::value> args, int flags) const {
	// std::cout << "CallableVersion::compile_call(" << args << ")" << std::endl;
	// Do the call
	auto full_flags = this->flags | flags;
	if (extra_arg.v) {
		args.push_back(extra_arg);
	}
	auto r = [&]() { if (user_fun) {
		// std::cout << "Compile CallableVersion user_fun " << user_fun->type << std::endl;
		user_fun->compile(c);
		if (user_fun->type->is_closure() or unknown) {
			args.insert(args.begin(), user_fun->value);
		}
		if (full_flags & Module::THROWS) {
			return c.insn_invoke(type->return_type(), args, user_fun->fun);
		} else {
			return c.insn_call(user_fun->fun, args);
		}
	} else if (symbol) {
		if (full_flags & Module::THROWS) {
			return c.insn_invoke(type->return_type(), args, name);
		} else {
			return c.insn_call(type->return_type(), args, name);
		}
	} else if (func) {
		return func(c, args, full_flags);
	} else if (value) {
		auto fun = [&]() { if (object) {
			auto oa = dynamic_cast<const ObjectAccess*>(value);
			auto k = c.new_const_string(oa->field->content);
			return c.insn_invoke(type->pointer(), {args[0], k}, "Value.attr");
		} else {
			return value->compile(c);
		}}();
		if (unknown) {
			args.insert(args.begin(), fun);
		}
		auto r = [&]() { if (unknown) {
			if (fun.t->is_closure()) {
				args.insert(args.begin(), fun);
			}
			return c.insn_call(Type::any, args, "Function.call");
		} else {
			if (full_flags & Module::THROWS) {
				return c.insn_invoke(type->return_type(), args, fun);
			} else {
				return c.insn_call(fun, args);
			}
		}}();
		if (!object) {
			value->compile_end(c);
		}
		return r;
	} else {
		assert(false);
	}}();
	return r;
}

}

namespace std {
	std::ostream& operator << (std::ostream& os, const ls::CallableVersion& v) {
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
		} else if (v.func) {
			os << " (compiler func)";
		} else {
			os << " (value)";
		}
		if (v.unknown) os << " (unknown)";
		if (v.flags) os << " " << v.flags;
		return os;
	}
	std::ostream& operator << (std::ostream& os, const ls::CallableVersion* v) {
		os << *v;
		return os;
	}
}