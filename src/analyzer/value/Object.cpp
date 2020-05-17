#include "Object.hpp"
#include "../../type/Type.hpp"
#include "../semantic/SemanticAnalyzer.hpp"

namespace ls {

Object::Object(Environment& env) : Value(env) {
	type = env.tmp_object;
}

void Object::print(std::ostream& os, int indent, PrintOptions options) const {
	os << "{";
	for (unsigned i = 0; i < keys.size(); ++i) {
		os << keys.at(i)->content;
		os << ": ";
		values.at(i)->print(os, indent, options);
		if (i < keys.size() - 1) {
			os << ", ";
		}
	}
	os << "}";
	if (options.debug) {
		os << " " << type;
	}
}

Location Object::location() const {
	return { opening_brace->location.file, opening_brace->location.end, closing_brace->location.end };
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

Hover Object::hover(SemanticAnalyzer& analyzer, size_t position) const {
	for (const auto& value : values) {
		if (value->location().contains(position)) {
			return value->hover(analyzer, position);
		}
	}
	return { type, location() };
}

#if COMPILER
Compiler::value Object::compile(Compiler& c) const {
	auto object = c.new_object();
	for (unsigned i = 0; i < keys.size(); ++i) {
		auto k = c.new_const_string(keys.at(i)->content);
		auto v = c.insn_to_any(values[i]->compile(c));
		c.insn_call(c.env.void_, {object, k, v}, "Object.add_field");
	}
	return object;
}
#endif

std::unique_ptr<Value> Object::clone(Block* parent) const {
	auto o = std::make_unique<Object>(type->env);
	o->opening_brace = opening_brace;
	o->closing_brace = closing_brace;
	for (const auto& k : keys) {
		o->keys.push_back(k);
	}
	for (const auto& v : values) {
		o->values.push_back(v->clone(parent));
	}
	return o;
}

}
