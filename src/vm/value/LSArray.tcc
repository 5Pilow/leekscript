#ifndef LS_ARRAY_TCC
#define LS_ARRAY_TCC

#include <algorithm>
#include "../LSValue.hpp"
#include "LSNull.hpp"
#include "LSNumber.hpp"
#include "LSBoolean.hpp"
#include "LSFunction.hpp"
#include "LSClosure.hpp"
#include "LSString.hpp"
#include "LSObject.hpp"
#include "LSSet.hpp"

namespace ls {

template <class T>
LSArray<T>* LSArray<T>::constructor(int capacity) {
	auto array = new LSArray<T>();
	array->reserve(capacity);
	return array;
}

template <>
inline LSArray<LSValue*>::~LSArray() {
	for (auto v : *this) {
		LSValue::delete_ref(v);
	}
}
template <typename T>
LSArray<T>::~LSArray() {}

template <>
inline void LSArray<LSValue*>::push_clone(LSValue* value) {
	this->push_back(value->clone_inc());
}
template <typename T>
void LSArray<T>::push_clone(T value) {
	this->push_back(value);
}

template <>
inline void LSArray<LSValue*>::push_move(LSValue* value) {
	this->push_back(value->move_inc());
}
template <typename T>
void LSArray<T>::push_move(T value) {
	this->push_back(value);
}

template <>
inline void LSArray<LSValue*>::push_inc(LSValue* value) {
	if (!value->native) value->refs++;
	this->push_back(value);
}
template <class T>
inline void LSArray<T>::push_inc(T value) {
	this->push_back(value);
}
template <class T>
void LSArray<T>::std_push_inc(LSArray<T>* array, T value) {
	array->push_inc(value);
}

template <class T>
LSArray<T>::LSArray() : LSValue(LSValue::ARRAY) {}

template <class T>
LSArray<T>::LSArray(std::initializer_list<T> values_list) : LSArray<T>() {
	for (auto i : values_list) {
		this->push_back(i);
	}
}

template <class T>
LSArray<T>::LSArray(const std::vector<T>& vec) : LSValue(LSValue::ARRAY), std::vector<T>(vec) {}

template <class T>
LSArray<T>::LSArray(size_t size) : LSValue(LSValue::ARRAY) {
	this->reserve(size);
}

template <>
inline LSArray<LSValue*>::LSArray(const LSArray<LSValue*>& other) : LSValue(other), std::vector<LSValue*>() {
	reserve(other.size());
	for (LSValue* v : other) {
		push_back(v->clone_inc());
	}
}
template <typename T>
inline LSArray<T>::LSArray(const LSArray<T>& other) : LSValue(other), std::vector<T>(other) {
}

template <typename T>
int LSArray<T>::int_size(LSArray<T>* array) {
	return array->size();
}

template <>
inline LSArray<LSValue*>* LSArray<LSValue*>::ls_clear(LSArray<LSValue*>* array) {
	for (auto v : *array) {
		LSValue::delete_ref(v);
	}
	array->clear();
	return new LSArray<LSValue*>();
}
template <class T>
inline LSArray<LSValue*>* LSArray<T>::ls_clear(LSArray<T>* array) {
	array->clear();
	LSValue::delete_temporary(array);
	return new LSArray<LSValue*>();
}

template <>
inline LSValue* LSArray<LSValue*>::ls_remove(LSArray<LSValue*>* array, int index) {
	LSValue* previous = array->operator [] (index);
	array->erase(array->begin() + index);
	previous->refs--;
	if (array->refs == 0) delete array;
	return previous;
}

template <typename T>
inline T LSArray<T>::ls_remove(LSArray<T>* array, int index) {
	T previous = array->operator [] (index);
	array->erase(array->begin() + index);
	if (array->refs == 0) delete array;
	return previous;
}

template <>
inline bool LSArray<LSValue*>::ls_remove_element(LSArray<LSValue*>* array, LSValue* element) {
	for (size_t i = 0; i < array->size(); ++i) {
		if (*(*array)[i] == *element) {
			LSValue::delete_temporary(element);
			LSValue::delete_ref(array->operator[] (i));
			(*array)[i] = array->back();
			array->pop_back();
			if (array->refs == 0) delete array;
			return true;
		}
	}
	LSValue::delete_temporary(element);
	if (array->refs == 0) delete array;
	return false;
}

template <typename T>
inline bool LSArray<T>::ls_remove_element(LSArray<T>* array, T element) {
	for (size_t i = 0; i < array->size(); ++i) {
		if ((*array)[i] == element) {
			(*array)[i] = array->back();
			array->pop_back();
			if (array->refs == 0) delete array;
			return true;
		}
	}
	if (array->refs == 0) delete array;
	return false;
}

template <>
inline LSValue* LSArray<LSValue*>::ls_sum(LSArray<LSValue*>* array) {
	if (array->size() == 0) return LSNumber::get(0);
	LSValue* sum = array->operator [] (0)->clone();
	for (size_t i = 1; i < array->size(); ++i) {
		sum = sum->add((*array)[i]);
	}
	if (array->refs == 0) delete array;
	return sum;
}

template <class T>
T LSArray<T>::ls_sum(LSArray<T>* array) {
	T sum = 0;
	for (auto v : *array) {
		sum += v;
	}
	if (array->refs == 0) delete array;
	return sum;
}

template <typename T>
inline T LSArray<T>::ls_product(LSArray<T>* array) {
	T product = 1;
	for (auto v : *array) {
		product *= v;
	}
	if (array->refs == 0) delete array;
	return product;
}

template <>
inline double LSArray<LSValue*>::ls_average(LSArray<LSValue*>* array) {
	if (array->refs == 0) delete array;
	return 0; // No average for a no integer array
}

template <class T>
double LSArray<T>::ls_average(LSArray<T>* array) {
	if (array->size() == 0) {
		LSValue::delete_temporary(array);
		return 0;
	}
	int size = array->size();
	double sum = ls_sum(array); // this will be destroyed in ls_sum()
	return sum / size;
}

template <>
inline LSValue* LSArray<LSValue*>::ls_first(LSArray<LSValue*>* array) {
	if (array->size() == 0) {
		if (array->refs == 0) {
			delete array;
		}
		throw vm::ExceptionObj(vm::Exception::ARRAY_OUT_OF_BOUNDS);
	}
	auto first = array->front();
	if (array->refs == 0) {
		first->refs++;
		delete array;
		first->refs--;
	}
	return first->move();
}

template <class T>
inline T LSArray<T>::ls_first(LSArray<T>* array) {
	if (array->size() == 0) {
		LSValue::delete_temporary(array);
		throw vm::ExceptionObj(vm::Exception::ARRAY_OUT_OF_BOUNDS);
	}
	T first = array->front();
	LSValue::delete_temporary(array);
	return first;
}

template <>
inline LSValue* LSArray<LSValue*>::ls_last(LSArray<LSValue*>* array) {
	if (array->size() == 0) {
		LSValue::delete_temporary(array);
		throw vm::ExceptionObj(vm::Exception::ARRAY_OUT_OF_BOUNDS);
	}
	auto last = array->back();
	if (array->refs == 0) {
		last->refs++;
		delete array;
		last->refs--;
	}
	return last->move();
}

template <class T>
inline T LSArray<T>::ls_last(LSArray<T>* array) {
	if (array->size() == 0) {
		LSValue::delete_temporary(array);
		throw vm::ExceptionObj(vm::Exception::ARRAY_OUT_OF_BOUNDS);
	}
	T last = array->back();
	LSValue::delete_temporary(array);
	return last;
}

template <typename T>
inline bool LSArray<T>::ls_empty(LSArray<T>* array) {
	bool e = array->empty();
	if (array->refs == 0) delete array;
	return e;
}

template <>
inline LSValue* LSArray<LSValue*>::ls_pop(LSArray<LSValue*>* array) {
	if (array->empty()) {
		if (array->refs == 0) {
			delete array;
		}
		throw vm::ExceptionObj(vm::Exception::ARRAY_OUT_OF_BOUNDS);
	}
	LSValue* last = array->back();
	last->refs--;
	array->pop_back();
	if (array->refs == 0) {
		delete array;
	}
	return last->move();
}

template <class T>
inline T LSArray<T>::ls_pop(LSArray<T>* array) {
	if (array->empty()) {
		if (array->refs == 0) {
			delete array;
		}
		throw vm::ExceptionObj(vm::Exception::ARRAY_OUT_OF_BOUNDS);
	}
	T last = array->back();
	array->pop_back();
	if (array->refs == 0) {
		delete array;
	}
	return last;
}

template <typename T>
inline int LSArray<T>::ls_size(LSArray<T>* array) {
	int s = array->size();
	LSValue::delete_temporary(array);
	return s;
}

template <typename T>
inline LSValue* LSArray<T>::ls_size_ptr(LSArray<T>* array) {
	int s = array->size();
	LSValue::delete_temporary(array);
	return LSNumber::get(s);
}

template <class T>
template <class F, class R>
LSArray<R>* LSArray<T>::ls_map(LSArray<T>* array, F function) {
	auto result = new LSArray<R>();
	result->reserve(array->size());
	for (auto v : *array) {
		auto x = ls::clone(v);
		ls::increfs(x);
		auto r = ls::call<R>(function, x);
		result->push_move(r);
		ls::unref(x);
	}
	LSValue::delete_temporary(array);
	return result;
}

template <typename T>
inline LSArray<LSValue*>* LSArray<T>::ls_chunk(LSArray<T>* array, int size) {
	if (size <= 0) size = 1;
	auto new_array = new LSArray<LSValue*>();
	new_array->reserve(array->size() / size + 1);
	size_t i = 0;
	while (i < array->size()) {
		auto sub_array = new LSArray<T>();
		sub_array->reserve(size);
		size_t j = std::min(i + size, array->size());
		if (array->refs == 0) {
			for (; i < j; ++i) {
				sub_array->push_inc((*array)[i]);
			}
		} else {
			for (; i < j; ++i) {
				sub_array->push_clone((*array)[i]);
			}
		}
		new_array->push_inc(sub_array);
	}
	if (array->refs == 0) {
		delete array;
	}
	return new_array;
}

template <>
inline LSArray<LSValue*>* LSArray<LSValue*>::ls_unique(LSArray<LSValue*>* array) {
	if (array->empty()) return array;
	auto it = array->begin();
	auto next = it;
	while (true) {
		++next;
		while (next != array->end() && (**next) == (**it)) {
			LSValue::delete_ref(*next);
			next++;
		}
		++it;
		if (next == array->end()) {
			break;
		}
		*it = *next;
	}
	array->resize(std::distance(array->begin(), it));
	return array;
}

template <class T>
LSArray<T>* LSArray<T>::ls_unique(LSArray<T>* array) {
	auto it = std::unique(array->begin(), array->end());
	array->resize(std::distance(array->begin(), it));
	return array;
}

template <>
inline LSArray<LSValue*>* LSArray<LSValue*>::ls_sort(LSArray<LSValue*>* array) {
	std::sort(array->begin(), array->end(), [](LSValue* a, LSValue* b) -> bool {
		return *a < *b;
	});
	return array;
}

template <class T>
LSArray<T>* LSArray<T>::ls_sort(LSArray<T>* array) {
	std::sort(array->begin(), array->end());
	return array;
}

template <class T>
template <class F>
LSArray<T>* LSArray<T>::ls_sort_fun(LSArray<T>* array, F function) {
	std::sort(array->begin(), array->end(), [&](T a, T b) {
		return ls::call<bool>(function, a, b);
	});
	return array;
}

template <class T>
template <class F>
void LSArray<T>::ls_iter(LSArray<T>* array, F function) {
	for (auto v : *array) {
		auto r = ls::call<LSValue*>(function, v);
		ls::release(r); // TODO not good for primitive type
	}
	if (array->refs == 0) {
		delete array;
	}
}

template <>
inline bool LSArray<LSValue*>::ls_contains(LSArray<LSValue*>* array, LSValue* val) {
	for (auto v : *array) {
		if (*v == *val) {
			if (array->refs == 0) delete array;
			if (val->refs == 0) delete val;
			return true;
		}
	}
	if (array->refs == 0) delete array;
	if (val->refs == 0) delete val;
	return false;
}

template <class T>
bool LSArray<T>::ls_contains(LSArray<T>* array, T val) {
	for (auto v : *array) {
		if (v == val) {
			if (array->refs == 0) delete array;
			return true;
		}
	}
	if (array->refs == 0) delete array;
	return false;
}

template <>
inline LSArray<LSValue*>* LSArray<LSValue*>::ls_push(LSArray<LSValue*>* array, LSValue* val) {
	array->push_back(val->move_inc());
	return array;
}

template <class T>
LSArray<T>* LSArray<T>::ls_push(LSArray<T>* array, T val) {
	array->push_back(val);
	return array;
}

template <>
inline LSArray<LSValue*>* LSArray<LSValue*>::ls_push_all_ptr(LSArray<LSValue*>* array1, LSArray<LSValue*>* array) {
	array1->reserve(array1->size() + array->size());
	if (array->refs == 0) {
		for (auto v : *array) {
			array1->push_back(v);
		}
		array->clear();
		delete array;
	} else {
		for (auto v : *array) {
			array1->push_clone(v);
		}
	}
	return array1;
}

template <typename T>
inline LSArray<T>* LSArray<T>::ls_push_all_ptr(LSArray<T>* array1, LSArray<LSValue*>* array) {
	if (array->refs == 0) delete array;
	return array1;
}

template <>
inline LSArray<LSValue*>* LSArray<LSValue*>::ls_push_all_int(LSArray<LSValue*>* array1, LSArray<int>* array) {
	array1->reserve(array1->size() + array->size());
	for (int v : *array) {
		array1->push_inc(LSNumber::get(v));
	}
	if (array->refs == 0) delete array;
	return array1;
}

template <typename T>
inline LSArray<T>* LSArray<T>::ls_push_all_int(LSArray<T>* array1, LSArray<int>* array) {
	array1->reserve(array1->size() + array->size());
	array1->insert(array1->end(), array->begin(), array->end());
	if (array->refs == 0) delete array;
	return array1;
}

template <>
inline LSArray<LSValue*>* LSArray<LSValue*>::ls_push_all_flo(LSArray<LSValue*>* array1, LSArray<double>* array) {
	array1->reserve(array1->size() + array->size());
	for (double v : *array) {
		array1->push_inc(LSNumber::get(v));
	}
	if (array->refs == 0) delete array;
	return array1;
}

template <typename T>
inline LSArray<T>* LSArray<T>::ls_push_all_flo(LSArray<T>* array1, LSArray<double>* array) {
	array1->reserve(array1->size() + array->size());
	array1->insert(array1->end(), array->begin(), array->end());
	if (array->refs == 0) delete array;
	return array1;
}

template <class T>
void shuffle_array(LSArray<T>* array, size_t permutations) {
	for (size_t i = 0; i < permutations; ++i) {
		size_t j = rand() % array->size();
		if (i == j) continue;
		T tmp = (*array)[i];
		(*array)[i] = (*array)[j];
		(*array)[j] = tmp;
	}
}

template <typename T>
LSArray<T>* LSArray<T>::ls_shuffle(LSArray<T>* array) {
	auto array1 = array->refs == 0 ? array : (LSArray<T>*) array->clone();
	shuffle_array(array1, array1->size());
	return array1;
}

template <typename T>
LSArray<T>* LSArray<T>::ls_random(LSArray<T>* array, int n) {
	n = std::max(0, std::min(n, (int) array->size()));
	auto result = array->refs == 0 ? array : (LSArray<T>*) array->clone();
	shuffle_array(result, n);
	for (auto i = result->begin() + n; i != result->end(); ++i) {
		ls::unref(*i);
	}
	result->erase(result->begin() + n, result->end());
	return result;
}

template <typename T>
LSArray<LSValue*>* LSArray<T>::to_any_array(LSArray<T>* array) {
	auto result = new LSArray<LSValue*>();
	result->reserve(array->size());
	for (const auto& e : *array) {
		result->emplace_back(ls::convert<LSValue*>(e));
	}
	return result;
}
template <typename T>
LSArray<double>* LSArray<T>::to_real_array(LSArray<T>* array) {
	auto result = new LSArray<double>();
	result->reserve(array->size());
	for (const auto& e : *array) {
		result->emplace_back(ls::convert<double>(e));
	}
	return result;
}
template <typename T>
LSArray<long>* LSArray<T>::to_long_array(LSArray<T>* array) {
	auto result = new LSArray<long>();
	result->reserve(array->size());
	for (const auto& e : *array) {
		result->emplace_back(ls::convert<long>(e));
	}
	return result;
}

template <typename T>
bool LSArray<T>::next_permutation(LSArray<T>* array) {
	return std::next_permutation(array->begin(), array->end());
}

template <typename T>
LSArray<T>* LSArray<T>::repeat(LSArray<T>* array, int n) {
	auto r = new LSArray<T>(array->size() * n);
	for (size_t i = 0; i < n; ++i) {
		for (auto& e : *array) {
			r->push_inc(e);
		}
	}
	LSValue::delete_temporary(array);
	return r;
}

template <class T>
bool LSArray<T>::std_in(LSArray<T>* array, const LSValue* const v) {
	return array->in(v);
}
template <class T>
bool LSArray<T>::std_in_i(LSArray<T>* array, const int v) {
	return array->in_i(v);
}
template <class T>
bool LSArray<T>::std_to_bool(LSArray<T>* array) {
	return array->to_bool();
}

template <typename T>
inline LSArray<T>* LSArray<T>::ls_reverse(LSArray<T>* array) {
	if (array->refs == 0) {
		for (size_t i = 0, j = array->size(); i < j; ++i, --j) {
			T tmp = (*array)[i];
			(*array)[i] = (*array)[j - 1];
			(*array)[j - 1] = tmp;
		}
		return array;
	} else {
		auto new_array = new LSArray<T>();
		new_array->reserve(array->size());
		for (auto it = array->rbegin(); it != array->rend(); it++) {
			new_array->push_clone(*it);
		}
		return new_array;
	}
}

template <class T>
template <class F>
LSArray<T>* LSArray<T>::ls_filter(LSArray<T>* array, F function) {
	if (refs == 0) {
		for (size_t i = 0; i < array->size(); ) {
			T v = (*array)[i];
			if (!ls::call<bool>(function, v)) {
				ls::unref(v);
				this->erase(array->begin() + i);
			} else {
				++i;
			}
		}
		return array;
	} else {
		auto new_array = new LSArray<T>();
		new_array->reserve(array->size());
		for (auto v : *array) {
			if (ls::call<bool>(function, v)) new_array->push_clone(v);
		}
		return new_array;
	}
}

template <class T>
template <class F, class R>
R LSArray<T>::ls_foldLeft(LSArray<T>* array, F function, R v0) {
	auto result = ls::move(v0);
	for (const auto& v : *array) {
		result = ls::call<R>(function, result, v);
	}
	if (array->refs == 0) delete array;
	return result;
}

template <class T>
template <class F, class R>
R LSArray<T>::ls_foldRight(LSArray<T>* array, F function, R v0) {
	auto result = ls::move(v0);
	for (auto it = array->rbegin(); it != array->rend(); it++) {
		result = ls::call<R>(function, *it, result);
	}
	if (array->refs == 0) delete array;
	return result;
}

template <>
inline LSArray<LSValue*>* LSArray<LSValue*>::ls_insert(LSArray<LSValue*>* array, LSValue* value, int pos) {
	if (pos >= (int) array->size()) {
		array->resize(pos, LSNull::get());
	}
	array->insert(array->begin() + pos, value->move_inc());
	return array;
}

template <class T>
LSArray<T>* LSArray<T>::ls_insert(LSArray<T>* array, T value, int pos) {
	if (pos >= (int) array->size()) {
		array->resize(pos, (T) 0);
	}
	array->insert(array->begin() + pos, value);
	return array;
}

template <class T>
template <class F, class R, class T2>
LSArray<R>* LSArray<T>::ls_map2(LSArray<T>* array1, LSArray<T2>* array, F function) {
	auto result = new LSArray<R>();
	result->reserve(array1->size());
	for (size_t i = 0; i < array1->size(); ++i) {
		T v1 = array1->operator [] (i);
		T2 v2 = array->operator [] (i);
		R res = ls::call<R>(function, v1, v2);
		result->push_move(res);
	}
	LSValue::delete_temporary(array1);
	LSValue::delete_temporary(array);
	return result;
}

template <>
inline int LSArray<LSValue*>::ls_search(LSArray<LSValue*>* array, LSValue* needle, int start) {
	for (size_t i = start; i < array->size(); i++) {
		if (*needle == *(*array)[i]) {
			if (array->refs == 0) delete array;
			LSValue::delete_temporary(needle);
			return i;
		}
	}
	if (array->refs == 0) delete array;
	LSValue::delete_temporary(needle);
	return -1;
}

template <class T>
int LSArray<T>::ls_search(LSArray<T>* array, T needle, int start) {
	for (size_t i = start; i < array->size(); i++) {
		if (needle == (*array)[i]) {
			if (array->refs == 0) delete array;
			return i;
		}
	}
	if (array->refs == 0) delete array;
	return -1;
}

template <>
inline LSString* LSArray<LSValue*>::ls_join(LSArray<LSValue*>* array) {
	if (array->empty()) {
		if (array->refs == 0) delete array;
		return new LSString();
	}
	auto it = array->begin();
	LSValue* result = new LSString();
	result = result->add(*it);
	for (++it; it != array->end(); ++it) {
		result = result->add(*it);
	}
	if (array->refs == 0) delete array;
	return (LSString*) result;
}

template <class T>
LSString* LSArray<T>::ls_join(LSArray<T>* array) {
	if (array->empty()) {
		if (array->refs == 0) delete array;
		return new LSString();
	}
	size_t i = 0;
	std::string result = LSNumber::print(array->operator[] (i));
	for (i++; i < array->size(); i++) {
		result = result + LSNumber::print(array->operator[] (i));
	}
	if (array->refs == 0) delete array;
	return new LSString(result);
}

template <>
inline LSString* LSArray<LSValue*>::ls_join_glue(LSArray<LSValue*>* array, LSString* glue) {
	if (array->empty()) {
		if (array->refs == 0) delete array;
		if (glue->refs == 0) delete glue;
		return new LSString();
	}
	glue->refs++; // because we will use it several times
	auto it = array->begin();
	LSValue* result = new LSString();
	result = result->add(*it);
	for (++it; it != array->end(); ++it) {
		result = result->add(glue);
		result = result->add(*it);
	}
	glue->refs--;
	if (array->refs == 0) delete array;
	if (glue->refs == 0) delete glue;
	return (LSString*) result;
}

template <class T>
LSString* LSArray<T>::ls_join_glue(LSArray<T>* array, LSString* glue) {
	if (array->empty()) {
		if (array->refs == 0) delete array;
		if (glue->refs == 0) delete glue;
		return new LSString();
	}
	size_t i = 0;
	std::string result = LSNumber::print(array->operator[] (i));
	for (i++; i < array->size(); i++) {
		result = result + *glue + LSNumber::print(array->operator[] (i));
	}
	if (array->refs == 0) delete array;
	if (glue->refs == 0) delete glue;
	return new LSString(result);
}

template <>
inline LSArray<LSValue*>* LSArray<LSValue*>::ls_fill(LSArray<LSValue*>* array, LSValue* element, int size) {
	for (auto v : *array) {
		LSValue::delete_ref(v);
	}
	array->clear();
	array->reserve(size);
	for (int i = 0; i < size; i++) {
		array->push_move(element);
	}
	LSValue::delete_temporary(element); // only useful if size = 0
	return array;
}

template <typename T>
inline LSArray<T>* LSArray<T>::ls_fill(LSArray<T>* array, T element, int size) {
	array->clear();
	array->resize(size, element);
	return array;
}

template <>
inline LSValue* LSArray<LSValue*>::ls_max(LSArray<LSValue*>* array) {
	if (array->empty()) {
		LSValue::delete_temporary(array);
		throw vm::ExceptionObj(vm::Exception::ARRAY_OUT_OF_BOUNDS);
	}
	LSValue* max = (*array)[0];
	for (size_t i = 1; i < array->size(); ++i) {
		if (*(*array)[i] > *max) {
			max = (*array)[i];
		}
	}
	if (array->refs == 0) {
		max = max->clone();
		LSValue::delete_temporary(array);
	}
	return max;
}
template <class T>
T LSArray<T>::ls_max(LSArray<T>* array) {
	if (array->empty()) {
		LSValue::delete_temporary(array);
		throw vm::ExceptionObj(vm::Exception::ARRAY_OUT_OF_BOUNDS);
	}
	T max = (*array)[0];
	for (size_t i = 1; i < array->size(); ++i) {
		if ((*array)[i] > max) {
			max = (*array)[i];
		}
	}
	if (array->refs == 0) delete array;
	return max;
}

template <>
inline LSValue* LSArray<LSValue*>::ls_min(LSArray<LSValue*>* array) {
	if (array->empty()) {
		LSValue::delete_temporary(array);
		throw vm::ExceptionObj(vm::Exception::ARRAY_OUT_OF_BOUNDS);
	}
	LSValue* min = (*array)[0];
	for (size_t i = 1; i < array->size(); ++i) {
		if (*(*array)[i] < *min) {
			min = (*array)[i];
		}
	}
	if (array->refs == 0) {
		min = min->clone();
		LSValue::delete_temporary(array);
	}
	return min;
}
template <class T>
T LSArray<T>::ls_min(LSArray<T>* array) {
	if (array->empty()) {
		if (array->refs == 0) delete array;
		throw vm::ExceptionObj(vm::Exception::ARRAY_OUT_OF_BOUNDS);
	}
	T min = (*array)[0];
	for (size_t i = 1; i < array->size(); ++i) {
		if ((*array)[i] < min) {
			min = (*array)[i];
		}
	}
	if (array->refs == 0) delete array;
	return min;
}

template <class T1, class T2>
std::pair<T1, T2> ls_mismatch(T1 first1, T1 last1, T2 first2) {
	while (first1 != last1 && ls::equals(*first1, *first2)) {
		++first1, ++first2;
	}
	return std::make_pair(first1, first2);
}

template <class T1, class T2>
int ls_count(T1 first, T1 last, const T2& value) {
	int ret = 0;
	for (; first != last; ++first) {
		if (ls::equals(*first, value)) ret++;
	}
	return ret;
}

/*
 * Based on http://en.cppreference.com/w/cpp/algorithm/is_permutation implementation
 */
template <class T1, class T2>
bool is_permutation_base(LSArray<T1>* a1, LSArray<T2>* a2) {
	auto first = a1->begin();
	auto last = a1->end();
	auto d_first = a2->begin();
	std::tie(first, d_first) = ls_mismatch(first, last, d_first);
	if (first != last) {
		auto d_last = d_first;
		std::advance(d_last, std::distance(first, last));
		for (auto i = first; i != last; ++i) {
			if (i != std::find(first, i, *i)) continue;
			auto m = ls_count(d_first, d_last, *i);
			if (m == 0 || ls_count(i, last, *i) != m) {
				return false;
			}
		}
	}
	return true;
}

template <class T>
template <class T2>
bool LSArray<T>::is_permutation(LSArray<T>* array, LSArray<T2>* other) {
	bool result = array->size() == other->size() and is_permutation_base(array, other);
	LSValue::delete_temporary(array);
	LSValue::delete_temporary(other);
	return result;
}

/*
 * LSValue methods
 */
template <class T>
bool LSArray<T>::to_bool() const {
	return this->size() > 0;
}

template <class T>
bool LSArray<T>::ls_not() const {
	auto r = this->size() == 0;
	LSValue::delete_temporary(this);
	return r;
}

template <class T>
int LSArray<T>::abso() const {
	return this->size();
}

template <class T>
LSValue* LSArray<T>::ls_tilde() {
	auto array = new LSArray<T>();
	array->reserve(this->size());
	if (refs == 0) {
		for (auto i = this->rbegin(); i != this->rend(); ++i) {
			array->push_back(*i);
		}
		this->clear();
		delete this;
	} else {
		for (auto i = this->rbegin(); i != this->rend(); ++i) {
			array->push_clone(*i);
		}
	}
	return array;
}

template <class T>
template <class T2>
LSValue* LSArray<T>::add_set(LSSet<T2>* set) {
	this->reserve(this->size() + set->size());
	if (set->refs == 0) {
		for (auto v : *set) {
			this->push_back(ls::oneref(ls::convert<T>(v)));
		}
		set->clear();
		delete set;
	} else {
		for (auto v : *set) {
			this->push_clone(ls::convert<T>(v));
		}
	}
	return this;
}

template <>
inline LSValue* LSArray<LSValue*>::add(LSValue* v) {
	if (v->type == ARRAY) {
		if (auto array = dynamic_cast<LSArray<LSValue*>*>(v)) {
			if (refs == 0) {
				return ls_push_all_ptr(this, array);
			}
			return ls_push_all_ptr((LSArray<LSValue*>*) this->clone(), array);
		}
		if (auto array = dynamic_cast<LSArray<int>*>(v)) {
			if (refs == 0) {
				return ls_push_all_int(this, array);
			}
			return ls_push_all_int(((LSArray<LSValue*>*) this->clone()), array);
		}
		if (auto array = dynamic_cast<LSArray<double>*>(v)) {
			if (refs == 0) {
				return ls_push_all_flo(this, array);
			}
			return ls_push_all_flo(((LSArray<LSValue*>*) this->clone()), array);
		}
	}
	if (refs == 0) {
		this->push_move(v);
		return this;
	}
	auto r = (LSArray<LSValue*>*) this->clone();
	r->push_move(v);
	return r;
}

template <>
inline LSValue* LSArray<double>::add(LSValue* v) {
	if (auto array = dynamic_cast<LSArray<LSValue*>*>(v)) {
		auto ret = new LSArray<LSValue*>();
		ret->reserve(this->size() + array->size());
		for (auto v : *this) {
			ret->push_inc(LSNumber::get(v));
		}
		if (array->refs == 0) {
			for (auto v : *array) {
				ret->push_back(v);
			}
			array->clear();
			delete array;
		} else {
			for (auto v : *array) {
				ret->push_clone(v);
			}
		}
		if (refs == 0) delete this;
		return ret;
	} else if (auto number = dynamic_cast<LSNumber*>(v)) {
		if (refs == 0) {
			this->push_back(number->value);
			if (number->refs == 0) delete v;
			return this;
		}
		auto r = (LSArray<double>*) this->clone();
		r->push_back(number->value);
		if (number->refs == 0) delete number;
		return r;
	} else {
		auto r = new LSArray<LSValue*>();
		r->reserve(this->size() + 1);
		for (auto number : *this) {
			r->push_inc(LSNumber::get(number));
		}
		r->push_move(v);
		if (refs == 0) delete this;
		return r;
	}
}

template <>
inline LSValue* LSArray<int>::add(LSValue* v) {
	if (v->type == ARRAY) {
		if (auto array = dynamic_cast<LSArray<LSValue*>*>(v)) {
			auto ret = new LSArray<LSValue*>();
			ret->reserve(this->size() + array->size());
			for (auto v : *this) {
				ret->push_inc(LSNumber::get(v));
			}
			if (array->refs == 0) {
				for (auto v : *array) {
					ret->push_back(v);
				}
				array->clear();
				delete array;
			} else {
				for (auto v : *array) {
					ret->push_clone(v);
				}
			}
			if (refs == 0) delete this;
			return ret;
		}
		if (auto array = dynamic_cast<LSArray<int>*>(v)) {
			if (refs == 0) {
				return ls_push_all_int(this, array);
			}
			return ls_push_all_int((LSArray<int>*) this->clone(), array);
		}
		if (auto array = dynamic_cast<LSArray<double>*>(v)) {
			auto ret = new LSArray<double>();
			ret->reserve(this->size() + array->size());
			ret->insert(ret->end(), this->begin(), this->end());
			ret->insert(ret->end(), array->begin(), array->end());
			if (refs == 0) delete this;
			if (array->refs == 0) delete array;
			return ret;
		}
	}
	if (auto number = dynamic_cast<LSNumber*>(v)) {
		if (number->value == (int) number->value) {
			if (refs == 0) {
				this->push_back(number->value);
				if (number->refs == 0) delete number;
				return this;
			}
			auto r = (LSArray<int>*) this->clone();
			r->push_back(number->value);
			if (number->refs == 0) delete number;
			return r;
		}
		auto ret = new LSArray<double>();
		ret->insert(ret->end(), this->begin(), this->end());
		ret->push_back(number->value);
		if (refs == 0) delete this;
		if (number->refs == 0) delete number;
		return ret;
	} else {
		auto r = new LSArray<LSValue*>();
		r->reserve(this->size() + 1);
		for (auto v : *this) {
			r->push_inc(LSNumber::get(v));
		}
		r->push_move(v);
		if (refs == 0) delete this;
		return r;
	}
}


template <>
inline LSValue* LSArray<long>::add(LSValue* v) {
	if (v->type == ARRAY) {
		if (auto array = dynamic_cast<LSArray<LSValue*>*>(v)) {
			auto ret = new LSArray<LSValue*>();
			ret->reserve(this->size() + array->size());
			for (auto v : *this) {
				ret->push_inc(LSNumber::get(v));
			}
			if (array->refs == 0) {
				for (auto v : *array) {
					ret->push_back(v);
				}
				array->clear();
				delete array;
			} else {
				for (auto v : *array) {
					ret->push_clone(v);
				}
			}
			if (refs == 0) delete this;
			return ret;
		}
		if (auto array = dynamic_cast<LSArray<int>*>(v)) {
			if (refs == 0) {
				return ls_push_all_int(this, array);
			}
			return LSArray<int>::ls_push_all_int((LSArray<int>*) this->clone(), array);
		}
		if (auto array = dynamic_cast<LSArray<double>*>(v)) {
			auto ret = new LSArray<double>();
			ret->reserve(this->size() + array->size());
			ret->insert(ret->end(), this->begin(), this->end());
			ret->insert(ret->end(), array->begin(), array->end());
			if (refs == 0) delete this;
			if (array->refs == 0) delete array;
			return ret;
		}
	}
	if (auto number = dynamic_cast<LSNumber*>(v)) {
		if (number->value == (int) number->value) {
			if (refs == 0) {
				this->push_back(number->value);
				if (number->refs == 0) delete number;
				return this;
			}
			auto r = (LSArray<int>*) this->clone();
			r->push_back(number->value);
			if (number->refs == 0) delete number;
			return r;
		}
		auto ret = new LSArray<double>();
		ret->insert(ret->end(), this->begin(), this->end());
		ret->push_back(number->value);
		if (refs == 0) delete this;
		if (number->refs == 0) delete number;
		return ret;
	} else {
		auto r = new LSArray<LSValue*>();
		r->reserve(this->size() + 1);
		for (auto v : *this) {
			r->push_inc(LSNumber::get(v));
		}
		r->push_move(v);
		if (refs == 0) delete this;
		return r;
	}
}

template <>
inline LSValue* LSArray<char>::add(LSValue* v) {
	if (v->type == ARRAY) {
		if (auto array = dynamic_cast<LSArray<LSValue*>*>(v)) {
			auto ret = new LSArray<LSValue*>();
			ret->reserve(this->size() + array->size());
			for (auto v : *this) {
				ret->push_inc(LSNumber::get(v));
			}
			if (array->refs == 0) {
				for (auto v : *array) {
					ret->push_back(v);
				}
				array->clear();
				delete array;
			} else {
				for (auto v : *array) {
					ret->push_clone(v);
				}
			}
			if (refs == 0) delete this;
			return ret;
		}
		if (auto array = dynamic_cast<LSArray<int>*>(v)) {
			if (refs == 0) {
				return ls_push_all_int(this, array);
			}
			return LSArray<int>::ls_push_all_int((LSArray<int>*) this->clone(), array);
		}
		if (auto array = dynamic_cast<LSArray<double>*>(v)) {
			auto ret = new LSArray<double>();
			ret->reserve(this->size() + array->size());
			ret->insert(ret->end(), this->begin(), this->end());
			ret->insert(ret->end(), array->begin(), array->end());
			if (refs == 0) delete this;
			if (array->refs == 0) delete array;
			return ret;
		}
	}
	if (auto number = dynamic_cast<LSNumber*>(v)) {
		if (number->value == (int) number->value) {
			if (refs == 0) {
				this->push_back(number->value);
				if (number->refs == 0) delete number;
				return this;
			}
			auto r = (LSArray<int>*) this->clone();
			r->push_back(number->value);
			if (number->refs == 0) delete number;
			return r;
		}
		auto ret = new LSArray<double>();
		ret->insert(ret->end(), this->begin(), this->end());
		ret->push_back(number->value);
		if (refs == 0) delete this;
		if (number->refs == 0) delete number;
		return ret;
	} else {
		auto r = new LSArray<LSValue*>();
		r->reserve(this->size() + 1);
		for (auto v : *this) {
			r->push_inc(LSNumber::get(v));
		}
		r->push_move(v);
		if (refs == 0) delete this;
		return r;
	}
}

template <>
inline LSValue* LSArray<LSValue*>::add_eq(LSValue* v) {
	if (v->type == ARRAY) {
		if (auto array = dynamic_cast<LSArray<LSValue*>*>(v)) {
			return ls_push_all_ptr(this, array);
		}
		if (auto array = dynamic_cast<LSArray<int>*>(v)) {
			return ls_push_all_int(this, array);
		}
		if (auto array = dynamic_cast<LSArray<double>*>(v)) {
			return ls_push_all_flo(this, array);
		}
	}
	if (v->type == SET) {
		if (auto set = dynamic_cast<LSSet<LSValue*>*>(v)) {
			return add_set(set);
		}
		if (auto set = dynamic_cast<LSSet<int>*>(v)) {
			return add_set(set);
		}
		if (auto set = dynamic_cast<LSSet<double>*>(v)) {
			return add_set(set);
		}
	}
	push_move(v);
	return this;
}

template <>
inline LSValue* LSArray<double>::add_eq(LSValue* v) {
	if (v->type == ARRAY) {
		if (auto array = dynamic_cast<LSArray<double>*>(v)) {
			return ls_push_all_flo(this, array);
		}
		if (auto array = dynamic_cast<LSArray<int>*>(v)) {
			return ls_push_all_int(this, array);
		}
	}
	if (auto set = dynamic_cast<LSSet<double>*>(v)) {
		return add_set(set);
	}
	if (auto number = dynamic_cast<LSNumber*>(v)) {
		this->push_back(number->value);
		LSValue::delete_temporary(number);
		return this;
	}
	auto set = dynamic_cast<LSSet<int>*>(v);
	return add_set(set);
}

template <>
inline LSValue* LSArray<int>::add_eq(LSValue* v) {
	if (auto number = dynamic_cast<LSNumber*>(v)) {
		this->push_back(number->value);
		LSValue::delete_temporary(number);
		return this;
	}
	if (auto array = dynamic_cast<LSArray<int>*>(v)) {
		this->reserve(this->size() + array->size());
		this->insert(this->end(), array->begin(), array->end());
		LSValue::delete_temporary(v);
		return this;
	}
	auto set = dynamic_cast<LSSet<int>*>(v);
	return add_set(set);
}


template <>
inline LSValue* LSArray<long>::add_eq(LSValue* v) {
	if (auto number = dynamic_cast<LSNumber*>(v)) {
		this->push_back(number->value);
		LSValue::delete_temporary(number);
		return this;
	}
	if (auto array = dynamic_cast<LSArray<int>*>(v)) {
		this->reserve(this->size() + array->size());
		this->insert(this->end(), array->begin(), array->end());
		LSValue::delete_temporary(v);
		return this;
	}
	auto set = dynamic_cast<LSSet<int>*>(v);
	return add_set(set);
}

template <>
inline LSValue* LSArray<char>::add_eq(LSValue* v) {
	if (auto number = dynamic_cast<LSNumber*>(v)) {
		this->push_back(number->value);
		LSValue::delete_temporary(number);
		return this;
	}
	if (auto array = dynamic_cast<LSArray<char>*>(v)) {
		this->reserve(this->size() + array->size());
		this->insert(this->end(), array->begin(), array->end());
		LSValue::delete_temporary(v);
		return this;
	}
	// auto set = dynamic_cast<LSSet<int>*>(v);
	// return add_set(set);
	return this;
}

template <class T>
LSValue* LSArray<T>::add_eq_double(double) {
	// TODO
	return this;
}

template <>
inline LSValue* LSArray<double>::add_eq_double(double v) {
	push_move(v);
	return this;
}

template <class T>
LSValue* LSArray<T>::add_eq_int(int) {
	// TODO
	return this;
}

template <>
inline LSValue* LSArray<int>::add_eq_int(int v) {
	push_move(v);
	return this;
}

template <class T, class T2>
bool array_equals(const LSArray<T>* self, const LSArray<T2>* array) {
	if (self->size() != array->size()) {
		return false;
	}
	auto j = array->begin();
	for (auto i = self->begin(); i != self->end(); i++, j++) {
		if (!ls::equals(*j, *i))
			return false;
	}
	return true;
}

template <class T>
bool LSArray<T>::eq(const LSValue* v) const {
	if (v->type == ARRAY) {
		if (auto array = dynamic_cast<const LSArray<LSValue*>*>(v)) {
			return array_equals(this, array);
		}
		if (auto array = dynamic_cast<const LSArray<int>*>(v)) {
			return array_equals(this, array);
		}
		if (auto array = dynamic_cast<const LSArray<double>*>(v)) {
			return array_equals(this, array);
		}
	}
	return false;
}

template <class T, class T2>
bool array_lt(const LSArray<T>* self, const LSArray<T2>* array) {
	auto i = self->begin();
	auto j = array->begin();
	while (i != self->end()) {
		if (j == array->end())
			return false;
		if ((*i)->type > LSValue::NUMBER)
			return false;
		if ((*i)->type < LSValue::NUMBER)
			return true;
		if (((LSNumber*) *i)->value < *j)
			return true;
		if (*j < ((LSNumber*) *i)->value)
			return false;
		++i; ++j;
	}
	return j != array->end();
}

template <typename T>
inline bool LSArray<T>::lt(const LSValue* v) const {
	if (v->type == ARRAY) {
		if (auto array = dynamic_cast<const LSArray<LSValue*>*>(v)) {
			auto i = this->begin();
			auto j = array->begin();
			while (i != this->end()) {
				if (j == array->end())
					return false;
				if ((*j)->type < LSValue::NUMBER)
					return false;
				if ((*j)->type > LSValue::NUMBER)
					return true;
				if (*i < ((LSNumber*) *j)->value)
					return true;
				if (((LSNumber*) *j)->value < *i)
					return false;
				++i; ++j;
			}
			return j != array->end();
		}
		if (auto array = dynamic_cast<const LSArray<int>*>(v)) {
			return std::lexicographical_compare(this->begin(), this->end(), array->begin(), array->end());
		}
		if (auto array = dynamic_cast<const LSArray<double>*>(v)) {
			return std::lexicographical_compare(this->begin(), this->end(), array->begin(), array->end());
		}
	}
	return LSValue::lt(v);
}

template <>
inline bool LSArray<LSValue*>::lt(const LSValue* v) const {
	if (v->type == ARRAY) {
		if (auto array = dynamic_cast<const LSArray<LSValue*>*>(v)) {
			return std::lexicographical_compare(begin(), end(), array->begin(), array->end(), [](const LSValue* a, const LSValue* b) -> bool {
				return *a < *b;
			});
		}
		if (auto array = dynamic_cast<const LSArray<int>*>(v)) {
			return array_lt(this, array);
		}
		if (auto array = dynamic_cast<const LSArray<double>*>(v)) {
			return array_lt(this, array);
		}
	}
	return LSValue::lt(v);
}

template <typename T>
inline bool LSArray<T>::in(const LSValue* const value) const {
	if (value->type != LSValue::NUMBER) {
		LSValue::delete_temporary(value);
		LSValue::delete_temporary(this);
		return false;
	}
	T v = static_cast<const LSNumber*>(value)->value;
	for (auto i = this->begin(); i != this->end(); i++) {
		if (*i == v) {
			LSValue::delete_temporary(value);
			LSValue::delete_temporary(this);
			return true;
		}
	}
	LSValue::delete_temporary(value);
	LSValue::delete_temporary(this);
	return false;
}

template <>
inline bool LSArray<LSValue*>::in(const LSValue* const value) const {
	for (auto i = this->begin(); i != this->end(); i++) {
		if (**i == *value) {
			LSValue::delete_temporary(this);
			LSValue::delete_temporary(value);
			return true;
		}
	}
	LSValue::delete_temporary(this);
	LSValue::delete_temporary(value);
	return false;
}

template <typename T>
bool LSArray<T>::in_i(const int v) const {
	for (auto i = this->begin(); i != this->end(); i++) {
		if (ls::equals(*i, v)) {
			LSValue::delete_temporary(this);
			return true;
		}
	}
	LSValue::delete_temporary(this);
	return false;
}

template <typename T>
inline LSValue* LSArray<T>::range(int start, int end) const {
	auto range = new LSArray<T>();
	size_t start_i = std::max<size_t>(0, start);
	size_t end_i = std::min<size_t>(this->size() - 1, end);
	for (size_t i = start_i; i <= end_i; ++i) {
		range->push_clone(this->operator [] (i));
	}
	return range;
}

template <class T>
LSValue* LSArray<T>::at(const LSValue* key) const {
	int index = 0;
	if (key->type == NUMBER) {
		auto n = static_cast<const LSNumber*>(key);
		index = (int) n->value;
	} else if (key->type == BOOLEAN) {
		auto b = static_cast<const LSBoolean*>(key);
		index = (int) b->value;
	} else {
		LSValue::delete_temporary(key);
		throw vm::ExceptionObj(vm::Exception::ARRAY_KEY_IS_NOT_NUMBER);
	}
	LSValue* res = nullptr;
	bool ex = false;
	try {
		res = ls::convert<LSValue*>(ls::clone(((std::vector<T>*) this)->at(index)));
	} catch (std::exception const & e) {
		LSValue::delete_temporary(key);
		ex = true;
	}
	if (ex) throw vm::ExceptionObj(vm::Exception::ARRAY_OUT_OF_BOUNDS);
	return res;
}

template <class T>
int LSArray<T>::at_i_i(const int) const {
	assert(false); // Not compatible
}

template <>
inline int LSArray<int>::at_i_i(const int key) const {
	try {
		return ((std::vector<int>*) this)->at(key);
	} catch (std::exception const & e) {
		throw vm::ExceptionObj(vm::Exception::ARRAY_OUT_OF_BOUNDS);
	}
}

template <class T>
LSValue** LSArray<T>::atL(const LSValue* key) {
	if (key->type != NUMBER) {
		throw vm::ExceptionObj(vm::Exception::ARRAY_KEY_IS_NOT_NUMBER);
	}
	auto n = static_cast<const LSNumber*>(key);
	int i = (int) n->value;
	LSValue::delete_temporary(key);
	if (i < 0 || (size_t) i >= this->size()) {
		throw vm::ExceptionObj(vm::Exception::ARRAY_OUT_OF_BOUNDS);
	}
	return (LSValue**) &((std::vector<T>*) this)->operator [](i);
}

template <class T>
LSValue* LSArray<T>::clone() const {
	return new LSArray<T>(*this);
}

template <typename T>
std::ostream& LSArray<T>::dump(std::ostream& os, int) const {
	os << "[";
	for (auto i = this->begin(); i != this->end(); i++) {
		if (i != this->begin()) os << ", ";
		os << (*i);
	}
	os << "]";
	return os;
}

template <>
inline std::ostream& LSArray<char>::dump(std::ostream& os, int level) const {
	os << "[";
	for (auto i = this->begin(); i != this->end(); i++) {
		if (i != this->begin()) os << ", ";
		os << std::boolalpha << (bool) (*i);
	}
	os << "]";
	return os;
}

template <>
inline std::ostream& LSArray<LSValue*>::dump(std::ostream& os, int level) const {
	os << "[";
	if (level > 0) {
		for (auto i = this->begin(); i != this->end(); i++) {
			if (i != this->begin()) os << ", ";
			(*i)->dump(os, level - 1);
		}
	} else {
		os << " ... ";
	}
	os << "]";
	return os;
}

template <typename T>
std::string LSArray<T>::json() const {
	std::string res = "[";
	bool last_valid = true;
	for (auto i = this->begin(); i != this->end(); i++) {
		if (i != this->begin() and last_valid)
			res += ", ";
		auto j = ls::to_json(*i);
		if (j.size())
			res += j;
		last_valid = j.size() > 0;
	}
	return res + "]";
}

template <class T>
LSValue* LSArray<T>::getClass(VM* vm) const {
	return vm->env.array_class.get();
}

} // end of namespace ls

#endif
