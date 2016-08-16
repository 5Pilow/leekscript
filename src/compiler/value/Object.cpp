#include "../../compiler/value/Object.hpp"

#include "../../vm/value/LSNull.hpp"
#include "../../vm/value/LSString.hpp"
#include "../../vm/value/LSObject.hpp"

using namespace std;

namespace ls {

Object::Object() {
	type = Type::OBJECT;
}

Object::~Object() {
	for (auto key : keys) {
		delete key;
	}
	for (auto ex : values) {
		delete ex;
	}
}

void Object::print(ostream& os, int indent, bool debug) const {
	os << "{";
	for (unsigned i = 0; i < keys.size(); ++i) {
		os << keys.at(i)->token->content;
		os << ": ";
		values.at(i)->print(os, indent);
		if (i < keys.size() - 1) {
			os << ", ";
		}
	}
	os << "}";
	if (debug) {
		os << " " << type;
	}
}

unsigned Object::line() const {
	return 0;
}

void Object::analyse(SemanticAnalyser* analyser, const Type&) {
	for (Value* value : values) {
		value->analyse(analyser, Type::POINTER);
	}
}

void push_object(LSObject* o, std::string* k, LSValue* v) {
	o->addField(*k, v);
}

jit_value_t Object::compile(Compiler& c) const {

	jit_value_t object = VM::create_object(c.F);

	jit_type_t args[3] = {JIT_POINTER, JIT_POINTER, JIT_POINTER};
	jit_type_t sig = jit_type_create_signature(jit_abi_cdecl, JIT_VOID, args, 3, 0);

	for (unsigned i = 0; i < keys.size(); ++i) {
		jit_value_t k = JIT_CREATE_CONST_POINTER(c.F, &keys.at(i)->token->content);
		jit_value_t v = values[i]->compile(c);
		jit_value_t args[] = {object, k, v};
		jit_insn_call_native(c.F, "push", (void*) push_object, sig, args, 3, JIT_CALL_NOTHROW);
	}

	return object;
}

}
