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

#if COMPILER
CallableVersion::CallableVersion(Environment& env) : type(env.void_), extra_arg(env) {}

CallableVersion::CallableVersion(Environment& env, const Callable* callable, size_t index, const Type* type) : callable(callable), index(index), type(type), extra_arg(env) {}
#else
CallableVersion::CallableVersion() {}
#endif

CallableVersion::operator bool() const {
	return callable;
}

const CallableVersionTemplate* CallableVersion::template_() const {
	assert(callable);
	return &callable->versions[index];
}

void CallableVersion::apply_mutators(SemanticAnalyzer* analyzer, std::vector<Value*> values) const {
	// std::cout << "CallableVersion::apply_mutators() mutators : " << mutators.size() << std::endl;
	for (const auto& mutator : template_()->mutators) {
		mutator->apply(analyzer, values, type->return_type());
	}
}

#if COMPILER
int CallableVersion::compile_mutators(Compiler& c, std::vector<Value*> values) const {
	int flags = 0;
	for (const auto& mutator : template_()->mutators) {
		flags |= mutator->compile(c, (CallableVersion*) this, values);
	}
	return flags;
}

Compiler::value CallableVersion::compile_call(Compiler& c, std::vector<Compiler::value> args, const Value* value, int flags) const {
	// std::cout << "CallableVersion::compile_call(" << args;
	// if (value) std::cout << ", value=\"" << value << "\"";
	// std::cout << ")" << std::endl;
	// Do the call
	auto full_flags = template_()->flags | flags;
	if (extra_arg.v) {
		args.push_back(extra_arg);
	}
	auto r = [&]() { if (template_()->user_fun) {
		// std::cout << "Compile CallableVersion user_fun " << user_fun->type << std::endl;
		template_()->user_fun->compile(c);
		if (template_()->user_fun->type->is_closure() or template_()->unknown) {
			args.insert(args.begin(), template_()->user_fun->value);
		}
		if (full_flags & Module::THROWS) {
			return c.insn_invoke(type->return_type(), args, template_()->user_fun->fun);
		} else {
			return c.insn_call(template_()->user_fun->fun, args);
		}
	} else if (template_()->symbol) {
		if (full_flags & Module::THROWS) {
			return c.insn_invoke(type->return_type(), args, template_()->name);
		} else {
			return c.insn_call(type->return_type(), args, template_()->name);
		}
	} else if (template_()->func) {
		return template_()->func(c, args, full_flags);
	} else if (value) {
		auto fun = [&]() { if (template_()->object) {
			auto oa = dynamic_cast<const ObjectAccess*>(value);
			auto k = c.new_const_string(oa->field->content);
			return c.insn_invoke(type->pointer(), {c.get_vm(), args[0], k}, "Value.attr");
		} else {
			return value->compile(c);
		}}();
		if (template_()->unknown) {
			args.insert(args.begin(), fun);
		}
		auto r = [&]() { if (template_()->unknown) {
			if (fun.t->is_closure()) {
				args.insert(args.begin(), fun);
			}
			return c.insn_call(c.env.any, args, "Function.call");
		} else {
			if (full_flags & Module::THROWS) {
				return c.insn_invoke(type->return_type(), args, fun);
			} else {
				return c.insn_call(fun, args);
			}
		}}();
		if (!template_()->object) {
			value->compile_end(c);
		}
		return r;
	} else {
		assert(false);
	}}();
	return r;
}
#endif

}

namespace std {
	std::ostream& operator << (std::ostream& os, const ls::CallableVersion& v) {
		os << v.template_()->name << " ";
		// if (v.object) os << "★ " << v.object << ":" << v.object->type << " ";
		os << v.type->arguments() << BLUE_BOLD << " => " << END_COLOR << v.type->return_type();
		if (v.template_()->user_fun) {
			os << " (user func " << v.template_()->user_fun << ")";
		} else if (v.template_()->symbol) {
			os << " (symbol " << v.template_()->name << ")";
		#if COMPILER
		} else if (v.template_()->func) {
			os << " (compiler func)";
		#endif
		}
		if (v.template_()->unknown) os << " (unknown)";
		if (v.template_()->flags) os << " " << v.template_()->flags;
		return os;
	}
	std::ostream& operator << (std::ostream& os, const ls::CallableVersion* v) {
		os << *v;
		return os;
	}
}