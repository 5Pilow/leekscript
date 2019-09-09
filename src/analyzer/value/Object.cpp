#include "Object.hpp"
#include "../../vm/value/LSNull.hpp"
#include "../../vm/value/LSString.hpp"
#include "../../vm/value/LSObject.hpp"
#include "../../type/Type.hpp"

namespace ls {

Object::Object() {
	type = Type::tmp_object;
}

void Object::print(std::ostream& os, int indent, bool debug, bool condensed) const {
	os << "{";
	for (unsigned i = 0; i < keys.size(); ++i) {
		os << keys.at(i)->content;
		os << ": ";
		values.at(i)->print(os, indent, debug);
		if (i < keys.size() - 1) {
			os << ", ";
		}
	}
	os << "}";
	if (debug) {
		os << " " << type;
	}
}

Location Object::location() const {
	return {nullptr, {0, 0, 0}, {0, 0, 0}}; // TODO
}

void Object::pre_analyze(SemanticAnalyzer* analyzer) {
	for (auto& value : values) {
		value->pre_analyze(analyzer);
	}
}

void Object::analyze(SemanticAnalyzer* analyzer) {
	for (auto& value : values) {
		value->analyze(analyzer);
	}
}

Compiler::value Object::compile(Compiler& c) const {
	auto object = c.new_object();
	for (unsigned i = 0; i < keys.size(); ++i) {
		auto k = c.new_const_string(keys.at(i)->content);
		auto v = c.insn_to_any(values[i]->compile(c));
		c.insn_call(Type::void_, {object, k, v}, "Object.add_field");
	}
	return object;
}

std::unique_ptr<Value> Object::clone() const {
	auto o = std::make_unique<Object>();
	for (const auto& k : keys) {
		o->keys.push_back(k);
	}
	for (const auto& v : values) {
		o->values.push_back(v->clone());
	}
	return o;
}

}
