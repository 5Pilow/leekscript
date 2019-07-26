#include <sstream>
#include "Value.hpp"
#include "../../type/Type.hpp"
#include "../semantic/Callable.hpp"
#include "../semantic/CallableVersion.hpp"
#include "../semantic/SemanticAnalyzer.hpp"
#include "../../environment/Environment.hpp"

namespace ls {

Value::Value(Environment& env) : type(env.any), return_type(env.void_), constant(false) {}

Call Value::get_callable(SemanticAnalyzer*, int argument_count) const {
	return {};
}

void Value::pre_analyze(SemanticAnalyzer*) {}

void Value::analyze(SemanticAnalyzer*) {}

const Type* Value::will_take(SemanticAnalyzer* analyzer, const std::vector<const Type*>& args, int) {
	set_version(analyzer, args, 1);
	return type;
}

bool Value::will_store(SemanticAnalyzer*, const Type*) {
	return false;
}

bool Value::elements_will_store(SemanticAnalyzer*, const Type*, int level) {
	return false;
}

bool Value::must_be_any(SemanticAnalyzer* analyzer) {
	if (type == analyzer->env.any) {
		return false;
	}
	type = analyzer->env.any;
	return true;
}

void Value::must_return_any(SemanticAnalyzer*) {}

void Value::set_version(SemanticAnalyzer* analyzer, const std::vector<const Type*>& args, int) {
	version = args;
	has_version = true;
}

const Type* Value::version_type(std::vector<const Type*>) const {
	return type;
}

bool Value::isLeftValue() const {
	return false;
}

bool Value::is_zero() const {
	return false;
}

#if COMPILER
Compiler::value Value::compile_version(Compiler& c, std::vector<const Type*>) const {
	return compile(c);
}
#endif

std::string Value::tabs(int indent) {
	return std::string(indent * 4, ' ');
}

std::string Value::to_string() const {
	std::ostringstream oss;
	print(oss, 0, { false, true, false });
	return oss.str();
}

std::ostream& operator << (std::ostream& os, const Value* v) {
	v->print(os, 0, { false, true, false });
	return os;
}

}
