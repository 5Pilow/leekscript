#include "VariableValue.hpp"
#include "math.h"
#include "../semantic/SemanticAnalyzer.hpp"
#include "../semantic/FunctionVersion.hpp"
#include "../value/Function.hpp"
#include "../instruction/VariableDeclaration.hpp"
#include "../semantic/Callable.hpp"
#include "../../vm/value/LSString.hpp"
#include "../../vm/value/LSSet.hpp"
#include "../../vm/value/LSClass.hpp"
#include "../../vm/Module.hpp"
#include "../../vm/Program.hpp"
#include "../semantic/Variable.hpp"

namespace ls {

VariableValue::VariableValue(Token* token) : token(token), previous_type(Type::void_) {
	this->name = token->content;
	this->var = nullptr;
	constant = false;
	if (ls_function != nullptr) {
		delete ls_function;
	}
}

bool VariableValue::isLeftValue() const {
	return scope != VarScope::INTERNAL; // Internal variables are not left-value
}

void VariableValue::print(std::ostream& os, int, bool debug, bool condensed) const {
	if (var != nullptr and debug) {
		os << var;
	} else {
		os << token->content;
	}
	if (debug) {
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
	if (name == "~") {
		auto T = Type::template_("T");
		auto R = Type::template_("R");
		auto type = Type::fun(R, {T, Type::fun(R, {T})});
		auto fun = [&](Compiler& c, std::vector<Compiler::value> args, bool) {
			return c.insn_call(args[1], {args[0]});
		};
		return { new CallableVersion { name, type, fun, {}, {R, T} } };
	}
	if (name == "Number") {
		return {
			new CallableVersion { "Number", Type::fun(Type::integer, {}), [&](Compiler& c, std::vector<Compiler::value>, bool) {
				return c.new_integer(0);
			} },
			new CallableVersion { "Number", Type::fun(Type::real, {Type::real}), [&](Compiler& c, std::vector<Compiler::value> args, bool) {
				return c.to_real(args[0]);
			} },
			new CallableVersion { "Number", Type::fun(Type::tmp_mpz_ptr, {Type::mpz_ptr}), [&](Compiler& c, std::vector<Compiler::value> args, bool) {
				return args[0];
			} }
		};
	}
	if (name == "Boolean") {
		auto type = Type::fun(Type::boolean, {});
		return { new CallableVersion { "Boolean", type, [&](Compiler& c, std::vector<Compiler::value>, bool) {
			return c.new_bool(false);
		} } };
	}
	if (name == "String") {
		return {
			new CallableVersion { "String.new", Type::fun(Type::tmp_string, {}) },
			new CallableVersion { "String", Type::fun(Type::tmp_string, {Type::string}), [&](Compiler& c, std::vector<Compiler::value> args, bool) {
				return args[0];
			} }
		};
	}
	if (name == "Array") {
		return { new CallableVersion { "Array", Type::fun(Type::array(Type::any), {}), [&](Compiler& c, std::vector<Compiler::value>, bool) {
			return c.new_array(Type::void_, {});
		} } };
	}
	if (name == "Object") {
		return { new CallableVersion { "Object", Type::fun(Type::tmp_object, {}), [&](Compiler& c, std::vector<Compiler::value>, bool) {
			return c.new_object();
		} } };
	}
	if (name == "Set") {
		return { new CallableVersion { "Set.new", Type::fun(Type::tmp_set(Type::any), {}) } };
	}
	if (type->is_class()) {
		auto type = Type::fun(Type::any, {Type::clazz()});
		return { new Callable {
			{ new CallableVersion { name, type, [&](Compiler& c, std::vector<Compiler::value> args, bool) {
				return c.new_object_class(args[0]);
			} } }
		}, (Value*) this };
	}
	if (var) {
		if (var->call.callable) return var->call;
		if (var->value) {
			Call call { new Callable() };
			auto c = var->value->get_callable(analyzer, argument_count);
			for (const auto& v : c.callable->versions) {
				auto nv = new CallableVersion { *v };
				nv->value = this;
				call.add_version(nv);
			}
			return call;
		}
	} else {
		Call call { new Callable() };
		for (const auto& clazz : analyzer->vm->internal_vars) {
			if (clazz.second->type->is_class()) {
				const auto& cl = (LSClass*) clazz.second->lsvalue;
				auto m = cl->methods.find(name);
				if (m != cl->methods.end()) {
					for (auto& i : m->second.versions) {
						call.add_version(i);
					}
				}
			}
		}
		if (call.callable->versions.size()) {
			return call;
		}
	}
	return {};
}

void VariableValue::pre_analyze(SemanticAnalyzer* analyzer) {
	var = analyzer->get_var(token->content);
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
	if (var != nullptr) {
		if (update_variable) {
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
	} else {
		bool found = false;
		for (const auto& clazz : analyzer->vm->internal_vars) {
			if (clazz.second->type->is_class()) {
				const auto& cl = (LSClass*) clazz.second->lsvalue;
				for (const auto& m : cl->methods) {
					if (m.first == name) {
						type = m.second.versions.at(0)->type;
						found = true;
						for (const auto& i : m.second.versions) {
							versions.insert({i->type->arguments(), clazz.first + "." + name});
						}
						class_method = true;
						break;
					}
				}
				for (const auto& c : cl->static_fields) {
					if (c.first == name) {
						type = c.second.type;
						static_access_function = c.second.static_fun;
						found = true;
						break;
					}
				}
			}
			if (found) break;
		}
		if (!found) {
			type = Type::any;
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
		for (const auto& v : var->call.callable->versions) {
			if (v->type->arguments() == version) {
				return v->type;
			}
		}
	}
	return type;
}

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
	if (var->parent and not var->val.v) {
		// std::cout << "get parent val " << var->parent->val.t << std::endl;
		var->val = var->parent->val;
	}

	Compiler::value v;
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
		return c.new_function(versions.at(version), Type::fun());
	}
	auto f = dynamic_cast<Function*>(var->value);
	if (f) {
		return f->compile_version(c, version);
	}
	if (var->parent and not var->val.v) {
		var->val = var->parent->val;
	}
	return var->get_value(c);
}

Compiler::value VariableValue::compile_l(Compiler& c) const {
	// std::cout << "VV compile_l " << type << " " << var->type << " " << var << std::endl;
	Compiler::value v;
	if (var->parent and not var->val.v) {
		var->val = var->parent->val;
	}
	// No internal values here
	if (scope == VarScope::LOCAL) {
		v = var->get_address(c);
	} else if (scope == VarScope::CAPTURE) {
		if (!var->addr_val.v) {
			std::cout << "No addr_val for variable " << var << "!" << std::endl;
			assert(false);
		}
		v = c.insn_load(var->addr_val);
	} else if (scope == VarScope::INTERNAL) {
		v = c.get_symbol(name, type);
	} else { /* if (scope == VarScope::PARAMETER) */
		v = var->get_address(c);
	}
	return v;
}

std::unique_ptr<Value> VariableValue::clone() const {
	return std::make_unique<VariableValue>(token);
}

}
