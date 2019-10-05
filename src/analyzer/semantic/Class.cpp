#include "Class.hpp"
#include "Callable.hpp"
#include "SemanticAnalyzer.hpp"
#include "../../standard/Module.hpp"
#include "Variable.hpp"
#if COMPILER
#include "../../vm/value/LSFunction.hpp"
#endif

namespace ls {

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

const Callable* Class::getOperator(SemanticAnalyzer* analyzer, std::string& name) {
	// std::cout << "getOperator(" << name << ")" << std::endl;
	if (name == "is not") name = "!=";
	else if (name == "÷") name = "/";
	else if (name == "×") name = "*";
	auto o = operators_callables.find(name);
	if (o != operators_callables.end()) {
		return o->second;
	}
	auto callable = new Callable();
	auto i = operators.find(name);
	if (i != operators.end()) {
		for (const auto& impl : i->second) {
			callable->add_version(&impl);
		}
	}
	if (this->name != "Value") {
		auto value_class = analyzer->globals["Value"]->clazz;
		auto i = value_class->operators.find(name);
		if (i != value_class->operators.end()) {
			for (const auto& impl : i->second) {
				callable->add_version(&impl);
			}
		}
	}
	if (callable->versions.size()) {
		operators_callables.insert({ name, callable });
		// oppa oppa gangnam style tetetorettt tetetorett ! blank pink in the areaaahhh !! bombayah bomm bayah bom bayahh yah yahh yahhh yahh ! bom bom ba BOMBAYAH !!!ya ya ya ya ya ya OPPA !!
		return callable;
	}
	return nullptr;
}

void Class::addMethod(std::string name, std::initializer_list<CallableVersion> impl, std::vector<const Type*> templates, int flags, bool legacy) {
	Callable callable;
	for (const auto& v : impl) {
		if ((v.flags & Module::LEGACY) and not legacy) continue;
		callable.add_version(new CallableVersion { v });
	}
	methods.insert({name, callable});
	int i = 0;
	for (auto& m : methods.at(name).versions) {
		((CallableVersion*) m)->name = this->name + "." + name + "." + std::to_string(i++);
		if (templates.size()) {
			((CallableVersion*) m)->templates = templates;
		}
		((CallableVersion*) m)->flags |= flags;
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
	fields.insert({name, {name, type, fun, nullptr}});
}
#endif
void Class::addField(std::string name, const Type* type, void* fun) {
	#if COMPILER
	fields.insert({name, {name, type, fun, nullptr}});
	#endif
}

void Class::addStaticField(field f) {
	static_fields.insert({f.name, f});
}

void Class::addOperator(std::string name, std::initializer_list<CallableVersion> impl, std::vector<const Type*> templates, bool legacy) {
	std::vector<CallableVersion> versions;
	for (const auto& v : impl) {
		if ((v.flags & Module::LEGACY) and not legacy) continue;
		versions.push_back(v);
	}
	if (not versions.size()) return;
	operators.insert({name, versions});
	int i = 0;
	for (auto& m : operators.at(name)) {
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