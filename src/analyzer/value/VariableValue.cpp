#include "VariableValue.hpp"
#include "math.h"
#include "../semantic/SemanticAnalyzer.hpp"
#include "../semantic/FunctionVersion.hpp"
#include "../value/Function.hpp"
#include "../instruction/VariableDeclaration.hpp"
#include "../semantic/Callable.hpp"
#include "../../standard/Module.hpp"
#include "../Program.hpp"
#include "../semantic/Variable.hpp"

namespace ls {

VariableValue::VariableValue(Environment& env, Token* token) : LeftValue(env), token(token) {
	this->name = token->content;
	this->var = nullptr;
	constant = false;
	#if COMPILER
	if (ls_function != nullptr) {
		delete ls_function;
	}
	#endif
}

bool VariableValue::isLeftValue() const {
	return scope != VarScope::INTERNAL
	#if COMPILER
	and (not var or not var->val.v) // Internal variables are not left-value
	#endif
	;
}

void VariableValue::print(std::ostream& os, int, PrintOptions options) const {
	if (var != nullptr and options.debug) {
		// os << var;
	} else {
		os << token->content;
	}
	if (options.debug) {
		os << " ";
		if (has_version && var != nullptr && var->value != nullptr)
			os << var->value->version_type(version);
		else
			os << type;
		if (has_version) os << " " << version;
	}
}

Location VariableValue::location() const {
	return token->location;
}

Call VariableValue::get_callable(SemanticAnalyzer* analyzer, int argument_count) const {
	auto& env = analyzer->env;
	if (name == "~") {
		auto T = env.template_("T");
		auto R = env.template_("R");
		auto type = Type::fun(R, {T, Type::fun(R, {T})});
		auto fun = ADDR([&](Compiler& c, std::vector<Compiler::value> args, bool) {
			return c.insn_call(args[1], {args[0]});
		});
		return { { name, type, fun, {}, {R, T} } };
	}
	if (name == "Number") {
		return {
			{ "Number", Type::fun(env.integer, {}), ADDR([&](Compiler& c, std::vector<Compiler::value>, bool) {
				return c.new_integer(0);
			}) },
			{ "Number", Type::fun(env.real, {env.real}), ADDR([&](Compiler& c, std::vector<Compiler::value> args, bool) {
				return c.to_real(args[0]);
			}) },
			{ "Number", Type::fun(env.tmp_mpz_ptr, {env.mpz_ptr}), ADDR([&](Compiler& c, std::vector<Compiler::value> args, bool) {
				return args[0];
			}) }
		};
	}
	if (name == "Boolean") {
		auto type = Type::fun(env.boolean, {});
		return { { "Boolean", type, ADDR([&](Compiler& c, std::vector<Compiler::value>, bool) {
			return c.new_bool(false);
		}) } };
	}
	if (name == "String") {
		return {
			{ "String.new", Type::fun(env.tmp_string, {}) },
			{ "String", Type::fun(env.tmp_string, {env.string}), ADDR([&](Compiler& c, std::vector<Compiler::value> args, bool) {
				return args[0];
			}) }
		};
	}
	if (name == "Array") {
		return { { "Array", Type::fun(Type::array(env.any), {}), ADDR([&](Compiler& c, std::vector<Compiler::value>, bool) {
			return c.new_array(env.void_, {});
		}) } };
	}
	if (name == "Object") {
		return { { "Object", Type::fun(env.tmp_object, {}), ADDR([&](Compiler& c, std::vector<Compiler::value>, bool) {
			return c.new_object();
		}) } };
	}
	if (name == "Set") {
		return { { "Set.new", Type::fun(Type::tmp_set(env.any), {}) } };
	}
	if (type->is_class()) {
		auto type = Type::fun(env.any, {env.clazz()});
		return { {
			{ name, type, ADDR([&](Compiler& c, std::vector<Compiler::value> args, bool) {
				return c.new_object_class(args[0]);
			}) }
		}, (Value*) this };
	}
	if (var) {
		if (var->call.callables.size()) return var->call;
		if (var->value) {
			Call call;
			auto c = var->value->get_callable(analyzer, argument_count);
			for (const auto& callable : c.callables) {
				auto callable2 = new Callable();
				for (const auto& v : callable->versions) {
					auto nv = v;
					nv.value = this;
					callable2->add_version(nv);
				}
				call.add_callable(callable2);
			}
			return call;
		}
	} else {
		Call call;
		for (const auto& variable : analyzer->globals) {
			if (variable.second->type->is_class()) {
				const auto& cl = variable.second->clazz;
				auto m = cl->methods.find(name);
				if (m != cl->methods.end()) {
					call.add_callable(&m->second);
				}
			}
		}
		return call;
	}
	return {};
}

void VariableValue::pre_analyze(SemanticAnalyzer* analyzer) {
	var = analyzer->get_var(token->content);
	// if (var == nullptr) {
		// std::cout << "var [" << token->content << "] not found in " << analyzer->current_section()->name << std::endl;
	// }
	if (var != nullptr) {
		// std::cout << "VV pre_analyze variable = " << var << std::endl;
		// std::cout << "pre analyze var " << var->name << " " << (void*) var->function << " <=> " << (void*) analyzer->current_function() << " " << (int) var->scope << std::endl;
		if (var->scope != VarScope::INTERNAL and var->function != analyzer->current_function()) {
			if (not var->type->is_function()) {
				// std::cout << "VV " << var << " capture " << std::endl;
				var = analyzer->current_function()->capture(analyzer, var);
			}
		}
	}
}

void VariableValue::analyze(SemanticAnalyzer* analyzer) {
	const auto& env = analyzer->env;
	if (var != nullptr) {
		if (update_variable and var->parent) {
			var->type = var->parent->type;
			var->value = var->parent->value;
		}
		auto function_object = dynamic_cast<Function*>(var->value);
		if (var->value && function_object) {
			// Analyse the real function (if the function is defined below its call for example)
			if (!function_object->analyzed) {
				function_object->analyze(analyzer);
			}
		}
		type = var->type;
		scope = var->scope;
		// std::cout << "var " << var << " " << (void*) var << " " << var->type << std::endl;
	} else {
		bool found = false;
		for (const auto& variable : analyzer->globals) {
			if (variable.second->type->is_class()) {
				const auto& cl = variable.second->clazz;
				for (const auto& m : cl->methods) {
					if (m.first == name) {
						type = m.second.versions.at(0).type;
						found = true;
						for (const auto& i : m.second.versions) {
							versions.insert({ i.type->arguments(), variable.first + "." + name });
						}
						class_method = true;
						break;
					}
				}
				for (const auto& c : cl->static_fields) {
					if (c.first == name) {
						type = c.second.type;
						#if COMPILER
						static_access_function = c.second.static_fun;
						#endif
						found = true;
						break;
					}
				}
			}
			if (found) break;
		}
		if (!found) {
			type = env.any;
			analyzer->add_error({Error::Type::UNDEFINED_VARIABLE, token->location, token->location, {token->content}});
		}
	}
	type = type->not_temporary();

	// if (var) std::cout << "VV " << var << " : " << type << " " << (int)var->scope << std::endl;
}

const Type* VariableValue::will_take(SemanticAnalyzer* analyzer, const std::vector<const Type*>& args, int level) {
	// std::cout << "VV will take " << args << " type " << type << std::endl;
	if (var != nullptr and var->value != nullptr) {
		auto ret = var->value->will_take(analyzer, args, level);
		if (auto f = dynamic_cast<Function*>(var->value)) {
			if (f->versions.find(args) != f->versions.end()) {
				var->version = args;
			}
		}
		type = var->type;
		return ret;
	}
	set_version(analyzer, args, level);
	return type;
}

void VariableValue::set_version(SemanticAnalyzer* analyzer, const std::vector<const Type*>& args, int level) {
	// std::cout << "VariableValue::set_version " << args << " " << level << std::endl;
	if (var != nullptr and var->value != nullptr) {
		var->value->set_version(analyzer, args, level);
	}
	if (level == 1) {
		version = args;
		has_version = true;
	}
}

bool VariableValue::will_store(SemanticAnalyzer* analyzer, const Type* type) {
	// std::cout << "VV will_store " << type << std::endl;
	if (var != nullptr and var->value != nullptr) {
		var->value->will_store(analyzer, type);
		this->type = var->type;
	}
	return false;
}

bool VariableValue::elements_will_store(SemanticAnalyzer* analyzer, const Type* type, int level) {
	if (var != nullptr and var->value != nullptr) {
		var->value->elements_will_store(analyzer, type, level);
		this->type = var->type->not_temporary();
	}
	return false;
}

void VariableValue::change_value(SemanticAnalyzer*, Value* value) {}

const Type* VariableValue::version_type(std::vector<const Type*> version) const {
	// std::cout << "VariableValue::version_type " << version << std::endl;
	if (var != nullptr && var->value != nullptr) {
		// std::cout << "VariableValue " << this << " version_type() " << version << std::endl;
		return var->value->version_type(version);
	}
	if (var) {
		for (const auto& callable : var->call.callables) {
			for (const auto& v : callable->versions) {
				if (v.type->arguments() == version) {
					return v.type;
				}
			}
		}
	}
	return type;
}

#if COMPILER
Compiler::value VariableValue::compile(Compiler& c) const {
	// std::cout << "Compile var " << name << " " << type << std::endl;
	// std::cout << "Compile vv " << name << " : " << type << "(" << (int) scope << ")" << std::endl;

	if (static_access_function != nullptr) {
		return static_access_function(c);
	}
	if (class_method) {
		const auto& fun = has_version and versions.find(version) != versions.end() ? versions.at(version) : default_version_fun;
		return c.new_function(fun, type);
	}
	if (var and var->parent and not var->entry.v) {
		// std::cout << "get parent val " << var->parent->entry.t << std::endl;
		var->entry = var->parent->entry;
	}

	Compiler::value v { c.env };
	if (scope == VarScope::CAPTURE) {
		v = var->get_value(c);
	} else if (scope == VarScope::INTERNAL) {
		auto f = dynamic_cast<Function*>(var->value);
		if (f) {
			if (has_version) {
				return f->compile_version(c, version);
			} else {
				return f->compile_default_version(c);
			}
		}
		v = c.get_symbol(name, type);
	} else if (scope == VarScope::LOCAL) {
		assert(var != nullptr);
		auto f = dynamic_cast<Function*>(var->value);
		auto vv = dynamic_cast<VariableValue*>(var->value);
		if (f) {
			if (has_version) {
				return f->compile_version(c, version);
			} else if (f->versions.size() == 1) {
				return f->compile(c);
			} else {
				return f->compile_default_version(c);
			}
		}
		if (vv && has_version) {
			return var->value->compile_version(c, version);
		}
		if (not is_void) {
			v = var->get_value(c);
		}
	} else if (scope == VarScope::PARAMETER) {
		// std::cout << "compile argument " << var << std::endl;
		if (not is_void) {
			v = var->get_value(c);
		}
	} else {
		assert(false);
	}
	assert(c.check_value(v));
	return v;
}

Compiler::value VariableValue::compile_version(Compiler& c, std::vector<const Type*> version) const {
	// std::cout << "VV compile_version " << var << std::endl;
	if (class_method) {
		return c.new_function(versions.at(version), Type::fun(c.env.void_, {}));
	}
	auto f = dynamic_cast<Function*>(var->value);
	if (f) {
		return f->compile_version(c, version);
	}
	if (var->parent and not var->entry.v) {
		var->entry = var->parent->entry;
	}
	return var->get_value(c);
}

Compiler::value VariableValue::compile_l(Compiler& c) const {
	// std::cout << "VV compile_l " << type << " " << var->type << " " << var << std::endl;
	Compiler::value v { c.env };
	if (var->parent and not var->entry.v) {
		// std::cout << "set entry from parent " << std::endl;
		var->entry = var->parent->entry;
	}
	// No internal values here
	if (scope == VarScope::LOCAL) {
		// std::cout << "VV " << var->name << " get_address " << var->entry.v << std::endl;
		// std::cout << "parent 1 " << var->parent->val.v << std::endl;
		// std::cout << "parent 2 " << var->parent->parent->val.v << std::endl;
		// std::cout << "parent 3 " << var->parent->parent->parent->val.v << std::endl;
		v = var->get_address(c);
	} else if (scope == VarScope::CAPTURE) {
		v = var->entry;
		// std::cout << "capture " << var << " " << (void*) var << " entry " << (void*) v.v << std::endl;
		// if (!var->addr_val.v) {
		// 	// std::cout << "No addr_val for variable " << var << "!" << std::endl;
		// 	assert(false);
		// }
		// v = c.insn_load(var->addr_val);
	} else if (scope == VarScope::INTERNAL) {
		v = c.get_symbol(name, type);
	} else { /* if (scope == VarScope::PARAMETER) */
		v = var->get_address(c);
	}
	return v;
}
#endif

std::unique_ptr<Value> VariableValue::clone(Block* parent) const {
	return std::make_unique<VariableValue>(type->env, token);
}

}
