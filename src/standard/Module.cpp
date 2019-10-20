#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include "Module.hpp"
#include "../analyzer/semantic/Callable.hpp"
#include "../analyzer/semantic/CallableVersion.hpp"
#include "../analyzer/semantic/Variable.hpp"
#include "../analyzer/semantic/Class.hpp"
#include "../type/Type.hpp"
#include "../standard/StandardLibrary.hpp"
#include "../environment/Environment.hpp"
#include "../util/json.hpp"

namespace ls {

int Module::THROWS = 1;
int Module::LEGACY = 2;
int Module::DEFAULT = 4;
int Module::NO_RETURN = 8;
int Module::EMPTY_VARIABLE = 16;
int Module::PRIVATE = 32;

bool Module::STORE_ARRAY_SIZE = true;

Module::Module(Environment& env, std::string name) : env(env), name(name) {
	clazz = std::make_unique<Class>(env, name);
	if (name != "Value") {
		clazz->parent = env.std.classes["Value"]->clazz.get();
	}
}

void Module::operator_(std::string name, std::initializer_list<CallableVersion> impl, std::vector<const Type*> templates) {
	clazz->addOperator(name, impl, templates, env.legacy);
}
void Module::field(std::string name, const Type* type) {
	clazz->addField(name, type, nullptr);
}
#if COMPILER
void Module::field(std::string name, const Type* type, std::function<Compiler::value(Compiler&, Compiler::value)> fun) {
	clazz->addField(name, type, fun);
}
#endif
void Module::field(std::string name, const Type* type, void* fun) {
	clazz->addField(name, type, fun);
}
#if COMPILER
void Module::static_field(std::string name, const Type* type, std::function<Compiler::value(Compiler&)> fun) {
	clazz->addStaticField({name, type, fun});
}
#endif
void Module::static_field(std::string name, const Type* type, void* addr) {
	clazz->addStaticField({name, type, addr});
}
void Module::static_field_fun(std::string name, const Type* type, void* fun) {
	#if COMPILER
	clazz->addStaticField({name, type, fun});
	#endif
}
void Module::constructor_(std::initializer_list<CallableVersion> methods) {
	clazz->addMethod("new", methods);
}
void Module::method(std::string name, std::initializer_list<CallableVersion> methods, int flags, std::vector<const Type*> templates) {
	clazz->addMethod(name, methods, templates, flags, env.legacy);
}
void Template::operator_(std::string name, std::initializer_list<CallableVersion> impl) {
	module->clazz->addOperator(name, impl, templates, module->env.legacy);
}
void Template::method(std::string name, std::initializer_list<CallableVersion> methods) {
	module->method(name, methods, 0, templates);
}

void Module::generate_doc(std::ostream& os, std::string translation_file) {

	std::ifstream f;
	f.open(translation_file);
	if (!f.good()) {
		return; // no file
	}
	std::stringstream j;
	j << f.rdbuf();
	std::string str = j.str();
	f.close();

	// Erase tabs
	str.erase(std::remove(str.begin(), str.end(), '	'), str.end());

	// Parse json
	Json translation;
	try {
		translation = Json::parse(str);
	} catch (std::exception& e) { // LCOV_EXCL_LINE
		assert(false); // LCOV_EXCL_LINE
	}

	std::map<std::string, Json> translation_map;

	for (Json::iterator it = translation.begin(); it != translation.end(); ++it) {
		translation_map.insert({it.key(), it.value()});
	}

	os << "\"" << name << "\":{";

	os << "\"attributes\":{";
	int e = 0;
	for (auto& f : clazz->static_fields) {
		auto& a = f.second;
		auto desc = (translation_map.find(f.first) != translation_map.end()) ?
				translation_map[f.first] : "";
		if (!desc.is_string()) continue;

		if (e > 0) os << ",";
		os << "\"" << f.first << "\":{\"type\":";
		os << a.type->json();
		//os << ",\"value\":\"" << a.value << "\"";
		os << ",\"desc\":" << desc << "";
		os << "}";
		e++;
	}

	os << "},\"methods\":[";
	e = 0;
	for (auto& m : clazz->methods) {
		auto& impl = m.second;
		if (impl.versions[0].flags & PRIVATE) continue;
		if (e > 0) os << ",";
		os << "{\"name\":\"" << m.first << "\",\"type\":";
		os << impl.versions[0].type->json();

		if (translation_map.find(m.first) != translation_map.end()) {
			Json json = translation_map[m.first];
			std::string desc = json["desc"];
			std::string return_desc = json["return"];
			os << ",\"args\":";
			os << json["args"];
			os << ",\"desc\":\"" << desc << "\"";
			os << ",\"return\":\"" << return_desc << "\"";
			os << ",\"examples\":" << json["examples"];
		}
		os << ",\"flags\":" << impl.versions[0].flags;
		os << "}";
		e++;
	}

	os << "],\"static_methods\":{";
	e = 0;
	for (auto& m : clazz->methods) {
		auto& impl = m.second;
		if (e > 0) os << ",";
		os << "\"" << m.first << "\":{\"type\":";
		os << impl.versions[0].type->json();

		if (translation_map.find(m.first) != translation_map.end()) {
			Json json = translation_map[m.first];
			std::string desc = (json.find("desc") != json.end()) ? json["desc"] : "?";
			std::string return_desc = (json.find("return") != json.end()) ? json["return"] : "?";
			os << ",\"desc\":\"" << desc << "\"";
			os << ",\"return\":\"" << return_desc << "\"";
		}
		os << "}";
		e++;
	}
	os << "}}";
}

}
