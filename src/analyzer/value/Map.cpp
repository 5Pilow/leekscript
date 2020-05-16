#include "Map.hpp"
#include "../../type/Type.hpp"
#include <cmath>
#include "../semantic/SemanticAnalyzer.hpp"

namespace ls {

Map::Map(Environment& env) : Value(env) {}

void Map::print(std::ostream& os, int indent, PrintOptions options) const {
	if (values.empty()) {
		os << "[:]";
	} else {
		os << "[";
		for (size_t i = 0; i < values.size(); ++i) {
			if (i > 0) os << ", ";
			keys[i]->print(os, indent + 1, options);
			os << ": ";
			values[i]->print(os, indent + 1, options);
		}
		os << "]";
	}
	if (options.debug) {
		os << " " << type;
	}
}

Location Map::location() const {
	return { opening_bracket->location.file, opening_bracket->location.start, closing_bracket->location.end };
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
	const auto& env = analyzer->env;

	const Type* key_type = env.void_;
	const Type* value_type = env.void_;

	if (values.size()) {
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
			value_type = value_type->operator + (ex->type);
			if (ex->constant == false) constant = false;
		}
		value_type = value_type->not_temporary();
	} else {
		key_type = env.never;
		value_type = env.never;
	}
	type = Type::tmp_map(key_type, value_type);
}

Hover Map::hover(SemanticAnalyzer& analyzer, size_t position) const {
	for (size_t i = 0; i < keys.size(); ++i) {
		if (keys[i]->location().contains(position)) {
			return keys[i]->hover(analyzer, position);
		}
		if (values[i]->location().contains(position)) {
			return values[i]->hover(analyzer, position);
		}
	}
	return { type, location() };
}

#if COMPILER
Compiler::value Map::compile(Compiler &c) const {

	std::string create;
	std::string insert;
	auto key_type = type->key()->fold();
	auto element_type = type->element()->fold();
	// std::cout << "key = " << key_type << " element_type = " << element_type << std::endl;
	if (key_type->is_integer()) {
		create = element_type->is_integer() ? "Map.new.8" : element_type->is_real() ? "Map.new.7" : "Map.new.6";
		insert = element_type->is_integer() ? "Map.insert_fun.8" : element_type->is_real() ? "Map.insert_fun.7" : "Map.insert_fun.6";
	} else if (key_type->is_real()) {
		create = element_type->is_integer() ? "Map.new.5" : element_type->is_real() ? "Map.new.4" : "Map.new.3";
		insert = element_type->is_integer() ? "Map.insert_fun.5" : element_type->is_real() ? "Map.insert_fun.4" : "Map.insert_fun.3";
	} else {
		create = element_type->is_integer() ? "Map.new.2" : element_type->is_real() ? "Map.new.1" : "Map.new";
		insert = element_type->is_integer() ? "Map.insert_fun.2" : element_type->is_real() ? "Map.insert_fun.1" : "Map.insert_fun";
	}

	unsigned ops = 0;
	auto map = c.insn_call(type, {}, create);

	for (size_t i = 0; i < keys.size(); ++i) {
		auto k = c.insn_convert(keys[i]->compile(c), type->key());
		keys[i]->compile_end(c);
		auto v = c.insn_convert(values[i]->compile(c), type->element());
		values[i]->compile_end(c);

		c.insn_call(c.env.void_, {map, k, v}, insert);
		ops += std::log2(i + 1);
	}
	c.inc_ops(ops);
	return map;
}
#endif

std::unique_ptr<Value> Map::clone(Block* parent) const {
	auto map = std::make_unique<Map>(type->env);
	map->opening_bracket = opening_bracket;
	map->closing_bracket = closing_bracket;
	for (const auto& k : keys) {
		map->keys.push_back(k->clone(parent));
	}
	for (const auto& v : values) {
		map->values.push_back(v->clone(parent));
	}
	return map;
}

}
