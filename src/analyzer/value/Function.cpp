#include "Function.hpp"
#include "../semantic/SemanticAnalyzer.hpp"
#include "../instruction/ExpressionInstruction.hpp"
#include "../Program.hpp"
#include "../../vm/Exception.hpp"
#include <string>
#include <fstream>
#include "../semantic/Callable.hpp"
#include "../instruction/VariableDeclaration.hpp"
#include "../../colors.h"
#include "../Context.hpp"
#include "../../standard/Module.hpp"
#include "../resolver/File.hpp"
#include "../semantic/FunctionVersion.hpp"
#include "../semantic/Variable.hpp"
#include "../../util/Util.hpp"
#include "../../type/Function_type.hpp"
#if COMPILER
#include "llvm/IR/Verifier.h"
#include "../../vm/LSValue.hpp"
#include "../../vm/value/LSFunction.hpp"
#include "../../vm/value/LSClosure.hpp"
#include "../../vm/value/LSNull.hpp"
#endif

namespace ls {

int Function::id_counter = 0;

Function::Function(Environment& env, Token* token) : Value(env), token(token) {
	parent = nullptr;
	constant = true;
	function_added = false;
}

Function::~Function() {
	if (default_version != nullptr) {
		delete default_version;
		default_version = nullptr;
	}
	for (const auto& version : versions) {
		delete version.second;
	}
}

void Function::addArgument(Token* name, Value* defaultValue) {
	arguments.push_back(name);
	defaultValues.emplace_back(defaultValue);
	if (defaultValue) default_values_count++;
}

void Function::print(std::ostream& os, int indent, PrintOptions options) const {
	for (const auto& capture : captures) {
		os << "[" << capture << " = any(" << capture->parent << ")] ";
	}
	if (captures.size() > 0) {
		os << "[";
		for (unsigned c = 0; c < captures.size(); ++c) {
			if (c > 0) os << ", ";
			os << captures[c] << " " << captures[c]->type;
		}
		os << "] ";
	}
	if (versions.size() > 0) {
		// std::cout << "print version " << versions.begin()->second->type << std::endl;
		int i = 0;
		for (const auto& v : versions) {
			if (i++ > 0) os << std::endl << tabs(indent);
			v.second->print(os, indent, options);
		}
	}
	if (default_version and (generate_default_version or is_main_function or versions.size() == 0)) {
		if (versions.size() > 0) os << std::endl << tabs(indent);
		// std::cout << "print default version" << std::endl;
		default_version->print(os, indent, options);
	}
}

Location Function::location() const {
	return default_version->body->location();
}

void Function::create_default_version(SemanticAnalyzer* analyzer) {
	if (default_version) return;
	auto& env = analyzer->env;

	default_version = new FunctionVersion(analyzer->env, std::unique_ptr<Block>(body));
	default_version->parent = this;

	std::vector<const Type*> args;
	for (unsigned int i = 0; i < arguments.size(); ++i) {
		if (defaultValues[i] != nullptr) {
			defaultValues[i]->analyze(analyzer);
		}
		if (!default_version->placeholder_type) {
			default_version->placeholder_type = env.generate_new_placeholder_type();
		}
		args.push_back(default_version->placeholder_type);
	}
	type = Type::fun(env.any, args, this);

	default_version->type = Type::fun(default_version->getReturnType(env), args, this);
}

void Function::pre_analyze(SemanticAnalyzer* analyzer) {
	// std::cout << "Function " << name << "::pre_analyze" << std::endl;
	if (is_main_function) {
		parent = this;
	} else {
		parent = analyzer->current_function()->parent;
	}

	create_default_version(analyzer);

	current_version = default_version;
	default_version->pre_analyze(analyzer, default_version->type->arguments());
}

/*
 * When returing a function, compile a default version with all parameters
 * as pointers, when the function will be in the nature, there will be no
 * problem. Inside a block, it's possible to compile a more specific version
 * of a function in this block to speed up things

let f = x -> x + '!'
let g2 = f('hello')   // default version with pointer
let g1 = f(10)		   // version with [x = int]

let f = x -> y -> x + y
let g = f('hello ')	// default version of f
let h1 = g(10)			// here we act like we don't know where does 'g' come from, and take the default version with pointer
let h2 = g('world')	// same version, pointers

let f = function(a) {
	let g = x -> x + a
	g('hello')	// default version with pointer
	g(12)		// version with [x = int]
	return g	// here, g will go outside his parent, so we take the full-pointer version, regardless of the version of function f. So f can be recompile multiple times, it will not affect the arguments of g and its return type.
}
let r1 = f(' !')		// default version
let r2 = f(12)			// version with number, recompiler f with a [a = int] version, it will not modify the signature of g
r2(12)
r2('hello')
 */
void Function::analyze(SemanticAnalyzer* analyzer) {

	if (!function_added) {
		analyzer->add_function(this);
		function_added = true;
	}
	create_default_version(analyzer);
	if (is_main_function) {
		default_version->analyze_global_functions(analyzer);
	}
	pre_analyze(analyzer);
	if (is_main_function) {
		analyse_default_method(analyzer);
	}
	// std::cout << "Function type: " << type << std::endl;
}

void Function::analyse_default_method(SemanticAnalyzer* analyzer) {
	analyzed = true;
	default_version->analyze(analyzer, type->arguments());
	type = Type::fun_object(default_version->getReturnType(analyzer->env), default_version->type->arguments());
}

void Function::create_version(SemanticAnalyzer* analyzer, const std::vector<const Type*>& args) {
	// std::cout << "Function::create_version(" << args << ")" << std::endl;
	auto version = new FunctionVersion(analyzer->env, unique_static_cast<Block>(default_version->body->clone()));
	version->parent = this;
	versions.insert({args, version});

	version->pre_analyze(analyzer, args);
	version->analyze(analyzer, args);
}

const Type* Function::will_take(SemanticAnalyzer* analyzer, const std::vector<const Type*>& args, int level) {
	// std::cout << "Function " << " ::will_take " << args << " level " << level << std::endl;
	if (level == 1) {
		auto version = args;
		// Fill with default arguments
		for (size_t i = version.size(); i < arguments.size(); ++i) {
			if (defaultValues.at(i)) {
				version.push_back(defaultValues.at(i)->type);
			}
		}
		auto v = versions.find(version);
		if (v == versions.end()) {
			for (const auto& t : version) {
				if (t->placeholder) return type;
			}
			create_version(analyzer, version);
			return versions.at(version)->type;
		}
		return v->second->type;
	} else {
		auto v = current_version ? current_version : default_version;
		if (auto ei = dynamic_cast<ExpressionInstruction*>(v->body->sections[0]->instructions[0].get())) {
			if (auto f = dynamic_cast<Function*>(ei->value.get())) {

				analyzer->enter_function(v);
				auto ret = f->will_take(analyzer, args, level - 1);
				analyzer->leave_function();

				if (captures.size()) {
					v->type = Type::closure(f->version_type(args), v->type->arguments(), this);
				} else {
					v->type = Type::fun(f->version_type(args), v->type->arguments(), this)->pointer();
				}
				return ret;
			}
		}
	}
	return default_version->type;
}

void Function::set_version(SemanticAnalyzer* analyzer, const std::vector<const Type*>& args, int level) {
	// std::cout << "Function::set_version " << args << " " << level << std::endl;
	if (level == 1) {
		for (const auto& t : args) {
			if (t->placeholder) return;
		}
		version = args;
		// Fill with defaultValues
		for (size_t i = version.size(); i < arguments.size(); ++i) {
			if (defaultValues.at(i)) {
				version.push_back(defaultValues.at(i)->type);
			}
		}
		has_version = true;
	} else {
		auto v = current_version ? current_version : default_version;
		if (auto ei = dynamic_cast<ExpressionInstruction*>(v->body->sections[0]->instructions[0].get())) {
			if (auto f = dynamic_cast<Function*>(ei->value.get())) {
				f->set_version(analyzer, args, level - 1);
			}
		}
	}
}

const Type* Function::version_type(std::vector<const Type*> version) const {
	// std::cout << "Function " << this << " ::version_type(" << version << ") " << std::endl;
	// Add default values types if needed
	for (size_t i = version.size(); i < arguments.size(); ++i) {
		if (defaultValues.at(i)) {
			version.push_back(defaultValues.at(i)->type);
		}
	}
	if (versions.find(version) != versions.end()) {
		return versions.at(version)->type;
	}
	return type;
}

void Function::must_return_any(SemanticAnalyzer* analyzer) {
	// std::cout << "Function " << name << " ::must_return_any()" << std::endl;
	generate_default_version = true;
	if (!analyzed) {
		analyse_default_method(analyzer);
	}
}

Call Function::get_callable(SemanticAnalyzer*, int argument_count) const {
	Call call { new Callable() };
	int flags = default_version->body->throws ? Module::THROWS : 0;
	call.add_version(new CallableVersion { "<default>", default_version->type, default_version, {}, {}, false, false, false, false, flags });
	for (const auto& version : versions) {
		int flags = version.second->body->throws ? Module::THROWS : 0;
		call.add_version(new CallableVersion { "<version>", version.second->type, version.second, {}, {}, false, false, false, false, flags });
	}
	return call;
}

#if COMPILER
Compiler::value Function::compile(Compiler& c) const {
	// std::cout << "Function::compile() " << this << " version " << version << " " << has_version << std::endl;
	if (has_version) {
		return compile_version(c, version);
	}
	// std::cout << "Function compile " << generate_default_version << std::endl;
	if (is_main_function or generate_default_version) {
		return default_version->compile(c);
	}
	// Create a null pointer of the correct function type
	// std::cout << "compile null function" << std::endl;
	return c.new_null_pointer(default_version->type->pointer());
}

Compiler::value Function::compile_version(Compiler& c, std::vector<const Type*> args) const {
	// std::cout << "Function " << name << "::compile_version(" << args << ")" << std::endl;
	// Fill with default arguments
	auto full_args = args;
	for (size_t i = version.size(); i < arguments.size(); ++i) {
		if (defaultValues.at(i)) {
			full_args.push_back(defaultValues.at(i)->type);
		}
	}
	if (versions.find(full_args) == versions.end()) {
		std::cout << "/!\\ Version " << args << " not found!" << std::endl;
		assert(false);
	}
	auto version = versions.at(full_args);
	// Compile version if needed
	return version->compile(c);
}

Compiler::value Function::compile_default_version(Compiler& c) const {
	// std::cout << "Function " << name << "::compile_default_version " << std::endl;
	return default_version->compile(c, false);
}

void Function::compile_captures(Compiler& c) const {
	if (captures_compiled) return;
	((Function*) this)->captures_compiled = true;
	// std::cout << "Function::compile_captures" << std::endl;
	for (const auto& capture : captures) {
		if (capture->parent->entry.v) {
			// std::cout << "Convert capture " << capture << " " << (void*) capture << " " << (int)capture->scope << " parent " << capture->parent << " " << capture->parent->val.v << " " << capture->parent->val.t << " " << (void*) capture->parent << std::endl;
			if (capture->parent->type->is_polymorphic()) {
				capture->entry = capture->parent->entry;
				// c.insn_inc_refs(c.insn_load(capture->val));
			} else {
				capture->entry = c.create_entry(capture->name, c.env.any);
				auto value = c.insn_convert(c.insn_load(capture->parent->entry), c.env.any);
				c.insn_inc_refs(value);
				c.insn_store(capture->entry, value);
			}
		}
	}
}

void Function::export_context(Compiler& c) const {
	// int deepness = c.get_current_function_blocks();
	for (const auto& variable : c.fun->body->sections.back()->variables) {
		c.export_context_variable(variable.second->name, c.insn_load(variable.second->entry));
	}
}
#endif

std::unique_ptr<Value> Function::clone() const {
	auto f = std::make_unique<Function>(type->env, token);
	f->lambda = lambda;
	f->name = name;
	f->default_version = new FunctionVersion(type->env, unique_static_cast<Block>(default_version->body->clone()));
	f->default_version->parent = f.get();
	f->default_version->type = default_version->type;
	for (const auto& a : arguments) {
		f->arguments.push_back(a);
	}
	for (const auto& d : defaultValues) {
		if (d != nullptr) {
			f->defaultValues.push_back(d->clone());
		} else {
			f->defaultValues.push_back(nullptr);
		}
	}
	f->default_values_count = default_values_count;
	for (const auto& v : versions) {
		auto v2 = new FunctionVersion(v.second->type->env, unique_static_cast<Block>(v.second->body->clone()));
		v2->parent = f.get();
		v2->type = v.second->type;
		v2->recursive = v.second->recursive;
		f->versions.insert({v.first, v2});
	}
	return f;
}

}
