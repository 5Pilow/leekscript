#include "Context.hpp"
#include <sstream>
#include <string.h>
#include <vector>
#include "../util/json.hpp"
#include "../type/Type.hpp"
#if COMPILER
#include "../vm/LSValue.hpp"
#endif

namespace ls {

Context::Context(Environment& env) : env(env) {}

Context::Context(Environment& env, std::string ctx) : env(env) {

	Json value = Json::parse(ctx);

	for (Json::iterator it = value.begin(); it != value.end(); ++it) {
		// vars.insert({it.key(), ls::LSValue::parse(it.value())});
	}
}

Context::~Context() {
	// std::cout << "delete context" << std::endl;
	for (const auto& var : vars) {
		if (var.second.type == env.any) {
			// std::cout << "delete var " << (LSValue*)var.second.value << std::endl;
			delete (LSValue*)var.second.value;
		}
	}
}

void Context::add_variable(char* name, void* v, const Type* type) {
	// std::cout << "add_variable " << name << " " << v << " " << ((LSValue*) v)->refs << std::endl;
	auto i = vars.find(name);
	if (i != vars.end()) {
		// std::cout << "variable already exists " << i->second.value << std::endl;
		if (i->second.type->is_polymorphic()) {
			#if COMPILER && DEBUG_LEAKS
			// The previous object was deleted in the program, but we don't count the destruction
			LSValue::obj_deleted--;
			#endif
		}
		vars[name] = { v, type };
	} else {
		// std::cout << "variable doesn't exist" << std::endl;
		vars.insert({ name, ContextVar { v, type, nullptr, nullptr } });
	}
}

}

namespace std {
	std::ostream& operator << (std::ostream& os, const ls::Context* context) {
		os << "{";
		int i = 0;
		for (const auto& v : context->vars) {
			if (i++ > 0) os << ", ";
			os << v.first << ": " << v.second.value << " " << v.second.type;
			#if COMPILER
			if (v.second.type == context->env.any) {
				os << " " << (ls::LSValue*) v.second.value;
				os << " " << ((ls::LSValue*) v.second.value)->refs;
			}
			#endif
		}
		os << "}";
		return os;
	}
}
