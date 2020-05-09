#include "Class.hpp"
#include "Callable.hpp"
#include "SemanticAnalyzer.hpp"
#include "../../standard/Module.hpp"
#include "Variable.hpp"
#include "../Program.hpp"
#if COMPILER
#include "../../vm/value/LSFunction.hpp"
#endif

namespace ls {

Class::field::field(std::string name, const Type* type, void* fun, int flags) : name(name), type(type), flags(flags)
#if COMPILER
, native_fun(fun)
#endif
{}

Class::Class(Environment& env, std::string name) : env(env), name(name) {}

Class::~Class() {
	for (auto s : static_fields) {
		#if COMPILER
		if (s.second.value != nullptr) {
			delete s.second.value;
		}
		#endif
	}
	for (auto f : fields) {
		#if COMPILER
		if (f.second.default_value != nullptr) {
			delete f.second.default_value;
		}
		#endif
	}
}

const Call& Class::getOperator(SemanticAnalyzer* analyzer, std::string& name) {
	// std::cout << "getOperator(" << name << ")" << std::endl;
	if (name == "is not") name = "!=";
	else if (name == "÷") name = "/";
	else if (name == "×") name = "*";
	auto o = operators_callables.find(name);
	if (o != operators_callables.end()) {
		return o->second;
	}
	Call call;
	auto i = operators.find(name);
	if (i != operators.end()) {
		call.add_callable(&i->second);
	}
	if (this->name != "Value") {
		auto value_class = analyzer->program->globals["Value"]->clazz;
		auto i = value_class->operators.find(name);
		if (i != value_class->operators.end()) {
			call.add_callable(&i->second);
		}
	}
	// oppa oppa gangnam style tetetorettt tetetorett ! blank pink in the areaaahhh !! bombayah bomm bayah bom bayahh yah yahh yahhh yahh ! bom bom ba BOMBAYAH !!!ya ya ya ya ya ya OPPA !!
	return operators_callables.insert({ name, call }).first->second;
}

void Class::addMethod(std::string name, std::initializer_list<CallableVersionTemplate> impl, std::vector<const Type*> templates, int flags) {
	Callable callable;
	for (const auto& v : impl) {
		if (env.legacy) {
			if (!(flags & Module::LEGACY) and !(v.flags & Module::LEGACY)) continue;
		} else {
			if ((flags & (Module::LEGACY_ONLY - Module::LEGACY)) or (v.flags & (Module::LEGACY_ONLY - Module::LEGACY))) continue;
		}
		callable.add_version(v);
	}
	if (callable.versions.size() == 0) return;
	methods.insert({name, callable});
	int i = 0;
	for (auto& m : methods.at(name).versions) {
		m.name = this->name + "." + name + "." + std::to_string(i++);
		if (templates.size()) {
			m.templates = templates;
		}
		m.flags |= flags;
	}
	// Add first implementation as default method
	#if COMPILER
	auto fun = new LSFunction(impl.begin()->addr);
	auto type = impl.begin()->type;
	static_fields.insert({name, {name, type, fun}});
	#endif
}

#if COMPILER
void Class::addField(std::string name, const Type* type, std::function<Compiler::value(Compiler&, Compiler::value)> fun) {
	if (env.legacy) return;
	fields.insert({name, {name, type, fun, nullptr}});
}
#endif
void Class::addField(std::string name, const Type* type, void* fun) {
	if (env.legacy) return;
	fields.insert({name, {name, type, fun}});
}

void Class::addStaticField(field f) {
	if (env.legacy and !(f.flags & Module::LEGACY)) return;
	if (not env.legacy and (f.flags & (Module::LEGACY_ONLY - Module::LEGACY))) return;
	static_fields.insert({f.name, f});
}

void Class::addOperator(std::string name, std::initializer_list<CallableVersionTemplate> impl, std::vector<const Type*> templates, int flags) {
	Callable callable;
	for (const auto& v : impl) {
		if (env.legacy) {
			if (!(flags & Module::LEGACY) and !(v.flags & Module::LEGACY)) continue;
		} else {
			if ((flags & (Module::LEGACY_ONLY - Module::LEGACY)) or (v.flags & (Module::LEGACY_ONLY - Module::LEGACY))) continue;
		}
		callable.add_version(v);
	}
	if (not callable.versions.size()) return;
	operators.insert({name, callable});
	int i = 0;
	for (auto& m : operators.at(name).versions) {
		m.name = this->name + ".operator" + name + "." + std::to_string(i++);
		if (templates.size()) {
			m.templates = templates;
		}
	}
}

#if COMPILER
LSFunction* Class::getDefaultMethod(const std::string& name) {
	try {
		auto& f = static_fields.at(name);
		f.value->refs++;
		return (LSFunction*) f.value;
	} catch (...) {
		return nullptr;
	}
}
#endif

}