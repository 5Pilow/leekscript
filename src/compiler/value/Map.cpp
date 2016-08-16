#include "Map.hpp"
#include "../../vm/VM.hpp"
#include "../../vm/value/LSMap.hpp"
#include <cmath>

using namespace std;

namespace ls {

Map::Map() {

}

Map::~Map()
{
	for (auto ex : keys) {
		delete ex;
	}
	for (auto ex : values) {
		delete ex;
	}
}

void Map::print(std::ostream& os, int indent, bool debug) const {
	if (values.empty()) {
		os << "[:]";
	} else {
		os << "[\n";
		for (size_t i = 0; i < values.size(); ++i) {
			os << tabs(indent+1);
			keys[i]->print(os, indent + 1, debug);
			os << " : ";
			values[i]->print(os, indent + 1, debug);
			os << "\n";
		}
		os << tabs(indent) << "]";
	}
	if (debug) {
		os << " " << type;
	}
}

unsigned Map::line() const {
	return 0;
}

void Map::analyse(SemanticAnalyser* analyser, const Type&) {
	constant = true;

	Type key_type = Type::UNKNOWN;
	Type value_type = Type::UNKNOWN;

	for (size_t i = 0; i < keys.size(); ++i) {
		Value* ex = keys[i];
		ex->analyse(analyser, Type::UNKNOWN);

		if (ex->constant == false) {
			constant = false;
		}
		key_type = Type::get_compatible_type(key_type, ex->type);
	}
	for (size_t i = 0; i < values.size(); ++i) {
		Value* ex = values[i];
		ex->analyse(analyser, Type::UNKNOWN);

		if (ex->constant == false) {
			constant = false;
		}
		value_type = Type::get_compatible_type(value_type, ex->type);
	}

	if (key_type == Type::INTEGER) {
	} else if (key_type.raw_type == RawType::FUNCTION) {
		key_type.nature = Nature::POINTER;
	} else {
		key_type = Type::POINTER;
		key_type.setReturnType(Type::POINTER);
	}
	if (value_type == Type::INTEGER || value_type == Type::FLOAT) {
	} else if (value_type.raw_type == RawType::FUNCTION) {
		value_type.nature = Nature::POINTER;
	} else {
		value_type = Type::POINTER;
		value_type.setReturnType(Type::POINTER);
	}

	// Re-analyze expressions with the supported type
	for (size_t i = 0; i < keys.size(); ++i) {
		keys[i]->analyse(analyser, key_type);
	}
	for (size_t i = 0; i < values.size(); ++i) {
		values[i]->analyse(analyser, value_type);
	}

	type = Type::MAP;
	type.element_types = { key_type, value_type };
}

LSMap<LSValue*,LSValue*>* LSMap_create_ptr_ptr() {
	return new LSMap<LSValue*,LSValue*>();
}
LSMap<LSValue*,int>* LSMap_create_ptr_int() {
	return new LSMap<LSValue*,int>();
}
LSMap<LSValue*,double>* LSMap_create_ptr_float() {
	return new LSMap<LSValue*,double>();
}
LSMap<int,LSValue*>* LSMap_create_int_ptr() {
	return new LSMap<int,LSValue*>();
}
LSMap<int,int>* LSMap_create_int_int() {
	return new LSMap<int,int>();
}
LSMap<int,double>* LSMap_create_int_float() {
	return new LSMap<int,double>();
}

void LSMap_insert_ptr_ptr(LSMap<LSValue*,LSValue*>* map, LSValue* key, LSValue* value) {
	map->ls_insert(key, value);
}
void LSMap_insert_ptr_int(LSMap<LSValue*,int>* map, LSValue* key, int value) {
	map->ls_insert(key, value);
}
void LSMap_insert_ptr_float(LSMap<LSValue*,double>* map, LSValue* key, double value) {
	map->ls_insert(key, value);
}
void LSMap_insert_int_ptr(LSMap<int,LSValue*>* map, int key, LSValue* value) {
	map->ls_insert(key, value);
}
void LSMap_insert_int_int(LSMap<int,int>* map, int key, int value) {
	map->ls_insert(key, value);
}
void LSMap_insert_int_float(LSMap<int,double>* map, int key, double value) {
	map->ls_insert(key, value);
}

jit_value_t Map::compile(Compiler &c) const {

	jit_type_t key_type = type.element_types[0] == Type::INTEGER ? JIT_INTEGER : JIT_POINTER;
	jit_type_t value_type = type.element_types[1] == Type::INTEGER ? JIT_INTEGER:
																	type.element_types[1] == Type::FLOAT ? JIT_FLOAT :
																										  JIT_POINTER;

	void* create = nullptr;
	void* insert = nullptr;

	if (type.element_types[0] == Type::INTEGER) {
		create = type.element_types[1] == Type::INTEGER ? (void*) LSMap_create_int_int :
														 type.element_types[1] == Type::FLOAT ? (void*) LSMap_create_int_float :
																							   (void*) LSMap_create_int_ptr;
		insert = type.element_types[1] == Type::INTEGER ? (void*) LSMap_insert_int_int :
														 type.element_types[1] == Type::FLOAT ? (void*) LSMap_insert_int_float :
																							   (void*) LSMap_insert_int_ptr;
	} else {
		create = type.element_types[1] == Type::INTEGER ? (void*) LSMap_create_ptr_int :
														 type.element_types[1] == Type::FLOAT ? (void*) LSMap_create_ptr_float :
																							   (void*) LSMap_create_ptr_ptr;
		insert = type.element_types[1] == Type::INTEGER ? (void*) LSMap_insert_ptr_int :
														 type.element_types[1] == Type::FLOAT ? (void*) LSMap_insert_ptr_float :
																							   (void*) LSMap_insert_ptr_ptr;
	}

	unsigned ops = 0;

	jit_type_t sig = jit_type_create_signature(jit_abi_cdecl, JIT_POINTER, {}, 0, 0);
	jit_value_t map = jit_insn_call_native(c.F, "new_map", (void*) create, sig, {}, 0, JIT_CALL_NOTHROW); ops += 1;

	for (size_t i = 0; i < keys.size(); ++i) {
		jit_value_t k = keys[i]->compile(c);
		jit_value_t v = values[i]->compile(c);

		jit_type_t args[3] = {JIT_POINTER, key_type, value_type};
		jit_type_t sig = jit_type_create_signature(jit_abi_cdecl, JIT_VOID, args, 3, 0);
		jit_value_t args_v[] = {map, k, v};
		jit_insn_call_native(c.F, "insert", (void*) insert, sig, args_v, 3, JIT_CALL_NOTHROW); ops += std::log2(i + 1);

		if (type.element_types[0].must_manage_memory()) {
			VM::delete_temporary(c.F, k);
		}
		if (type.element_types[1].must_manage_memory()) {
			VM::delete_temporary(c.F, v);
		}
	}

	VM::inc_ops(c.F, ops);

	return map;
}

}
