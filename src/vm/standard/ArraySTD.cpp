#include <algorithm>
#include "ArraySTD.hpp"
#include "../value/LSNumber.hpp"
#include "../value/LSArray.hpp"

using namespace std;

namespace ls {

ArraySTD::ArraySTD() : Module("Array") {

	method("average", {
		{Type::ARRAY, Type::FLOAT, {}, (void*) &LSArray<LSValue*>::average},
		{Type::FLOAT_ARRAY, Type::FLOAT, {}, (void*) &LSArray<double>::average},
		{Type::INT_ARRAY, Type::FLOAT, {}, (void*) &LSArray<int>::average}
	});

	method("max", {
		{Type::ARRAY, Type::INTEGER_P, {}, (void*) &array_max}
	});

	method("min", {
		{Type::ARRAY, Type::INTEGER_P, {}, (void*) &array_min}
	});

	method("size", {
		{Type::ARRAY, Type::INTEGER, {}, (void*) &LSArray<LSValue*>::ls_size},
		{Type::FLOAT_ARRAY, Type::INTEGER, {}, (void*) &LSArray<double>::ls_size},
		{Type::INT_ARRAY, Type::INTEGER, {}, (void*) &LSArray<int>::ls_size}
	});

	method("sum", {
		{Type::ARRAY, Type::POINTER, {}, (void*) &LSArray<LSValue*>::sum},
		{Type::FLOAT_ARRAY, Type::FLOAT, {}, (void*) &LSArray<double>::sum},
		{Type::INT_ARRAY, Type::INTEGER, {}, (void*) &LSArray<int>::sum}
	});

	Type map_int_fun_type = Type::FUNCTION;
	map_int_fun_type.setArgumentType(0, Type::INTEGER);
	map_int_fun_type.setReturnType(Type::POINTER);

	Type map_fun_type = Type::FUNCTION;
	map_fun_type.setArgumentType(0, Type::POINTER);
	map_fun_type.setReturnType(Type::POINTER);

	method("map", {
		{Type::ARRAY, Type::ARRAY, {map_fun_type}, (void*) &LSArray<LSValue*>::ls_map},
		{Type::INT_ARRAY, Type::ARRAY, {map_int_fun_type}, (void*) &LSArray<int>::ls_map},
	});

	method("chunk", {
		{Type::ARRAY, Type::ARRAY, {}, (void*) array_chunk_1_ptr},
		{Type::FLOAT_ARRAY, Type::FLOAT_ARRAY, {}, (void*) array_chunk_1_float},
		{Type::INT_ARRAY, Type::INT_ARRAY, {}, (void*) array_chunk_1_int},

		{Type::ARRAY, Type::ARRAY, {Type::INTEGER}, (void*) &LSArray<LSValue*>::ls_chunk},
		{Type::FLOAT_ARRAY, Type::FLOAT_ARRAY, {Type::INTEGER}, (void*) &LSArray<double>::ls_chunk},
		{Type::INT_ARRAY, Type::INT_ARRAY, {Type::INTEGER}, (void*) &LSArray<int>::ls_chunk},
    });

	method("unique", {
		{Type::ARRAY, Type::ARRAY, {}, (void*) &LSArray<LSValue*>::ls_unique},
		{Type::FLOAT_ARRAY, Type::FLOAT_ARRAY, {}, (void*) &LSArray<double>::ls_unique},
		{Type::INT_ARRAY, Type::INT_ARRAY, {}, (void*) &LSArray<int>::ls_unique},
	});

	method("sort", {
		{Type::ARRAY, Type::ARRAY, {}, (void*) &LSArray<LSValue*>::ls_sort},
		{Type::FLOAT_ARRAY, Type::FLOAT_ARRAY, {}, (void*) &LSArray<double>::ls_sort},
		{Type::INT_ARRAY, Type::INT_ARRAY, {}, (void*) &LSArray<int>::ls_sort},
	});

	Type map2_fun_type = Type::FUNCTION;
	map2_fun_type.setArgumentType(0, Type::POINTER);
	map2_fun_type.setArgumentType(1, Type::POINTER);
	map2_fun_type.setReturnType(Type::POINTER);

	Type map2_fun_type_int = Type::FUNCTION;
	map2_fun_type_int.setArgumentType(0, Type::POINTER);
	map2_fun_type_int.setArgumentType(1, Type::INTEGER);
	map2_fun_type_int.setReturnType(Type::POINTER);

	method("map2", {
		{Type::ARRAY, Type::ARRAY, {Type::ARRAY, map2_fun_type}, (void*) &LSArray<LSValue*>::map2},
		{Type::ARRAY, Type::ARRAY, {Type::INT_ARRAY, map2_fun_type_int}, (void*) &LSArray<LSValue*>::map2_int},
	});

	Type iter_fun_type = Type::FUNCTION_P;
	iter_fun_type.setArgumentType(0, Type::POINTER);
	iter_fun_type.setReturnType(Type::POINTER);
	method("iter", Type::ARRAY, Type::POINTER, {iter_fun_type},(void*) &array_iter);

	Type filter_fun_type = Type::FUNCTION;
	filter_fun_type.setArgumentType(0, Type::POINTER);
	filter_fun_type.setReturnType(Type::BOOLEAN);
	Type filter_fun_type_int = Type::FUNCTION;
	filter_fun_type_int.setArgumentType(0, Type::INTEGER);
	filter_fun_type_int.setReturnType(Type::BOOLEAN);
	method("filter", {
		{Type::ARRAY, Type::ARRAY, {filter_fun_type}, (void*) &LSArray<LSValue*>::filter},
		{Type::ARRAY, Type::ARRAY, {filter_fun_type_int}, (void*) &LSArray<int>::filter},
	});

	method("contains", {
		{Type::ARRAY, Type::BOOLEAN, {Type::POINTER}, (void*) &LSArray<LSValue*>::contains},
		{Type::ARRAY, Type::BOOLEAN, {Type::INTEGER}, (void*) &LSArray<int>::contains_int}
	});

	method("isEmpty", Type::ARRAY, Type::BOOLEAN_P, {}, (void*) &array_isEmpty);

	Type partition_fun_type = Type::FUNCTION;
	partition_fun_type.setArgumentType(0, Type::POINTER);
	partition_fun_type.setReturnType(Type::BOOLEAN);

	Type partition_fun_type_int = Type::FUNCTION;
	partition_fun_type_int.setArgumentType(0, Type::INTEGER);
	partition_fun_type_int.setReturnType(Type::BOOLEAN);

	method("partition", {
		{Type::ARRAY, Type::ARRAY, {partition_fun_type}, (void*) &LSArray<LSValue*>::partition},
		{Type::ARRAY, Type::ARRAY, {partition_fun_type_int}, (void*) &LSArray<int>::partition}
	});

	method("first", {
		{Type::ARRAY, Type::POINTER, {}, (void*) &LSArray<LSValue*>::ls_first},
		{Type::FLOAT_ARRAY, Type::POINTER, {}, (void*) &LSArray<double>::ls_first},
		{Type::INT_ARRAY, Type::POINTER, {}, (void*) &LSArray<int>::ls_first},
	});
	method("last", {
		{Type::ARRAY, Type::POINTER, {}, (void*) &LSArray<LSValue*>::ls_last},
		{Type::FLOAT_ARRAY, Type::POINTER, {}, (void*) &LSArray<double>::ls_last},
		{Type::INT_ARRAY, Type::POINTER, {}, (void*) &LSArray<int>::ls_last},
	});

	Type fold_fun_type = Type::FUNCTION_P;
	fold_fun_type.setArgumentType(0, Type::POINTER);
	fold_fun_type.setArgumentType(1, Type::POINTER);
	fold_fun_type.setReturnType(Type::POINTER);
	Type fold_fun_type_int = Type::FUNCTION_P;
	fold_fun_type_int.setArgumentType(0, Type::POINTER);
	fold_fun_type_int.setArgumentType(1, Type::POINTER);
	fold_fun_type_int.setReturnType(Type::POINTER);
	method("foldLeft", {
		{Type::ARRAY, Type::POINTER, {fold_fun_type, Type::POINTER}, (void*) &LSArray<LSValue*>::foldLeft},
		{Type::ARRAY, Type::POINTER, {fold_fun_type_int, Type::POINTER}, (void*) &array_foldLeft}
	});
	method("foldRight", Type::ARRAY, Type::POINTER, {fold_fun_type, Type::POINTER}, (void*)&array_foldRight);

	method("shuffle", Type::ARRAY, Type::ARRAY, {}, (void*)&array_shuffle);

	method("search", {
		{Type::ARRAY, Type::INTEGER, {Type::POINTER, Type::INTEGER}, (void*) &LSArray<LSValue*>::search},
		{Type::ARRAY, Type::INTEGER, {Type::INTEGER, Type::INTEGER}, (void*) &LSArray<int>::search_int}
	});

	method("pop", {
		{Type::ARRAY, Type::POINTER, {}, (void*) &LSArray<LSValue*>::ls_pop},
		{Type::FLOAT_ARRAY, Type::POINTER, {}, (void*) &LSArray<double>::ls_pop},
		{Type::INT_ARRAY, Type::POINTER, {}, (void*) &LSArray<int>::ls_pop}
	});

	method("push", {
		{Type::ARRAY, Type::ARRAY, {Type::POINTER}, (void*) &LSArray<LSValue*>::push},
		{Type::FLOAT_ARRAY, Type::FLOAT_ARRAY, {Type::FLOAT}, (void*) &LSArray<double>::push},
		{Type::INT_ARRAY, Type::INT_ARRAY, {Type::INTEGER}, (void*) &LSArray<int>::push},
	});

	method("pushAll", {
		{Type::ARRAY, Type::ARRAY, {Type::ARRAY}, (void*) &LSArray<LSValue*>::push_all},
		{Type::ARRAY, Type::INT_ARRAY, {Type::INT_ARRAY}, (void*) &LSArray<int>::push_all_int}
	});

	method("join", {
		{Type::ARRAY, Type::STRING, {Type::STRING}, (void*) &LSArray<LSValue*>::join},
//		{Type::STRING, {Type::STRING}, (void*) &LSArray<int>::join}
	});

	method("clear", Type::ARRAY, Type::ARRAY, {}, (void*)&array_clear);

	method("fill", {
		{Type::ARRAY, Type::ARRAY, {Type::POINTER, Type::INTEGER}, (void*) &LSArray<LSValue*>::fill},
		{Type::ARRAY, Type::INT_ARRAY, {Type::INTEGER_P, Type::INTEGER}, (void*) &LSArray<int>::fill}
	});

	method("insert", {
		{Type::ARRAY, Type::ARRAY, {Type::POINTER, Type::POINTER}, (void*) &LSArray<LSValue*>::insert_v},
		{Type::ARRAY, Type::INT_ARRAY, {Type::INTEGER, Type::POINTER}, (void*) &LSArray<int>::insert_v}
	});

	method("remove", {
		{Type::ARRAY, Type::POINTER, {Type::POINTER}, (void*)&LSArray<LSValue*>::remove}
	});

	method("removeKey", {
		{Type::ARRAY, Type::POINTER, {Type::POINTER}, (void*)&LSArray<LSValue*>::remove_key}
	});

	method("removeElement", {
		{Type::ARRAY, Type::POINTER, {Type::POINTER}, (void*)&LSArray<LSValue*>::remove_element},
		{Type::ARRAY, Type::INTEGER, {Type::INTEGER}, (void*)&LSArray<int>::remove_element},
	});

	method("reverse", {
		{Type::ARRAY, Type::ARRAY, {}, (void*) &LSArray<LSValue*>::reverse},
		{Type::INT_ARRAY, Type::INT_ARRAY, {}, (void*) &LSArray<int>::reverse},
	});

	/*
	 * Static methods
	 */
	static_method("average", {
		{Type::FLOAT, {Type::ARRAY}, (void*) &LSArray<LSValue*>::average},
		{Type::FLOAT, {Type::INT_ARRAY}, (void*) &LSArray<int>::average}
	});

	static_method("max", Type::INTEGER_P, {Type::ARRAY}, (void*) &array_max);
	static_method("min", Type::INTEGER_P, {Type::ARRAY}, (void*) &array_min);

	static_method("size", {
		{Type::INTEGER, {Type::ARRAY}, (void*) &LSArray<LSValue*>::ls_size},
		{Type::INTEGER, {Type::FLOAT_ARRAY}, (void*) &LSArray<double>::ls_size},
		{Type::INTEGER, {Type::INT_ARRAY}, (void*) &LSArray<int>::ls_size}
	});

	static_method("sum", {
		{Type::POINTER, {Type::ARRAY}, (void*) &LSArray<LSValue*>::sum},
		{Type::INTEGER, {Type::INT_ARRAY}, (void*) &LSArray<int>::sum}
	});

	static_method("map", {
		{Type::ARRAY, {Type::ARRAY, map_fun_type}, (void*) &LSArray<LSValue*>::ls_map},
		{Type::ARRAY, {Type::INT_ARRAY, map_int_fun_type}, (void*) &LSArray<int>::ls_map},
	});

	static_method("map2", {
		{Type::ARRAY, {Type::ARRAY, Type::ARRAY, map2_fun_type}, (void*) &LSArray<LSValue*>::map2},
		{Type::ARRAY, {Type::ARRAY, Type::INT_ARRAY, map2_fun_type_int}, (void*) &LSArray<LSValue*>::map2_int},
	});

	static_method("iter", Type::POINTER, {Type::ARRAY, iter_fun_type},(void*)&array_iter);

	static_method("filter", {
		{Type::ARRAY, {Type::ARRAY, filter_fun_type}, (void*) &LSArray<LSValue*>::filter},
		{Type::INT_ARRAY, {Type::INT_ARRAY, filter_fun_type_int}, (void*) &LSArray<int>::filter},
	});

	static_method("contains", {
		{Type::BOOLEAN_P, {Type::ARRAY, Type::POINTER}, (void*) &array_contains},
		{Type::BOOLEAN_P, {Type::ARRAY, Type::INTEGER}, (void*) &array_contains_int}
	});
	static_method("isEmpty",Type::BOOLEAN_P, {Type::ARRAY}, (void*)&array_isEmpty);

	static_method("partition", {
		{Type::ARRAY, {Type::ARRAY, partition_fun_type}, (void*) &LSArray<LSValue*>::partition},
		{Type::INT_ARRAY, {Type::INT_ARRAY, partition_fun_type_int}, (void*) &LSArray<int>::partition}
	});

	static_method("first", {
		{Type::POINTER, {Type::ARRAY}, (void*) &LSArray<LSValue*>::ls_first},
		{Type::POINTER, {Type::FLOAT_ARRAY}, (void*) &LSArray<double>::ls_first},
		{Type::POINTER, {Type::INT_ARRAY}, (void*) &LSArray<int>::ls_first},
	});
	static_method("last", {
		{Type::POINTER, {Type::ARRAY}, (void*) &LSArray<LSValue*>::ls_last},
		{Type::POINTER, {Type::FLOAT_ARRAY}, (void*) &LSArray<double>::ls_last},
		{Type::POINTER, {Type::INT_ARRAY}, (void*) &LSArray<int>::ls_last},
	});

	static_method("foldLeft", {
		{Type::POINTER, {Type::ARRAY, fold_fun_type, Type::POINTER}, (void*) &array_foldLeft},
		{Type::POINTER, {Type::ARRAY, fold_fun_type_int, Type::POINTER}, (void*) &array_foldLeft}
	});

	static_method("foldRight", {
		{Type::POINTER, {Type::ARRAY, fold_fun_type, Type::POINTER}, (void*)&array_foldRight},
		{Type::POINTER, {Type::ARRAY, fold_fun_type_int, Type::POINTER}, (void*)&array_foldRight}
	});

	static_method("reverse", Type::ARRAY, {Type::ARRAY}, (void*) &array_reverse);
	static_method("shuffle", Type::ARRAY, {Type::ARRAY}, (void*)&array_shuffle);

	static_method("search", {
		{Type::INTEGER, {Type::ARRAY, Type::POINTER, Type::INTEGER}, (void*) &LSArray<LSValue*>::search},
		{Type::INTEGER, {Type::INT_ARRAY, Type::INTEGER, Type::INTEGER}, (void*) &LSArray<int>::search_int}
	});

	static_method("subArray", {
		{Type::ARRAY, {Type::ARRAY, Type::INTEGER, Type::INTEGER}, (void* )&LSArray<LSValue*>::range}
	});

	static_method("pop", {
		{Type::POINTER, {Type::ARRAY}, (void*) &LSArray<LSValue*>::ls_pop},
		{Type::INTEGER, {Type::FLOAT_ARRAY}, (void*) &LSArray<double>::ls_pop},
		{Type::INTEGER, {Type::INT_ARRAY}, (void*) &LSArray<int>::ls_pop}
	});

	static_method("push", {
		{Type::ARRAY, {Type::ARRAY, Type::POINTER}, (void*) &LSArray<LSValue*>::push},
		{Type::INT_ARRAY, {Type::INT_ARRAY, Type::INTEGER}, (void*) &LSArray<int>::push}
	});

	static_method("pushAll", Type::ARRAY, {Type::ARRAY, Type::ARRAY}, (void*)&array_pushAll);
	static_method("concat", Type::ARRAY, {Type::ARRAY, Type::ARRAY}, (void*) &array_concat);

	static_method("join", {
		{Type::STRING, {Type::ARRAY, Type::STRING}, (void*) &LSArray<LSValue*>::join},
		{Type::STRING, {Type::INT_ARRAY, Type::STRING}, (void*) &LSArray<int>::join}
	});

	static_method("clear", Type::ARRAY, {Type::ARRAY}, (void*)&array_clear);

	static_method("fill", {
		{Type::ARRAY, {Type::ARRAY, Type::POINTER, Type::INTEGER}, (void*) &LSArray<LSValue*>::fill},
		{Type::INT_ARRAY, {Type::INT_ARRAY, Type::POINTER, Type::INTEGER}, (void*) &LSArray<int>::fill}
	});

	static_method("insert", {
		{Type::ARRAY, {Type::ARRAY, Type::POINTER, Type::POINTER}, (void*) &LSArray<LSValue*>::insert_v},
		{Type::INT_ARRAY, {Type::INT_ARRAY, Type::INTEGER, Type::POINTER}, (void*) &LSArray<int>::insert_v}
	});

	static_method("remove", {
		{Type::POINTER, {Type::ARRAY, Type::INTEGER}, (void*) &LSArray<LSValue*>::remove},
		{Type::INTEGER, {Type::INT_ARRAY, Type::INTEGER}, (void*) &LSArray<int>::remove}
	});

	static_method("removeKey", {
		{Type::POINTER, {Type::ARRAY, Type::POINTER}, (void*) &LSArray<LSValue*>::remove_key}
	});

	static_method("removeElement", {
		{Type::ARRAY, {Type::ARRAY, Type::POINTER}, (void*) &LSArray<LSValue*>::remove_element},
		{Type::INTEGER, {Type::INT_ARRAY, Type::INTEGER}, (void*) &LSArray<int>::remove_element}
	});
}


LSArray<LSValue*>* array_concat(const LSArray<LSValue*>* array1, const LSArray<LSValue*>* array2) {
	return (LSArray<LSValue*>*) array1->operator + (array2);
}

LSArray<LSValue*>* array_clear(LSArray<LSValue*>* array) {
	array->clear();
	return array;
}

LSValue* array_fill(LSArray<LSValue*>* array, const LSValue* value, const LSNumber* size) {
	array->clear();
	for (int i = 0; i < (int) size->value; i++) {
		array->push_clone((LSValue*) value);
	}
	return array;
}

LSArray<LSValue*>* array_filter(const LSArray<LSValue*>* array, const void* function) {
	return array->filter(function);
}

LSArray<LSValue*>* array_flatten(const LSArray<LSValue*>*, const LSNumber*) {
	// TODO
	return new LSArray<LSValue*>();
}

LSValue* array_foldLeft(const LSArray<LSValue*>* array, const LSFunction* function, const LSValue* v0) {
	return array->foldLeft(function->function, v0);
}

LSValue* array_foldRight(const LSArray<LSValue*>* array, const LSFunction* function, const LSValue* v0) {
	return array->foldRight(function->function, v0);
}

LSValue* array_iter(LSArray<LSValue*>* array, const LSFunction* function) {
	array->iter(function);
	return array;
}

LSValue* array_contains(const LSArray<LSValue*>* array, const LSValue* value) {
	return LSBoolean::get(array->contains(value));
}

LSValue* array_contains_int(const LSArray<int>* array, int value) {
	return LSBoolean::get(array->contains_int(value));
}

LSValue* array_insert(LSArray<LSValue*>* array, const LSValue* element, const LSValue* index) {
	return array->insert_v((LSValue*) element, index);
}

LSValue* array_isEmpty(const LSArray<LSValue*>* array) {
	return LSBoolean::get(array->size() == 0);
}

LSValue* array_max(const LSArray<LSValue*>*) {
	/*
	if (array->values.size() == 0) {
		return LSNumber::get(0);
	}
	auto it = array->values.begin();
	double max = ((LSNumber*) it->second)->value;
	it++;
	while (it != array->values.end()) {
		double val = ((LSNumber*) it->second)->value;
		if (val > max) {
			max = val;
		}
		it++;
	}
	return LSNumber::get(max);
	*/
	return LSNumber::get(0);
}

LSValue* array_min(const LSArray<LSValue*>*) {
	/*
	if (array->values.size() == 0) {
		return LSNumber::get(0);
	}
	auto it = array->values.begin();
	double min = ((LSNumber*) it->second)->value;
	it++;
	while (it != array->values.end()) {
		double val = ((LSNumber*) it->second)->value;
		if (val < min) {
			min = val;
		}
		it++;
	}
	return LSNumber::get(min);
	*/
	return LSNumber::get(0);
}

LSValue* array_pushAll(LSArray<LSValue*>* array, const LSArray<LSValue*>* elements) {
	array->push_all(elements);
	return array;
}

LSValue* array_remove(LSArray<LSValue*>*, const LSValue*) {
	/*
	if (not array->associative and index->isInteger() and ((LSNumber*)index)->value < array ->index and ((LSNumber*)index)->value >= 0) {
		return array->remove((LSNumber*) index);
	} else {
		return array->removeKey((LSValue*) index);
	}
	*/
	return LSNull::get();
}

LSValue* array_removeElement(LSArray<LSValue*>*, const LSValue*) {
	/*
	for (auto i = array->values.begin(); i != array->values.end(); i++) {
		if (i->second->operator ==(element)){
			if (array->associative) {
				array->removeKey(i->first);
			} else {
				array->remove((LSNumber*) i->first);
			}
			break;
		}
	}
	*/
	return LSNull::get();
}

LSValue* array_removeKey(LSArray<LSValue*>* array, const LSValue* index) {
	return array->remove_key((LSValue*) index);
}

LSValue* array_reverse(const LSArray<LSValue*>* array) {
	return array->reverse();
}

LSValue* array_search(const LSArray<LSValue*>*, const LSValue*, const LSValue*) {
	/*
	for (auto i = array->values.begin(); i != array->values.end(); i++) {
		if (start->operator < (i->first)) continue; // i < start
		if (value->operator == (i->second))
			return i->first->clone();
	}
	*/
	return LSNull::get();
}

LSValue* array_shift(const LSArray<LSValue*>*) {
	// TODO
	return new LSArray<LSValue*>();
}

LSArray<LSValue*>* array_shuffle(const LSArray<LSValue*>*) {
	LSArray<LSValue*>* new_array = new LSArray<LSValue*>();
	/*
	if (array->values.empty()) {
		return new_array;
	}
	vector<LSValue*> shuffled_values;
	for (auto it = array->values.begin(); it != array->values.end(); it++) {
		shuffled_values.push_back(it->second);
	}
	random_shuffle(shuffled_values.begin(), shuffled_values.end());
	for (auto it = shuffled_values.begin(); it != shuffled_values.end(); it++) {
		new_array->pushClone(*it);
	}
	*/
	return new_array;
}

LSValue* array_sum(const LSArray<LSValue*>*) {
//	return LSNumber::get(array->sum());
	return LSNumber::get(0);
}

LSValue* array_unshift(const LSArray<LSValue*>*, const LSValue*) {
	// TODO
	return new LSArray<LSValue*>();
}

LSArray<LSValue*>* array_chunk_1_ptr(const LSArray<LSValue*>* array) {
	return array->ls_chunk(1);
}

LSArray<LSValue*>* array_chunk_1_int(const LSArray<int>* array) {
	return array->ls_chunk(1);
}

LSArray<LSValue*>* array_chunk_1_float(const LSArray<double>* array) {
	return array->ls_chunk(1);
}

}
