#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include "Module.hpp"
#include "LSValue.hpp"
#include "value/LSClass.hpp"
#include "value/LSNumber.hpp"
#include "../analyzer/semantic/Callable.hpp"
#include "../analyzer/semantic/CallableVersion.hpp"
#include "../analyzer/semantic/Variable.hpp"
#include "../analyzer/semantic/Class.hpp"
#include "VM.hpp"
#include "../type/Type.hpp"

namespace ls {

int Module::THROWS = 1;
int Module::LEGACY = 2;
int Module::DEFAULT = 4;
int Module::NO_RETURN = 8;
int Module::EMPTY_VARIABLE = 16;

bool Module::STORE_ARRAY_SIZE = true;

Module::Module(VM* vm, std::string name) : vm(vm), name(name) {
	clazz = std::make_unique<Class>(name);
	#if COMPILER
	lsclass = std::make_unique<LSClass>(clazz.get());
	#endif
	if (name != "Value") {
		clazz->parent = vm->internal_vars["Value"]->clazz;
	}
}

void Module::operator_(std::string name, std::initializer_list<CallableVersion> impl, std::vector<const Type*> templates) {
	clazz->addOperator(name, impl, templates, vm->legacy);
}
void Module::field(std::string name, const Type* type) {
	clazz->addField(name, type, nullptr);
}
void Module::field(std::string name, const Type* type, std::function<Compiler::value(Compiler&, Compiler::value)> fun) {
	clazz->addField(name, type, fun);
}
void Module::field(std::string name, const Type* type, void* fun) {
	clazz->addField(name, type, fun);
}
void Module::static_field(std::string name, const Type* type, std::function<Compiler::value(Compiler&)> fun) {
	clazz->addStaticField({name, type, fun});
}
void Module::static_field(std::string name, const Type* type, void* addr) {
	clazz->addStaticField({name, type, addr, true});
}
void Module::static_field_fun(std::string name, const Type* type, void* fun) {
	clazz->addStaticField({name, type, fun, nullptr});
}
void Module::constructor_(std::initializer_list<CallableVersion> methods) {
	clazz->addMethod("new", methods);
}
void Module::method(std::string name, std::initializer_list<CallableVersion> methods, std::vector<const Type*> templates, bool legacy) {
	clazz->addMethod(name, methods, templates, vm->legacy);
}
void Template::operator_(std::string name, std::initializer_list<CallableVersion> impl) {
	module->clazz->addOperator(name, impl, templates, module->vm->legacy);
}
void Template::method(std::string name, std::initializer_list<CallableVersion> methods) {
	module->method(name, methods, templates, module->vm->legacy);
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
		a.type->toJson(os);
		//os << ",\"value\":\"" << a.value << "\"";
		os << ",\"desc\":\"" << desc << "\"";
		os << "}";
		e++;
	}

	os << "},\"methods\":{";
	e = 0;
	for (auto& m : clazz->methods) {
		auto& impl = m.second;
		if (e > 0) os << ",";
		os << "\"" << m.first << "\":{\"type\":";
		impl.versions[0]->type->toJson(os);

		if (translation_map.find(m.first) != translation_map.end()) {
			Json json = translation_map[m.first];
			std::string desc = json["desc"];
			std::string return_desc = json["return"];

			os << ",\"desc\":\"" << desc << "\"";
			os << ",\"return\":\"" << return_desc << "\"";
		}
		os << "}";
		e++;
	}

	os << "},\"static_methods\":{";
	e = 0;
	for (auto& m : clazz->methods) {
		auto& impl = m.second;
		if (e > 0) os << ",";
		os << "\"" << m.first << "\":{\"type\":";
		impl.versions[0]->type->toJson(os);

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
