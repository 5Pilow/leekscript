#include "Map.hpp"
#include "../../vm/value/LSMap.hpp"
#include "../../type/Type.hpp"
#include <cmath>

namespace ls {

void Map::print(std::ostream& os, int indent, bool debug, bool condensed) const {
	if (values.empty()) {
		os << "[:]";
	} else {
		os << "[";
		for (size_t i = 0; i < values.size(); ++i) {
			if (i > 0) os << ", ";
			keys[i]->print(os, indent + 1, debug);
			os << ": ";
			values[i]->print(os, indent + 1, debug);
		}
		os << "]";
	}
	if (debug) {
		os << " " << type;
	}
}

Location Map::location() const {
	return {opening_bracket->location.file, opening_bracket->location.start, closing_bracket->location.end};
}

void Map::pre_analyze(SemanticAnalyzer* analyzer) {
	for (size_t i = 0; i < keys.size(); ++i) {
		const auto& ex = keys[i];
		ex->pre_analyze(analyzer);
	}
	for (size_t i = 0; i < values.size(); ++i) {
		const auto& ex = values[i];
		ex->pre_analyze(analyzer);
	}
}

void Map::analyze(SemanticAnalyzer* analyzer) {

	const Type* key_type = Type::void_;
	const Type* value_type = Type::void_;

	for (size_t i = 0; i < keys.size(); ++i) {
		const auto& ex = keys[i];
		ex->analyze(analyzer);
		key_type = key_type->operator * (ex->type);
		if (ex->constant == false) constant = false;
	}
	key_type = key_type->not_temporary();

	for (size_t i = 0; i < values.size(); ++i) {
		const auto& ex = values[i];
		ex->analyze(analyzer);
		value_type = value_type->operator * (ex->type);
		if (ex->constant == false) constant = false;
	}
	value_type = value_type->not_temporary();

	if (key_type->is_integer() or key_type->is_real()) {
	} else {
		key_type = Type::any;
		// key_type.setReturnType(Type::any());
	}
	if (value_type->is_integer() || value_type->is_real()) {
	} else {
		value_type = Type::any;
		// value_type.setReturnType(Type::any());
	}
	type = Type::tmp_map(key_type, value_type);
}

Compiler::value Map::compile(Compiler &c) const {

	std::string create;
	std::string insert;
	if (type->key()->is_integer()) {
		create = type->element()->is_integer() ? "Map.new.8" : type->element()->is_real() ? "Map.new.7" : "Map.new.6";
		insert = type->element()->is_integer() ? "Map.insert_fun.8" : type->element()->is_real() ? "Map.insert_fun.7" : "Map.insert_fun.6";
	} else if (type->key()->is_real()) {
		create = type->element()->is_integer() ? "Map.new.5" : type->element()->is_real() ? "Map.new.4" : "Map.new.3";
		insert = type->element()->is_integer() ? "Map.insert_fun.5" : type->element()->is_real() ? "Map.insert_fun.4" : "Map.insert_fun.3";
	} else {
		create = type->element()->is_integer() ? "Map.new.2" : type->element()->is_real() ? "Map.new.1" : "Map.new";
		insert = type->element()->is_integer() ? "Map.insert_fun.2" : type->element()->is_real() ? "Map.insert_fun.1" : "Map.insert_fun";
	}

	unsigned ops = 0;
	auto map = c.insn_call(type, {}, create);

	for (size_t i = 0; i < keys.size(); ++i) {
		auto k = c.insn_convert(keys[i]->compile(c), type->key());
		keys[i]->compile_end(c);
		auto v = c.insn_convert(values[i]->compile(c), type->element());
		values[i]->compile_end(c);

		c.insn_call(Type::void_, {map, k, v}, insert);
		ops += std::log2(i + 1);
	}
	c.inc_ops(ops);
	return map;
}

std::unique_ptr<Value> Map::clone() const {
	auto map = std::make_unique<Map>();
	map->opening_bracket = opening_bracket;
	map->closing_bracket = closing_bracket;
	for (const auto& k : keys) {
		map->keys.push_back(k->clone());
	}
	for (const auto& v : values) {
		map->values.push_back(v->clone());
	}
	return map;
}

}
