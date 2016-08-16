#include "../../compiler/value/VariableValue.hpp"

#include "../../vm/VM.hpp"
#include "math.h"
#include "../semantic/SemanticAnalyser.hpp"
#include "../value/Function.hpp"
#include "../instruction/VariableDeclaration.hpp"

using namespace std;

namespace ls {

VariableValue::VariableValue(Token* token) {
	this->name = token->content;
	this->token = token;
	this->var = nullptr;
	constant = false;
}

VariableValue::~VariableValue() {}

void VariableValue::print(ostream& os, int, bool debug) const {
	os << token->content;
	if (debug) {
		os << " " << type;
	}
}

unsigned VariableValue::line() const {
	return token->line;
}

void VariableValue::analyse(SemanticAnalyser* analyser, const Type& req_type) {

	var = analyser->get_var(token);
	if (var != nullptr) {
		type = var->type;
		attr_types = var->attr_types;

		if (var->function != analyser->current_function()) {
			analyser->current_function()->capture(var);
  		}
	} else {
		type = Type::POINTER;
	}

	if (req_type.nature == Nature::POINTER) {
		type.nature = req_type.nature;
	}
//	cout << "VV " << name << " : " << type << endl;
//	cout << "var scope : " << (int)var->scope << endl;
//	for (auto t : attr_types)
//		cout << t.first << " : " << t.second << endl;
}

bool VariableValue::will_take(SemanticAnalyser* analyser, const vector<Type>& args_type) {

	bool changed = false;
	if (var != nullptr and var->value != nullptr) {
		changed = var->value->will_take(analyser, args_type);
		type = var->value->type;

	}
	return changed;
}

void VariableValue::must_return(SemanticAnalyser* analyser, const Type& ret_type) {

	var->value->must_return(analyser, ret_type);
	type.setReturnType(ret_type);
}

/*
 * let a = 12 // integer
 * a = 5.6 // float
 * a = 12 // integer
 * a += 1.2 // float at the previous assignment
 */
void VariableValue::change_type(SemanticAnalyser*, const Type& type) {
	if (var != nullptr) {
		var->type = type;
		this->type = type;
	}
}

extern map<string, jit_value_t> internals;

jit_value_t VariableValue::compile(Compiler& c) const {

//	cout << "compile vv " << name->content << " : " << type << endl;
//	cout << "req type : " << req_type << endl;

	jit_value_t v;

	if (var->scope == VarScope::INTERNAL) {

		v = internals[name];

	} else if (var->scope == VarScope::LOCAL) {

		v = c.get_var(name).value;

	} else { // Parameter

		v = jit_value_get_param(c.F, var->index);
	}

	if (var->type.nature != Nature::POINTER and type.nature == Nature::POINTER) {
		return VM::value_to_pointer(c.F, v, var->type);
	}
	return v;
}

jit_value_t VariableValue::compile_l(Compiler& c) const {
	return compile(c);
}

}
