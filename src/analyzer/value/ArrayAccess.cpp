#include "ArrayAccess.hpp"
#include "Array.hpp"
#include "../semantic/SemanticAnalyzer.hpp"
#include "../error/Error.hpp"
#include "../semantic/Callable.hpp"
#include "../semantic/CallableVersion.hpp"

namespace ls {

ArrayAccess::ArrayAccess(Environment& env) : LeftValue(env)
#if COMPILER
, compiled_array(env)
#endif
{
	type = env.any;
	map_key_type = env.void_;
	throws = true;
}

bool ArrayAccess::isLeftValue() const {
	return key2 == nullptr // Range access is not left-value (yet)
		&& not array->type->is_string(); // String access is not yet
}

void ArrayAccess::print(std::ostream& os, int indent, PrintOptions options) const {
	array->print(os, indent, options);
	os << "[";
	if (key != nullptr) {
		key->print(os, indent, options);
	}
	if (key2 != nullptr) {
		os << ":";
		key2->print(os, indent, options);
	}
	os << "]";
	if (options.debug) {
		os << " " << type;
	}
}

Location ArrayAccess::location() const {
	return {close_bracket->location.file, array->location().start, close_bracket->location.end};
}

Call ArrayAccess::get_callable(SemanticAnalyzer* analyzer, int argument_count) const {
	const auto& env = analyzer->env;
	// std::cout << "Array access get callable " << type << std::endl;
	if (type->is_function()) {
		return { { { "<aa>", type, this } } };
	} else {
		// The array is not homogeneous, so the function inside an array always returns any
		return { { { "<aa>", Type::fun(env.any, { env.any }), this, {}, {}, false, true } } };
	}
}

void ArrayAccess::pre_analyze(SemanticAnalyzer* analyzer) {
	array->pre_analyze(analyzer);
	if (key) key->pre_analyze(analyzer);
	if (key2) key2->pre_analyze(analyzer);
}

void ArrayAccess::analyze(SemanticAnalyzer* analyzer) {
	const auto& env = analyzer->env;
	// std::cout << "Analyze AA " << this << " : " << req_type << std::endl;

	array->analyze(analyzer);

	if (not array->type->can_be_container()) {
		analyzer->add_error({Error::Type::VALUE_MUST_BE_A_CONTAINER, location(), array->location(), {array->to_string()}});
		return;
	}
	if (key == nullptr) {
		return;
	}

	key->analyze(analyzer);
	constant = array->constant && key->constant;

	type = array->type->element();

	if (array->type->is_map()) {
		map_key_type = array->type->key();
	}

	// Range array access : array[4:12], check if the values are numbers
	if (key != nullptr and key2 != nullptr) {

		key2->analyze(analyzer);

		if (!key->type->is_any() and not key->type->is_number()) {
			std::string k = "<key 1>";
			analyzer->add_error({Error::Type::ARRAY_ACCESS_RANGE_KEY_MUST_BE_NUMBER, location(), key->location(), {k}});
		}
		if (!key2->type->is_any() and not key2->type->is_number()) {
			std::string k = "<key 2>";
			analyzer->add_error({Error::Type::ARRAY_ACCESS_RANGE_KEY_MUST_BE_NUMBER, location(), key2->location(), {k}});
		}
		type = array->type;
		type = type->add_temporary();

	} else if (array->type->is_array() or array->type->is_string() or array->type->is_interval()) {
		if (not env.legacy) { // In legacy mode, any type can be used as the key, to transform the array into a map
			if (not key->type->can_be_numeric()) {
				std::string a = array->to_string();
				std::string k = key->to_string();
				std::string kt = key->type->to_string();
				analyzer->add_error({Error::Type::ARRAY_ACCESS_KEY_MUST_BE_NUMBER, location(), key->location(), {k, a, kt}});
			}
		}
		if (array->type->is_string()) {
			type = env.string;
		}
	} else if (array->type->is_map()) {
		if (not env.legacy) { // In legacy mode, any type can be used as the key
			if (!key->type->castable(map_key_type)) {
				std::string a = array->to_string();
				std::string k = key->to_string();
				std::string kt = key->type->to_string();
				analyzer->add_error({Error::Type::INVALID_MAP_KEY, location(), key->location(), {k, a, kt}});
			}
		}
	}
	// TODO should be temporary
}

const Type* ArrayAccess::will_take(SemanticAnalyzer* analyzer, const std::vector<const Type*>& args, int) {

	// std::cout << "ArrayAccess::will_take(" << args << ", " << level << ")" << std::endl;

	if (Array* arr = dynamic_cast<Array*>(array.get())) {
		arr->elements_will_take(analyzer, args, 1);
	}
	if (ArrayAccess* arr = dynamic_cast<ArrayAccess*>(array.get())) {
		arr->array_access_will_take(analyzer, args, 1);
	}

	type = array->type->element();
	return type;
}

bool ArrayAccess::array_access_will_take(SemanticAnalyzer* analyzer, const std::vector<const Type*>& arg_types, int level) {
	if (auto arr = dynamic_cast<Array*>(array.get())) {
		arr->elements_will_take(analyzer, arg_types, level);
	}
	if (auto arr = dynamic_cast<ArrayAccess*>(array.get())) {
		arr->array_access_will_take(analyzer, arg_types, level + 1);
	}
	type = array->type->element();
	return false;
}

bool ArrayAccess::will_store(SemanticAnalyzer* analyzer, const Type* type) {
	array->elements_will_store(analyzer, type, 1);
	this->type = array->type->element();
	return false;
}

void ArrayAccess::change_value(SemanticAnalyzer* analyzer, Value* value) {
	array->will_store(analyzer, value->type);
	if (!type->is_any()) {
		this->type = array->type->element();
	}
}

const Type* ArrayAccess::version_type(std::vector<const Type*> version) const {
	const auto& env = type->env;
	// std::cout << "AA vt " << type << std::endl;
	if (type->is_function()) {
		return type;
	} else {
		// The array is not homogeneous, so the function inside an array always returns any
		return Type::fun_object(env.any, { env.any });
	}
}

Hover ArrayAccess::hover(SemanticAnalyzer& analyzer, size_t position) const {
	if (position < open_bracket->location.start.raw) {
		return array->hover(analyzer, position);
	}
	if (key and position > open_bracket->location.end.raw and position < close_bracket->location.start.raw) {
		return key->hover(analyzer, position);
	}
	return { type, location() };
}

#if COMPILER
Compiler::value ArrayAccess::compile(Compiler& c) const {
	const auto& env = c.env;

	c.mark_offset(open_bracket->location.start.line);

	((ArrayAccess*) this)->compiled_array = array->compile(c);
	c.add_temporary_value(compiled_array);
	((ArrayAccess*) this)->should_delete_array = true;
	array->compile_end(c);

	c.inc_ops(2); // Array access : 2 operations

	if (key2 == nullptr) {

		if (array->type->is_interval()) {

			auto k = key->compile(c);
			auto r = c.insn_invoke(env.integer, {compiled_array, k}, "Interval.at_i_i");
			key->compile_end(c);
			return r;

		} else if (array->type->is_map()) {

			auto k = key->compile(c);
			key->compile_end(c);
			k = c.insn_convert(k, map_key_type->fold());

			std::string func;
			if (map_key_type->fold()->is_integer()) {
				if (type->fold()->is_integer()) {
					func = "Map.at.0";
				} else if (type->fold()->is_real()) {
					func = "Map.at.1";
				} else {
					func = "Map.at.2";
				}
			} else if (map_key_type->fold()->is_real()) {
				if (type->fold()->is_integer()) {
					func = "Map.at.3";
				} else if (type->fold()->is_real()) {
					func = "Map.at.4";
				} else {
					func = "Map.at.5";
				}
			} else {
				if (type->fold()->is_integer()) {
					func = "Map.at.6";
				} else if (type->fold()->is_real()) {
					func = "Map.at.7";
				} else {
					func = "Map.at.8";
				}
			}
			auto res = c.insn_invoke(type, {compiled_array, k}, func);
			c.insn_delete_temporary(k);
			c.inc_ops(2);
			return res;

		} else if (array->type->is_string() or array->type->is_array()) {

			auto k = key->compile(c);
			if (k.t->is_polymorphic()) {
				k = c.insn_invoke(env.integer, {k}, "Array.convert_key");
			}
			auto int_key = c.to_int(k);
			key->compile_end(c);

			// Check index : k < 0 or k >= size
			auto array_size = c.insn_array_size(compiled_array);
			c.insn_if(c.insn_or(c.insn_lt(int_key, c.new_integer(0)), c.insn_ge(int_key, array_size)), [&]() {
				c.insn_throw_object(vm::Exception::ARRAY_OUT_OF_BOUNDS);
			});

			if (array->type->is_string()) {
				auto e = c.insn_call(env.tmp_string, {compiled_array, int_key}, "String.codePointAt");
				c.insn_delete_temporary(k);
				return e;
			} else {
				auto element_addr = c.insn_array_at(compiled_array, int_key);
				c.insn_delete_temporary(k);
				return c.insn_load(element_addr);
			}
		} else {
			// Unknown type, call generic at() operator
			if (type->is_integer() and key->type->is_integer()) {
				auto k = key->compile(c);
				key->compile_end(c);
				auto e = c.insn_invoke(env.integer, {compiled_array, k}, "Value.at_i_i");
				c.insn_delete_temporary(k);
				return e;
			} else {
				auto k = c.insn_to_any(key->compile(c));
				key->compile_end(c);
				auto e = c.insn_invoke(env.any, {compiled_array, k}, "Value.at");
				c.insn_delete_temporary(k);
				return e;
			}
		}
	} else {
		auto start = key->compile(c);
		auto end = key2->compile(c);
		auto r = c.insn_invoke(type, {compiled_array, start, end}, "Value.range");
		key->compile_end(c);
		key2->compile_end(c);
		return r;
	}
}

Compiler::value ArrayAccess::compile_l(Compiler& c) const {
	const auto& env = c.env;

	// Compile the array
	((ArrayAccess*) this)->compiled_array = [&]() {
		if (array->isLeftValue()) {
			return c.insn_load(static_cast<LeftValue*>(array.get())->compile_l(c));
		} else {
			auto a = array->compile(c);
			c.add_temporary_value(a);
			((ArrayAccess*) this)->should_delete_array = true;
			return a;
		}
	}();

	if (key2 == nullptr) {
		// Compile the key
		auto k = key->compile(c);
		key->compile_end(c);
		k = c.insn_convert(k, array->type->key()->fold());
		// Access
		c.mark_offset(location().start.line);
		if (array->type->is_array()) {

			auto array_size = c.insn_array_size(compiled_array);
			c.insn_if(c.insn_or(c.insn_lt(k, c.new_integer(0)), c.insn_ge(k, array_size)), [&]() {
				c.insn_delete_temporary(compiled_array);
				c.insn_throw_object(vm::Exception::ARRAY_OUT_OF_BOUNDS);
			});
			return c.insn_array_at(compiled_array, k);

		} else if (array->type->is_map()) {
			auto key_type = array->type->key()->fold();
			auto element_type = array->type->element()->fold();
			auto f = [&]() { if (key_type == env.integer) {
				if (element_type == env.integer) return "Map.atL.8";
				if (element_type == env.real) return "Map.atL.7";
				return "Map.atL.6";
			} else if (key_type == env.real) {
				if (element_type == env.integer) return "Map.atL.5";
				if (element_type == env.real) return "Map.atL.4";
				return "Map.atL.3";
			} else {
				if (element_type == env.integer) return "Map.atL.2";
				if (element_type == env.real) return "Map.atL.1";
				return "Map.atL";
			}}();
			return c.insn_call(array->type->element()->pointer(), {compiled_array, k}, f);
		} else {
			k = c.insn_to_any(k);
			return c.insn_invoke(type->pointer(), {compiled_array, k}, "Value.atl");
		}
	} else {
		// auto start = key->compile(c);
		// auto end = key2->compile(c);
		// key->compile_end(c);
		// key2->compile_end(c);
		// c.mark_offset(open_bracket->location.start.line);
		assert(false);
	}
}

void ArrayAccess::compile_end(Compiler& c) const {
	if (should_delete_array) {
		c.pop_temporary_value();
	}
}
#endif

std::unique_ptr<Value> ArrayAccess::clone(Block* parent) const {
	auto aa = std::make_unique<ArrayAccess>(type->env);
	aa->array = array->clone(parent);
	aa->key = key->clone(parent);
	aa->key2 = key2 ? key2->clone(parent) : nullptr;
	aa->open_bracket = open_bracket;
	aa->close_bracket = close_bracket;
	return aa;
}

}
