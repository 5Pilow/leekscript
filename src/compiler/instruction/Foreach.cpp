#include "../../compiler/instruction/Foreach.hpp"
#include "../../vm/value/LSNull.hpp"
#include "../../vm/value/LSArray.hpp"
#include "../../vm/value/LSMap.hpp"

using namespace std;

namespace ls {

Foreach::Foreach() {
	key = nullptr;
	value = nullptr;
	body = nullptr;
	container = nullptr;
	key_var = nullptr;
	value_var = nullptr;
}

Foreach::~Foreach() {
	delete container;
	delete body;
}

void Foreach::print(ostream& os, int indent, bool debug) const {
	os << "for ";

	if (key != nullptr) {
		os << key->content;
		os << " : ";
	}
	os << value->content;

	os << " in ";
	container->print(os, indent + 1, debug);

	os << " ";
	body->print(os, indent, debug);
}

void Foreach::analyse(SemanticAnalyser* analyser, const Type&) {

	analyser->enter_block();

	container->analyse(analyser, Type::UNKNOWN);

	if (container->type.element_types.size() == 1) {
		key_type = Type::INTEGER; // If no key type in array key = 0, 1, 2...
		value_type = container->type.element_types[0];
	} else if (container->type.element_types.size() == 2) {
		key_type = container->type.element_types[0];
		value_type = container->type.element_types[1];
	} else if (container->type == Type::ARRAY) {
		key_type = Type::INTEGER;
		value_type = Type::POINTER;
	} else {
		key_type = Type::POINTER;
		value_type = Type::POINTER;
	}

	if (key != nullptr) {
		key_var = analyser->add_var(key, key_type, nullptr, nullptr);
	}
	value_var = analyser->add_var(value, value_type, nullptr, nullptr);


	analyser->enter_loop();
	body->analyse(analyser, Type::VOID);
	analyser->leave_loop();
	analyser->leave_block();
}

/*
 * Begin
 */
LSArrayIterator<LSValue*> fun_begin_array_all(LSArray<LSValue*>* array) {
	return array->begin();
}
LSMapIterator<LSValue*,LSValue*> fun_begin_map_all(LSMap<LSValue*,LSValue*>* map) {
	return map->begin();
}

/*
 * Condition
 */
bool fun_condition_array_all(LSArray<LSValue*>* array, LSArrayIterator<LSValue*> it) {
	return it != array->end();
}
bool fun_condition_map_all(LSMap<LSValue*,LSValue*>* map, LSMapIterator<LSValue*,LSValue*> it) {
	return it != map->end();
}

/*
 * Value
 */
LSValue* fun_value_array_ptr(LSArrayIterator<LSValue*> it) {
	return *it;
}
int fun_value_array_int(LSArrayIterator<int> it) {
	return *it;
}
LSValue* fun_value_array_int_2ptr(LSArrayIterator<int> it) {
	return LSNumber::get(*it);
}
double fun_value_array_float(LSArrayIterator<double> it) {
	return *it;
}
LSValue* fun_value_array_float_2ptr(LSArrayIterator<double> it) {
	return LSNumber::get(*it);
}
LSValue* fun_value_map_ptr_ptr(LSMapIterator<LSValue*,LSValue*> it) {
	return it->second;
}
int fun_value_map_ptr_int(LSMapIterator<LSValue*,int> it) {
	return it->second;
}
LSValue* fun_value_map_ptr_int_2ptr(LSMapIterator<LSValue*,int> it) {
	return LSNumber::get(it->second);
}
double fun_value_map_ptr_float(LSMapIterator<LSValue*,double> it) {
	return it->second;
}
LSValue* fun_value_map_ptr_float_2ptr(LSMapIterator<LSValue*,double> it) {
	return LSNumber::get(it->second);
}
LSValue* fun_value_map_int_ptr(LSMapIterator<int,LSValue*> it) {
	return it->second;
}
int fun_value_map_int_int(LSMapIterator<int,int> it) {
	return it->second;
}
LSValue* fun_value_map_int_int_2ptr(LSMapIterator<int,int> it) {
	return LSNumber::get(it->second);
}
double fun_value_map_int_float(LSMapIterator<int,double> it) {
	return it->second;
}
LSValue* fun_value_map_int_float_2ptr(LSMapIterator<int,double> it) {
	return LSNumber::get(it->second);
}

/*
 * Key
 */
int fun_key_array_ptr(LSArray<LSValue*>* array, LSArrayIterator<LSValue*> it) {
	return distance(array->begin(), it);
}
int fun_key_array_int(LSArray<int>* array, LSArrayIterator<int> it) {
	return distance(array->begin(), it);
}
int fun_key_array_float(LSArray<double>* array, LSArrayIterator<double> it) {
	return distance(array->begin(), it);
}
LSValue* fun_key_array_ptr_2ptr(LSArray<LSValue*>* array, LSArrayIterator<LSValue*> it) {
	return LSNumber::get(distance(array->begin(), it));
}
LSValue* fun_key_array_int_2ptr(LSArray<int>* array, LSArrayIterator<int> it) {
	return LSNumber::get(distance(array->begin(), it));
}
LSValue* fun_key_array_float_2ptr(LSArray<double>* array, LSArrayIterator<double> it) {
	return LSNumber::get(distance(array->begin(), it));
}
LSValue* fun_key_map_ptr_ptr(void*, LSMapIterator<LSValue*,LSValue*> it) {
	return it->first;
}
LSValue* fun_key_map_ptr_int(void*, LSMapIterator<LSValue*,int> it) {
	return it->first;
}
LSValue* fun_key_map_ptr_float(void*, LSMapIterator<LSValue*,double> it) {
	return it->first;
}
int fun_key_map_int_ptr(void*, LSMapIterator<int,LSValue*> it) {
	return it->first;
}
int fun_key_map_int_int(void*, LSMapIterator<int,int> it) {
	return it->first;
}
int fun_key_map_int_float(void*, LSMapIterator<int,double> it) {
	return it->first;
}
LSValue* fun_key_map_int_ptr_2ptr(void*, LSMapIterator<int,LSValue*> it) {
	return LSNumber::get(it->first);
}
LSValue* fun_key_map_int_int_2ptr(void*, LSMapIterator<int,int> it) {
	return LSNumber::get(it->first);
}
LSValue* fun_key_map_int_float_2ptr(void*, LSMapIterator<int,double> it) {
	return LSNumber::get(it->first);
}


/*
 * Increment
 */
LSArrayIterator<LSValue*> fun_inc_array_ptr(LSArrayIterator<LSValue*> it) {
	return ++it;
}
LSArrayIterator<int> fun_inc_array_int(LSArrayIterator<int> it) {
	return ++it;
}
LSArrayIterator<double> fun_inc_array_float(LSArrayIterator<double> it) {
	return ++it;
}
LSMapIterator<LSValue*,LSValue*> fun_inc_map_ptr_ptr(LSMapIterator<LSValue*,LSValue*> it) {
	return ++it;
}
LSMapIterator<LSValue*,int>fun_inc_map_ptr_int(LSMapIterator<LSValue*,int> it) {
	return ++it;
}
LSMapIterator<LSValue*,double> fun_inc_map_ptr_float(LSMapIterator<LSValue*,double> it) {
	return ++it;
}
LSMapIterator<int,LSValue*> fun_inc_map_int_ptr(LSMapIterator<int,LSValue*> it) {
	return ++it;
}
LSMapIterator<int,int> fun_inc_map_int_int(LSMapIterator<int,int> it) {
	return ++it;
}
LSMapIterator<int,double> fun_inc_map_int_float(LSMapIterator<int,double> it) {
	return ++it;
}

/*
 * Dynamic selector
 */
enum SelectorType {
	PTR_ARRAY = 0,
	INT_ARRAY,
	FLOAT_ARRAY,
	PTR_PTR_MAP,
	PTR_INT_MAP,
	PTR_FLOAT_MAP,
	INT_PTR_MAP,
	INT_INT_MAP,
	INT_FLOAT_MAP,
	OTHER_NOT_SUPPORTED
};
int fun_selector(LSValue* containter) {
	if (dynamic_cast<LSArray<LSValue*>*>(containter)) {
		return SelectorType::PTR_ARRAY;
	}
	if (dynamic_cast<LSArray<int>*>(containter)) {
		return SelectorType::INT_ARRAY;
	}
	if (dynamic_cast<LSArray<double>*>(containter)) {
		return SelectorType::FLOAT_ARRAY;
	}
	if (dynamic_cast<LSMap<LSValue*,LSValue*>*>(containter)) {
		return SelectorType::PTR_PTR_MAP;
	}
	if (dynamic_cast<LSMap<LSValue*,int>*>(containter)) {
		return SelectorType::PTR_INT_MAP;
	}
	if (dynamic_cast<LSMap<LSValue*,double>*>(containter)) {
		return SelectorType::PTR_FLOAT_MAP;
	}
	if (dynamic_cast<LSMap<int,LSValue*>*>(containter)) {
		return SelectorType::INT_PTR_MAP;
	}
	if (dynamic_cast<LSMap<int,int>*>(containter)) {
		return SelectorType::INT_INT_MAP;
	}
	if (dynamic_cast<LSMap<int,double>*>(containter)) {
		return SelectorType::INT_FLOAT_MAP;
	}

	return SelectorType::OTHER_NOT_SUPPORTED;
}

jit_value_t Foreach::compile(Compiler& c) const {

	c.enter_block(); // { for x in [1, 2] {} }<-- this block

	// Array
	jit_value_t a = container->compile(c);

	if (container->type.must_manage_memory()) {
		VM::inc_refs(c.F, a);
	}

	c.add_var("{array}", a, container->type, false);

	// Create variables
	jit_type_t jit_value_type = VM::get_jit_type(value_type);
	jit_value_t v = jit_value_create(c.F, jit_value_type);
	jit_type_t jit_key_type = VM::get_jit_type(key_type);
	jit_value_t k = key ? jit_value_create(c.F, jit_key_type) : nullptr;

	c.add_var(value->content, v, value_type, true);
	if (key) c.add_var(key->content, k, key_type, true);


	jit_label_t label_end = jit_label_undefined;
	jit_label_t label_it = jit_label_undefined;
	c.enter_loop(&label_end, &label_it);

	// Static Selector
	if (equal_type(container->type, Type::INT_ARRAY)) {
		compile_foreach(c, a, (void*) fun_begin_array_all, (void*) fun_condition_array_all, (void*) fun_value_array_int, (void*) fun_key_array_int, (void*) fun_inc_array_int,
						&label_it, &label_end, jit_value_type, v, jit_key_type, k);
	} else if (equal_type(container->type, Type::FLOAT_ARRAY)) {
		compile_foreach(c, a, (void*) fun_begin_array_all, (void*) fun_condition_array_all, (void*) fun_value_array_float, (void*) fun_key_array_float, (void*) fun_inc_array_float,
						&label_it, &label_end, jit_value_type, v, jit_key_type, k);
	} else if (equal_type(container->type, Type::PTR_ARRAY)) {
		compile_foreach(c, a, (void*) fun_begin_array_all, (void*) fun_condition_array_all, (void*) fun_value_array_ptr, (void*) fun_key_array_ptr, (void*) fun_inc_array_ptr,
						&label_it, &label_end, jit_value_type, v, jit_key_type, k);
	} else if (equal_type(container->type, Type::PTR_PTR_MAP)) {
		compile_foreach(c, a, (void*) fun_begin_map_all, (void*) fun_condition_map_all, (void*) fun_value_map_ptr_ptr, (void*) fun_key_map_ptr_ptr, (void*) fun_inc_map_ptr_ptr,
						&label_it, &label_end, jit_value_type, v, jit_key_type, k);
	} else if (equal_type(container->type, Type::PTR_INT_MAP)) {
		compile_foreach(c, a, (void*) fun_begin_map_all, (void*) fun_condition_map_all, (void*) fun_value_map_ptr_int, (void*) fun_key_map_ptr_int, (void*) fun_inc_map_ptr_int,
						&label_it, &label_end, jit_value_type, v, jit_key_type, k);
	} else if (equal_type(container->type, Type::PTR_FLOAT_MAP)) {
		compile_foreach(c, a, (void*) fun_begin_map_all, (void*) fun_condition_map_all, (void*) fun_value_map_ptr_float, (void*) fun_key_map_ptr_float, (void*) fun_inc_map_ptr_float,
						&label_it, &label_end, jit_value_type, v, jit_key_type, k);
	} else if (equal_type(container->type, Type::INT_PTR_MAP)) {
		compile_foreach(c, a, (void*) fun_begin_map_all, (void*) fun_condition_map_all, (void*) fun_value_map_int_ptr, (void*) fun_key_map_int_ptr, (void*) fun_inc_map_int_ptr,
						&label_it, &label_end, jit_value_type, v, jit_key_type, k);
	} else if (equal_type(container->type, Type::INT_INT_MAP)) {
		compile_foreach(c, a, (void*) fun_begin_map_all, (void*) fun_condition_map_all, (void*) fun_value_map_int_int, (void*) fun_key_map_int_int, (void*) fun_inc_map_int_int,
						&label_it, &label_end, jit_value_type, v, jit_key_type, k);
	} else if (equal_type(container->type, Type::INT_FLOAT_MAP)) {
		compile_foreach(c, a, (void*) fun_begin_map_all, (void*) fun_condition_map_all, (void*) fun_value_map_int_float, (void*) fun_key_map_int_float, (void*) fun_inc_map_int_float,
						&label_it, &label_end, jit_value_type, v, jit_key_type, k);
	} else if (container->type.nature == Nature::POINTER) {

		// Dynamic selector
		jit_type_t args_types_sel[1] = {JIT_POINTER};
		jit_type_t sig_sel = jit_type_create_signature(jit_abi_cdecl, JIT_INTEGER, args_types_sel, 1, 0);
		jit_value_t s = jit_insn_call_native(c.F, "selector", (void*) fun_selector, sig_sel, &a, 1, JIT_CALL_NOTHROW);

		// Goto begin
		jit_label_t label_begin = jit_label_undefined;
		jit_insn_branch(c.F, &label_begin);

		// Block
		jit_label_t label_block = jit_label_undefined;
		jit_insn_label(c.F, &label_block);
		body->compile(c);
		jit_insn_label(c.F, &label_it);


		if (container->type == Type::ARRAY) {

			jit_label_t destinations_begin[3] = {
				jit_label_undefined, jit_label_undefined, jit_label_undefined,
			};
			jit_label_t destinations_it[3] = {
				jit_label_undefined, jit_label_undefined, jit_label_undefined,
			};

			jit_insn_jump_table(c.F, s, destinations_it, 3);

			// Begin
			jit_insn_label(c.F, &label_begin);
			jit_insn_jump_table(c.F, s, destinations_begin, 3);
			jit_insn_branch(c.F, &label_end); // by default (not realy needed for the array case)


			compile_foreach_noblock(c, a, (void*) fun_begin_array_all, (void*) fun_condition_array_all, (void*) fun_value_array_ptr, (void*) fun_key_array_ptr, (void*) fun_inc_array_ptr,
									&destinations_begin[0], &destinations_it[0], &label_block, &label_end, jit_value_type, v, jit_key_type, k);
			compile_foreach_noblock(c, a, (void*) fun_begin_array_all, (void*) fun_condition_array_all, (void*) fun_value_array_int_2ptr, (void*) fun_key_array_int, (void*) fun_inc_array_int,
									&destinations_begin[1], &destinations_it[1], &label_block, &label_end, jit_value_type, v, jit_key_type, k);
			compile_foreach_noblock(c, a, (void*) fun_begin_array_all, (void*) fun_condition_array_all, (void*) fun_value_array_float_2ptr, (void*) fun_key_array_float, (void*) fun_inc_array_float,
									&destinations_begin[2], &destinations_it[2], &label_block, &label_end, jit_value_type, v, jit_key_type, k);

		} else {

			jit_label_t destinations_begin[9] = {
				jit_label_undefined, jit_label_undefined, jit_label_undefined,
				jit_label_undefined, jit_label_undefined, jit_label_undefined,
				jit_label_undefined, jit_label_undefined, jit_label_undefined,
			};
			jit_label_t destinations_it[9] = {
				jit_label_undefined, jit_label_undefined, jit_label_undefined,
				jit_label_undefined, jit_label_undefined, jit_label_undefined,
				jit_label_undefined, jit_label_undefined, jit_label_undefined,
			};

			jit_insn_jump_table(c.F, s, destinations_it, 9);

			// Begin
			jit_insn_label(c.F, &label_begin);
			jit_insn_jump_table(c.F, s, destinations_begin, 9);
			jit_insn_branch(c.F, &label_end); // by default


			compile_foreach_noblock(c, a, (void*) fun_begin_array_all, (void*) fun_condition_array_all, (void*) fun_value_array_ptr, (void*) fun_key_array_ptr_2ptr, (void*) fun_inc_array_ptr,
									&destinations_begin[0], &destinations_it[0], &label_block, &label_end, jit_value_type, v, jit_key_type, k);
			compile_foreach_noblock(c, a, (void*) fun_begin_array_all, (void*) fun_condition_array_all, (void*) fun_value_array_int_2ptr, (void*) fun_key_array_int_2ptr, (void*) fun_inc_array_int,
									&destinations_begin[1], &destinations_it[1], &label_block, &label_end, jit_value_type, v, jit_key_type, k);
			compile_foreach_noblock(c, a, (void*) fun_begin_array_all, (void*) fun_condition_array_all, (void*) fun_value_array_float_2ptr, (void*) fun_key_array_float_2ptr, (void*) fun_inc_array_float,
									&destinations_begin[2], &destinations_it[2], &label_block, &label_end, jit_value_type, v, jit_key_type, k);

			compile_foreach_noblock(c, a, (void*) fun_begin_map_all, (void*) fun_condition_map_all, (void*) fun_value_map_ptr_ptr, (void*) fun_key_map_ptr_ptr, (void*) fun_inc_map_ptr_ptr,
									&destinations_begin[3], &destinations_it[3], &label_block, &label_end, jit_value_type, v, jit_key_type, k);
			compile_foreach_noblock(c, a, (void*) fun_begin_map_all, (void*) fun_condition_map_all, (void*) fun_value_map_ptr_int_2ptr, (void*) fun_key_map_ptr_int, (void*) fun_inc_map_ptr_int,
									&destinations_begin[4], &destinations_it[4], &label_block, &label_end, jit_value_type, v, jit_key_type, k);
			compile_foreach_noblock(c, a, (void*) fun_begin_map_all, (void*) fun_condition_map_all, (void*) fun_value_map_ptr_float, (void*) fun_key_map_ptr_float, (void*) fun_inc_map_ptr_float,
									&destinations_begin[5], &destinations_it[5], &label_block, &label_end, jit_value_type, v, jit_key_type, k);
			compile_foreach_noblock(c, a, (void*) fun_begin_map_all, (void*) fun_condition_map_all, (void*) fun_value_map_int_ptr, (void*) fun_key_map_int_ptr_2ptr, (void*) fun_inc_map_int_ptr,
									&destinations_begin[6], &destinations_it[6], &label_block, &label_end, jit_value_type, v, jit_key_type, k);
			compile_foreach_noblock(c, a, (void*) fun_begin_map_all, (void*) fun_condition_map_all, (void*) fun_value_map_int_int_2ptr, (void*) fun_key_map_int_int_2ptr, (void*) fun_inc_map_int_int,
									&destinations_begin[7], &destinations_it[7], &label_block, &label_end, jit_value_type, v, jit_key_type, k);
			compile_foreach_noblock(c, a, (void*) fun_begin_map_all, (void*) fun_condition_map_all, (void*) fun_value_map_int_float_2ptr, (void*) fun_key_map_int_float_2ptr, (void*) fun_inc_map_int_float,
									&destinations_begin[8], &destinations_it[8], &label_block, &label_end, jit_value_type, v, jit_key_type, k);

		}

	}

	c.leave_loop();

	// end label:
	jit_insn_label(c.F, &label_end);

	c.leave_block(c.F); // { for x in ['a' 'b'] { ... }<--- not this block }<--- this block

//	if (container->type.nature == Nature::POINTER) {
//		VM::delete_temporary(c.F, a);
//	}
	return nullptr;
}

void Foreach::compile_foreach(Compiler&c, jit_value_t a,
							  void* fun_begin, void* fun_condition, void* fun_value, void* fun_key, void* fun_inc,
							  jit_label_t* label_it, jit_label_t* label_end,
							  jit_type_t jit_value_type, jit_value_t v, jit_type_t jit_key_type, jit_value_t k) const {

	// Labels
	jit_label_t label_cond = jit_label_undefined;

	// Variable it = begin()
	jit_value_t it = jit_value_create(c.F, JIT_POINTER);
	jit_type_t args_types_begin[1] = {JIT_POINTER};
	jit_type_t sig_begin = jit_type_create_signature(jit_abi_cdecl, JIT_POINTER, args_types_begin, 1, 0);
	jit_insn_store(c.F, it, jit_insn_call_native(c.F, "begin", (void*) fun_begin, sig_begin, &a, 1, JIT_CALL_NOTHROW));

	// cond label:
	jit_insn_label(c.F, &label_cond);

	// Condition to continue
	jit_type_t args_types_cond[2] = {JIT_POINTER, JIT_POINTER};
	jit_type_t sig_cond = jit_type_create_signature(jit_abi_cdecl, JIT_BOOLEAN, args_types_cond, 2, 0);
	jit_value_t args_cond[2] = { a, it };
	jit_value_t cond = jit_insn_call_native(c.F, "cond", (void*) fun_condition, sig_cond, args_cond, 2, JIT_CALL_NOTHROW);
	jit_insn_branch_if_not(c.F, cond, label_end);

	// Get Value
	jit_type_t args_types_value[1] = {JIT_POINTER};
	jit_type_t sig_value = jit_type_create_signature(jit_abi_cdecl, jit_value_type, args_types_value, 1, 0);
	jit_value_t vtmp = jit_insn_call_native(c.F, "value", fun_value, sig_value, &it, 1, JIT_CALL_NOTHROW);
	jit_insn_store(c.F, v, vtmp);

	// Get Key
	if (key != nullptr) {
		jit_type_t args_types_key[2] = {JIT_POINTER, JIT_POINTER};
		jit_type_t sig_key = jit_type_create_signature(jit_abi_cdecl, jit_key_type, args_types_key, 2, 0);
		jit_value_t args_key[2] = { a, it };
		jit_value_t ktmp = jit_insn_call_native(c.F, "key", (void*) fun_key, sig_key, args_key, 2, JIT_CALL_NOTHROW);
		jit_insn_store(c.F, k, ktmp);
	}

	// Body
	body->compile(c);

	// it++
	jit_insn_label(c.F, label_it);
	jit_type_t args_types_inc[1] = {JIT_POINTER};
	jit_type_t sig_inc = jit_type_create_signature(jit_abi_cdecl, JIT_POINTER, args_types_inc, 1, 0);
	jit_insn_store(c.F, it, jit_insn_call_native(c.F, "inc", fun_inc, sig_inc, &it, 1, JIT_CALL_NOTHROW));

	// jump to cond
	jit_insn_branch(c.F, &label_cond);
}

void Foreach::compile_foreach_noblock(Compiler& c, jit_value_t a,
									  void* fun_begin, void* fun_condition, void* fun_value, void* fun_key, void* fun_inc,
									  jit_label_t* label_begin, jit_label_t* label_it, jit_label_t* label_block, jit_label_t* label_end,
									  jit_type_t jit_value_type, jit_value_t v, jit_type_t jit_key_type, jit_value_t k) const
{
	// begin label
	jit_insn_label(c.F, label_begin);

	// Variable it = begin()
	jit_value_t it = jit_value_create(c.F, JIT_POINTER);
	jit_type_t args_types_begin[1] = {JIT_POINTER};
	jit_type_t sig_begin = jit_type_create_signature(jit_abi_cdecl, JIT_POINTER, args_types_begin, 1, 0);
	jit_insn_store(c.F, it, jit_insn_call_native(c.F, "begin", (void*) fun_begin, sig_begin, &a, 1, JIT_CALL_NOTHROW));


	// cond label:
	jit_label_t label_cond = jit_label_undefined;
	jit_insn_label(c.F, &label_cond);

	// Condition to continue
	jit_type_t args_types_cond[2] = {JIT_POINTER, JIT_POINTER};
	jit_type_t sig_cond = jit_type_create_signature(jit_abi_cdecl, JIT_BOOLEAN, args_types_cond, 2, 0);
	jit_value_t args_cond[2] = { a, it };
	jit_value_t cond = jit_insn_call_native(c.F, "cond", (void*) fun_condition, sig_cond, args_cond, 2, JIT_CALL_NOTHROW);
	jit_insn_branch_if_not(c.F, cond, label_end);

	// Get Value
	jit_type_t args_types_value[1] = {JIT_POINTER};
	jit_type_t sig_value = jit_type_create_signature(jit_abi_cdecl, jit_value_type, args_types_value, 1, 0);
	jit_value_t vtmp = jit_insn_call_native(c.F, "value", fun_value, sig_value, &it, 1, JIT_CALL_NOTHROW);
	jit_insn_store(c.F, v, vtmp);

	// Get Key
	if (key != nullptr) {
		jit_type_t args_types_key[2] = {JIT_POINTER, JIT_POINTER};
		jit_type_t sig_key = jit_type_create_signature(jit_abi_cdecl, jit_key_type, args_types_key, 2, 0);
		jit_value_t args_key[2] = { a, it };
		jit_value_t ktmp = jit_insn_call_native(c.F, "key", (void*) fun_key, sig_key, args_key, 2, JIT_CALL_NOTHROW);
		jit_insn_store(c.F, k, ktmp);
	}

	// Body
	jit_insn_branch(c.F, label_block);

	// it++
	jit_insn_label(c.F, label_it);
	jit_type_t args_types_inc[1] = {JIT_POINTER};
	jit_type_t sig_inc = jit_type_create_signature(jit_abi_cdecl, JIT_POINTER, args_types_inc, 1, 0);
	jit_insn_store(c.F, it, jit_insn_call_native(c.F, "inc", fun_inc, sig_inc, &it, 1, JIT_CALL_NOTHROW));

	// jump to cond
	jit_insn_branch(c.F, &label_cond);
}

bool Foreach::equal_type(const Type& generic, const Type& actual)
{
	if (generic.raw_type != actual.raw_type) return false;
	if (generic.nature != actual.nature) return false;
	if (generic.element_types.size() != actual.element_types.size()) return false;

	for (size_t i = 0; i < generic.element_types.size(); ++i) {
		const Type& a = generic.element_types[i];
		const Type& b = actual.element_types[i];
		if (a.nature != b.nature) return false;
		if (a.nature == Nature::VALUE && a.raw_type != b.raw_type) return false;
	}

	return true;
}

}
