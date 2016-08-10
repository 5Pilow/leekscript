#ifndef LS_ARRAY_TCC
#define LS_ARRAY_TCC

#include "LSNull.hpp"
#include "LSNumber.hpp"
#include "LSBoolean.hpp"
#include "LSFunction.hpp"

#include <algorithm>

namespace ls {

template <class T>
LSValue* LSArray<T>::array_class(new LSClass("Array"));

template <>
inline void LSArray<LSValue*>::push_clone(LSValue* value) {
	if (value->native) {
		this->push_back(value); // no clone if native value
	} else {
		LSValue* val = value->clone();
		val->refs++;
		this->push_back(val);
	}
}
template <>
inline void LSArray<int>::push_clone(int value) {
	this->push_back(value);
}

template <>
inline void LSArray<double>::push_clone(double value) {
	this->push_back(value);
}

template <>
inline void LSArray<LSValue*>::push_move(LSValue* value) {
	if (value->native) {
		this->push_back(value); // no clone if native value
	} else {
		LSValue* val = value->move();
		val->refs++;
		this->push_back(val);
	}
}
template <>
inline void LSArray<int>::push_move(int value) {
	this->push_back(value);
}

template <>
inline void LSArray<double>::push_move(double value) {
	this->push_back(value);
}

template <>
inline void LSArray<LSValue*>::push_no_clone(LSValue* value) {
	this->push_back(value);
	value->refs++;
}
template <>
inline void LSArray<int>::push_no_clone(int value) {
	this->push_back(value);
}
template <>
inline void LSArray<double>::push_no_clone(double value) {
	this->push_back(value);
}

template <class T>
LSArray<T>::LSArray() {}

template <class T>
LSArray<T>::LSArray(std::initializer_list<T> values_list) {
	for (auto i : values_list) {
		this->push_back(i);
	}
}

template <class T>
LSArray<T>::LSArray(const std::vector<T>& vec) {
	for (auto i : vec) {
		this->push_back(i);
	}
}

template <class T>
LSArray<T>::LSArray(Json& json) {
	for (Json::iterator it = json.begin(); it != json.end(); ++it) {
		push_clone((T) LSValue::parse(it.value()));
	}
}

template <>
inline LSArray<int>::LSArray(Json& json) {
	for (Json::iterator it = json.begin(); it != json.end(); ++it) {
		push_clone(((LSNumber*) LSValue::parse(it.value()))->value);
	}
}

template <>
inline LSArray<LSValue*>::~LSArray() {
//	std::cout << "~LSArray<T>" << std::endl;
	for (auto v : *this) {
//		std::cout << "delete ";
//		v->print(std::cout);
//		std::cout << " " << v->refs << std::endl;
		LSValue::delete_val(v);
	}
}
template <>
inline LSArray<int>::~LSArray() {
//	std::cout << "~LSArray<int>" << std::endl;
}
template <>
inline LSArray<double>::~LSArray() {
//	std::cout << "~LSArray<double>" << std::endl;
}

template <class T>
void LSArray<T>::clear() {
	// TODO
	((std::vector<T>*) this)->clear();
}

template <class T>
T LSArray<T>::remove(const int index) {
	T previous = this->operator [] (index);
	this->erase(this->begin() + index);
	return previous;
}

template <class T>
T LSArray<T>::remove_key(LSValue*) {
	// TODO : move all indices after index to the left ?
	// or transform the array into a associative one
	return LSNull::get();
}

template <class T>
T LSArray<T>::remove_element(T) {
	// TODO : move all indices after index to the left ?
	// or transform the array into a associative one
	return LSNull::get();
}

template <>
inline int LSArray<int>::remove_element(int element) {
	for (auto it = this->begin(); it != this->end(); ++it) {
		if (*it == element) {
			this->erase(it);
			return element;
		}
	}
	return 0;
}

template <class T>
inline size_t LSArray<T>::size() const {
	return ((std::vector<T>*) this)->size();
}

template <class T>
T LSArray<T>::sum() const {
	if (size() == 0) return (T) LSNumber::get(0);
	LSValue* sum = this->operator [] (0)->clone();
	for (unsigned i = 1; i < this->size(); ++i) {
		LSValue* new_sum = (*this)[i]->operator + (sum);
		LSValue::delete_val(sum);
		sum = new_sum;
	}
	return (T) sum;
}

template <>
inline int LSArray<int>::sum() const {
	int sum = 0;
	for (auto v : *this) {
		sum += v;
	}
	return sum;
}

template <>
inline double LSArray<double>::sum() const {
	double sum = 0;
	for (auto v : *this) {
		sum += v;
	}
	return sum;
}

template <class T>
double LSArray<T>::average() const {
	return 0; // No average for a no integer array
}

template <>
inline double LSArray<double>::average() const {
	if (size() == 0) return 0;
	return this->sum() / size();
}

template <>
inline double LSArray<int>::average() const {
	if (size() == 0) return 0;
	return (double) this->sum() / size();
}

template <class T>
T LSArray<T>::first() const {
	if (size() == 0) return T();
	return this->operator [] (0);
}

template <class T>
T LSArray<T>::last() const {
	if (size() == 0) return T();
	return this->back();
}

template <class T>
LSValue* LSArray<T>::pop() {
	LSValue* last = LSNull::get();
	if (this->size() > 0) {
		last = this->back();
		this->pop_back();
	}
	return last;
}

template <>
inline LSValue* LSArray<int>::pop() {
	if (this->size() > 0) {
		int last = this->back();
		this->pop_back();
		return LSNumber::get(last);
	}
	return LSNull::get();
}

template <>
inline LSArray<LSValue*>* LSArray<LSValue*>::map(const void* function) const {

	LSArray<LSValue*>* new_array = new LSArray<LSValue*>();
	new_array->reserve(this->size());
	auto fun = (LSValue* (*)(void*)) function;
	for (auto v : *this) {
		LSValue* c = v->clone();
		new_array->push_move(fun(c));
	}
	return new_array;
}

template <>
inline LSArray<LSValue*>* LSArray<int>::map(const void* function) const {

	LSArray<LSValue*>* new_array = new LSArray<LSValue*>();
	new_array->reserve(this->size());
	auto fun = (LSValue* (*)(int)) function;
	for (auto v : *this) {
		new_array->push_move(fun(v));
	}
	return new_array;
}
template <>
inline LSArray<LSValue*>* LSArray<double>::map(const void* function) const {
	LSArray<LSValue*>* new_array = new LSArray<LSValue*>();
	new_array->reserve(this->size());
	auto fun = (LSValue* (*)(double)) function;
	for (auto v : *this) {
		new_array->push_move(fun(v));
	}
	return new_array;
}

template <>
inline LSArray<double>* LSArray<int>::map_real(const void* function) const {
	LSArray<double>* new_array = new LSArray<double>();
	new_array->reserve(this->size());
	auto fun = (double (*)(int)) function;
	for (auto v : *this) {
		new_array->push_back(fun(v));
	}
	return new_array;
}
template <>
inline LSArray<double>* LSArray<double>::map_real(const void* function) const {
	LSArray<double>* new_array = new LSArray<double>();
	new_array->reserve(this->size());
	auto fun = (double (*)(double)) function;
	for (auto v : *this) {
		new_array->push_no_clone(fun(v));
	}
	return new_array;
}

template <>
inline LSArray<int>* LSArray<int>::map_int(const void* function) const {
	LSArray<int>* new_array = new LSArray<int>();
	new_array->reserve(this->size());
	auto fun = (int (*)(int)) function;
	for (auto v : *this) {
		new_array->push_back(fun(v));
	}
	return new_array;
}
template <>
inline LSArray<int>* LSArray<double>::map_int(const void* function) const {
	LSArray<int>* new_array = new LSArray<int>();
	new_array->reserve(this->size());
	auto fun = (int (*)(double)) function;
	for (auto v : *this) {
		new_array->push_no_clone(fun(v));
	}
	return new_array;
}



template <typename T>
inline LSArray<LSValue*>* LSArray<T>::chunk(int size) const {
	if (size <= 0) size = 1;

	LSArray<LSValue*>* new_array = new LSArray<LSValue*>();

	new_array->reserve(this->size() / size + 1);

	size_t i = 0;
	while (i < this->size()) {
		LSArray<T>* sub_array = new LSArray<T>();
		sub_array->reserve(size);

		size_t j = std::min(i + size, this->size());
		for (; i < j; ++i) {
			sub_array->push_clone((*this)[i]);
		}

		new_array->push_no_clone(sub_array);
	}
	return new_array;
}

template <>
inline LSArray<LSValue*>* LSArray<LSValue*>::unique() {
	this->refs++;
	if (this->empty()) return this;

	auto it = this->begin();
	auto next = it;

	while (true) {
		++next;
		while (next != this->end() && (*next)->operator == (*it)) {
			LSValue::delete_val(*next++);
		}
		++it;
		if (next == this->end()) {
			break;
		}
		*it = *next;
	}
	this->resize(std::distance(this->begin(), it));
	return this;
}

template <>
inline LSArray<int>* LSArray<int>::unique() {
	auto it = std::unique(this->begin(), this->end());
	this->resize(std::distance(this->begin(), it));
	this->refs++;
	return this;
}

template <>
inline LSArray<double>* LSArray<double>::unique() {
	auto it = std::unique(this->begin(), this->end());
	this->resize(std::distance(this->begin(), it));
	this->refs++;
	return this;
}

template <>
inline LSArray<LSValue*>* LSArray<LSValue*>::sort() {
	std::sort(this->begin(), this->end(), [](LSValue* a, LSValue* b) -> bool {
		return b->operator < (a);
	});
	this->refs++;
	return this;
}

template <>
inline LSArray<int>* LSArray<int>::sort() {
	std::sort(this->begin(), this->end());
	this->refs++;
	return this;
}

template <>
inline LSArray<double>* LSArray<double>::sort() {
	std::sort(this->begin(), this->end());
	this->refs++;
	return this;
}

template <class T>
void LSArray<T>::iter(const LSFunction* function) const {

	auto fun = (void* (*)(void*)) function->function;
	for (auto v : *this) {
		fun((void*) v);
	}
}

template <class T>
int LSArray<T>::contains(const LSValue* val) const {
	for (auto v : *this) {
		if (v->operator == (val)) {
			return true;
		}
	}
	return false;
}

template <>
inline int LSArray<int>::contains(const LSValue* val) const {
	if (const LSNumber* n = dynamic_cast<const LSNumber*>(val)) {
		for (auto v : *this) {
			if (v == n->value) return true;
		}
	}
	return false;
}

template <>
inline int LSArray<int>::contains_int(int val) const {
	for (auto v : *this) {
		if (v == val) return true;
	}
	return false;
}

template <class T>
LSArray<T>* LSArray<T>::push(const T val) {
	push_clone(val);
	if (this->refs == 0) refs = 1;
	return this;
}

template <class T>
LSArray<T>* LSArray<T>::push_all(const LSArray<LSValue*>* array) {
	for (auto v : *array) {
		push_clone((T) v);
	}
	if (this->refs == 0) refs = 1;
	return this;
}

template <>
inline const LSArray<int>* LSArray<int>::push_all_int(const LSArray<int>* array) {
	for (auto v : *array) {
		push_clone(v);
	}
	if (this->refs == 0) refs = 1;
	return this;
}

template <class T>
LSArray<T>* LSArray<T>::reverse() const {
	LSArray<T>* new_array = new LSArray<T>();
	new_array->reserve(this->size());
	for (auto it = this->rbegin(); it != this->rend(); it++) {
		new_array->push_clone(*it);
	}
	return new_array;
}

template <>
inline LSArray<int>* LSArray<int>::reverse() const {
	LSArray<int>* new_array = new LSArray<int>();
	new_array->reserve(this->size());
	for (auto it = this->rbegin(); it != this->rend(); it++) {
		new_array->push_clone(*it);
	}
	return new_array;
}

template <class T>
LSArray<T>* LSArray<T>::filter(const void* function) const {
	LSArray<T>* new_array = new LSArray<T>();
	new_array->reserve(this->size());
	auto fun = (bool (*)(void*)) function;
	for (auto v : *this) {
		if (fun(v)) new_array->push_clone(v);
	}
	return new_array;
}

template <>
inline LSArray<int>* LSArray<int>::filter(const void* function) const {
	LSArray<int>* new_array = new LSArray<int>();
	new_array->reserve(this->size());
	auto fun = (int (*)(int)) function;
	for (auto v : *this) {
		if (fun(v)) {
			new_array->push_clone(v);
		}
	}
	return new_array;
}

template <class T>
LSValue* LSArray<T>::foldLeft(const void* function, const LSValue* v0) const {
	auto fun = (LSValue* (*)(LSValue*, LSValue*)) function;
	LSValue* result = v0->clone();
	for (auto v : *this) {
		result = fun(result, v);
	}
	return result;
}

template <>
inline LSValue* LSArray<int>::foldLeft(const void* function, const LSValue* v0) const {
	auto fun = (LSValue* (*)(LSValue*, int)) function;
	LSValue* result = v0->clone();
	for (auto v : *this) {
		result = fun(result, v);
	}
	return result;
}

template <class T>
LSValue* LSArray<T>::foldRight(const void* function, const LSValue* v0) const {
	auto fun = (LSValue* (*)(LSValue*, LSValue*)) function;
	LSValue* result = v0->clone();
	for (auto it = this->rbegin(); it != this->rend(); it++) {
		result = fun(*it, result);
	}
	return result;
}

template <>
inline LSValue* LSArray<int>::foldRight(const void* function, const LSValue* v0) const {
	auto fun = (LSValue* (*)(int, LSValue*)) function;
	LSValue* result = v0->clone();
	for (auto it = this->rbegin(); it != this->rend(); it++) {
		result = fun(*it, result);
	}
	return result;
}

template <class T>
LSArray<T>* LSArray<T>::insert_v(const T v, const LSValue* pos) {
	if (const LSNumber* n = dynamic_cast<const LSNumber*>(pos)) {
		if (this->size() <= n->value) {
			this->resize(n->value);
		}
		this->insert(this->begin() + (int) n->value, v);
	}
	this->refs++;
	return this;
}

template <>
inline LSArray<int>* LSArray<int>::insert_v(const int v, const LSValue* pos) {
	if (const LSNumber* n = dynamic_cast<const LSNumber*>(pos)) {
		if (this->size() <= n->value) {
			this->resize(n->value);
		}
		this->insert(this->begin() + (int) n->value, (int) v);
	}
	this->refs++;
	return this;
}

template <>
inline LSArray<LSValue*>* LSArray<LSValue*>::partition(const void* function) const {

	LSArray<LSValue*>* array_true = new LSArray<LSValue*>();
	LSArray<LSValue*>* array_false = new LSArray<LSValue*>();
	auto fun = (bool (*)(void*)) function;

	for (auto v : *this) {
		if (fun(v)) {
			array_true->push_clone(v);
		} else {
			array_false->push_clone(v);
		}
	}
	return new LSArray<LSValue*> {array_true, array_false};
}

template <>
inline LSArray<LSValue*>* LSArray<int>::partition(const void* function) const {

	LSArray<int>* array_true = new LSArray<int>();
	LSArray<int>* array_false = new LSArray<int>();
	auto fun = (bool (*)(int)) function;

	for (auto v : *this) {
		if (fun(v)) {
			array_true->push_clone(v);
		} else {
			array_false->push_clone(v);
		}
	}
	return new LSArray<LSValue*> {array_true, array_false};
}

template <class T>
LSArray<LSValue*>* LSArray<T>::map2(const LSArray<LSValue*>* array, const void* function) const {
	LSArray<LSValue*>* new_array = new LSArray<LSValue*>();
	new_array->reserve(this->size());
	auto fun = (void* (*)(void*, void*)) function;

	for (unsigned i = 0; i < this->size(); ++i) {
		LSValue* v1 = this->operator [] (i);
		LSValue* v2 = ((LSArray<LSValue*>*) array)->operator [] (i);
		LSValue* res = (LSValue*) fun(v1, v2);
		new_array->push_clone(res); // push_no_clone ?
		LSValue::delete_val(res);
	}
	return new_array;
}

template <>
inline LSArray<LSValue*>* LSArray<int>::map2(const LSArray<LSValue*>* array, const void* function) const {

	LSArray<LSValue*>* new_array = new LSArray<LSValue*>();
	new_array->reserve(this->size());
	auto fun = (void* (*)(void*, void*)) function;

	for (unsigned i = 0; i < this->size(); ++i) {
		LSValue* v1 = LSNumber::get(this->operator [] (i));
		LSValue* v2 = array->operator [] (i);
		LSValue* res = (LSValue*) fun(v1, v2);
		new_array->push_clone(res);
		LSValue::delete_val(res);
	}
	return new_array;
}

template <class T>
LSArray<LSValue*>* LSArray<T>::map2_int(const LSArray<int>* array, const void* function) const {

	LSArray<LSValue*>* new_array = new LSArray<LSValue*>();
	new_array->reserve(this->size());
	auto fun = (void* (*)(void*, int)) function;

	for (unsigned i = 0; i < this->size(); ++i) {
		LSValue* v1 = this->operator [] (i);
		int v2 = array->operator [] (i);
		LSValue* res = (LSValue*) fun(v1, v2);
		new_array->push_clone(res);
		LSValue::delete_val(res);
	}
	return new_array;
}

template <>
inline LSArray<LSValue*>* LSArray<int>::map2_int(const LSArray<int>* array, const void* function) const {

	LSArray<LSValue*>* new_array = new LSArray<LSValue*>();
	new_array->reserve(this->size());
	auto fun = (void* (*)(int, int)) function;

	for (unsigned i = 0; i < this->size(); ++i) {
		int v1 = this->operator [] (i);
		int v2 = array->operator [] (i);
		LSValue* res = (LSValue*) fun(v1, v2);
		new_array->push_clone(res);
		LSValue::delete_val(res);
	}
	return new_array;
}

template <class T>
int LSArray<T>::search(const LSValue* value, const int start) const {

	for (unsigned i = 0; i < this->size(); i++) {
		if (i < (unsigned) start) continue; // i < start
		if (value->operator == (this->operator[] (i))) {
			return i;
		}
	}
	return -1;
}

template <>
inline int LSArray<int>::search(const LSValue* value, const int start) const {

	if (const LSNumber* n = dynamic_cast<const LSNumber*>(value)) {
		for (unsigned i = 0; i < this->size(); i++) {
			if (i < (unsigned) start) continue; // i < start
			if (n->value == this->operator[] (i)) {
				return i;
			}
		}
	}
	return -1;
}

template <>
inline int LSArray<int>::search_int(const int value, const int start) const {

	for (unsigned i = 0; i < this->size(); i++) {
		if (i < (unsigned) start) continue; // i < start
		if (value == (this->operator[] (i))) {
			return i;
		}
	}
	return -1;
}

template <class T>
LSString* LSArray<T>::join(const LSString* glue) const {
	if (this->empty()) {
		return new LSString();
	}
	auto it = this->begin();
	LSString* empty = new LSString();
	LSValue* result = (*it)->operator + (empty);
	LSValue::delete_val(empty);
	for (it++; it != this->end(); it++) {
		LSValue* n1 = glue->operator + (result);
		LSValue* n = (*it)->operator + (n1);
		LSValue::delete_val(result);
		LSValue::delete_val(n1);
		result = n;
	}
	return (LSString*) result;
}

template <>
inline LSString* LSArray<int>::join(const LSString* glue) const {
	if (this->empty()) {
		return new LSString();
	}
	unsigned i = 0;
	std::string result = std::to_string(this->operator[] (i));
	for (i++; i < this->size(); i++) {
		result = std::to_string(this->operator[] (i)) + *glue + result;
	}
	return new LSString(result);
}

template <class T>
LSArray<T>* LSArray<T>::fill(const LSValue* element, const int size) {
	this->clear();
	for (int i = 0; i < size; i++) {
		this->push_clone((LSValue*) element);
	}
	this->refs++;
	return this;
}

template <>
inline LSArray<int>* LSArray<int>::fill(const LSValue* element, const int size) {
	this->clear();
	for (int i = 0; i < size; i++) {
		this->push_clone(((LSNumber*) element)->value);
	}
	this->refs++;
	return this;
}


/*
 * LSValue methods
 */
template <class T>
bool LSArray<T>::isTrue() const {
	return this->size() > 0;
}

template <class T>
LSValue* LSArray<T>::operator ! () const {
	return LSBoolean::get(this->size() == 0);
}

template <class T>
LSValue* LSArray<T>::operator ~ () const {
	LSArray<T>* array = new LSArray<T>();
	array->reserve(this->size());
	for (auto i = this->rbegin(); i != this->rend(); ++i) {
		array->push_clone(*i);
	}
	return array;
}




template <class T>
LSValue* LSArray<T>::operator + (const LSValue* v) const {
	return v->operator + (this);
}

template <class T>
inline LSValue* LSArray<T>::operator + (const LSNull* v) const {
	LSArray* new_array = (LSArray*) this->clone();
	new_array->push_clone((T) v);
	return new_array;
}
template <>
inline LSValue* LSArray<int>::operator + (const LSNull*) const {
	return LSNull::get();
}
template <>
inline LSValue* LSArray<double>::operator + (const LSNull*) const {
	return LSNull::get();
}

template <class T>
inline LSValue* LSArray<T>::operator + (const LSBoolean* boolean) const {
	LSArray* new_array = (LSArray*) this->clone();
	new_array->push_clone((T) boolean);
	return new_array;
}
template <>
inline LSValue* LSArray<int>::operator + (const LSBoolean*) const {
	return LSNull::get();
}
template <>
inline LSValue* LSArray<double>::operator + (const LSBoolean*) const {
	return LSNull::get();
}

template <class T>
inline LSValue* LSArray<T>::operator + (const LSNumber* v) const {
	LSArray* new_array = (LSArray*) this->clone();
	new_array->push_clone((T) v);
	return new_array;
}
template <>
inline LSValue* LSArray<int>::operator + (const LSNumber* number) const {
	LSArray* new_array = (LSArray*) this->clone();
	new_array->push_clone(number->value);
	return new_array;
}
template <>
inline LSValue* LSArray<double>::operator + (const LSNumber* number) const {
	LSArray* new_array = (LSArray*) this->clone();
	new_array->push_clone(number->value);
	return new_array;
}

template <class T>
inline LSValue* LSArray<T>::operator + (const LSString* string) const {
	LSArray* new_array = (LSArray*) this->clone();
	new_array->push_clone((T) string);
	return new_array;
}
template <>
inline LSValue* LSArray<int>::operator + (const LSString*) const {
	return LSNull::get();
}
template <>
inline LSValue* LSArray<double>::operator + (const LSString*) const {
	return LSNull::get();
}

template <class T>
inline LSValue* LSArray<T>::operator + (const LSArray<LSValue*>* array) const {
	LSArray<LSValue*>* new_array = (LSArray<LSValue*>*) this->clone();
	new_array->reserve(new_array->size() + array->size());
	for (auto v : *array) {
		new_array->push_clone(v);
	}
	return new_array;
}
template <>
inline LSValue* LSArray<int>::operator + (const LSArray<LSValue*>*) const {
	return LSNull::get();
}
template <>
inline LSValue* LSArray<double>::operator + (const LSArray<LSValue*>*) const {
	return LSNull::get();
}
template <class T>
inline LSValue* LSArray<T>::operator + (const LSArray<int>*) const {
	return LSNull::get();
}
template <>
inline LSValue* LSArray<int>::operator + (const LSArray<int>* array) const {
	LSArray<int>* new_array = new LSArray<int>();
	new_array->insert(new_array->end(), this->begin(), this->end());
	new_array->insert(new_array->end(), array->begin(), array->end());
	return new_array;
}
template <class T>
inline LSValue* LSArray<T>::operator + (const LSArray<double>*) const {
	return LSNull::get();
}
template <>
inline LSValue* LSArray<double>::operator + (const LSArray<double>* array) const {
	LSArray<double>* new_array = new LSArray<double>();
	new_array->insert(new_array->end(), this->begin(), this->end());
	new_array->insert(new_array->end(), array->begin(), array->end());
	return new_array;
}

template <class T>
inline LSValue* LSArray<T>::operator + (const LSObject* object) const {
	LSArray* new_array = (LSArray*) this->clone();
	new_array->push_clone((T) object);
	return new_array;
}
template <>
inline LSValue* LSArray<int>::operator + (const LSObject*) const {
	return LSNull::get();
}
template <>
inline LSValue* LSArray<double>::operator + (const LSObject*) const {
	return LSNull::get();
}

template <class T>
inline LSValue* LSArray<T>::operator + (const LSFunction* fun) const {
	LSArray* new_array = (LSArray*) this->clone();
	new_array->push_clone((T) fun);
	return new_array;
}
template <>
inline LSValue* LSArray<int>::operator + (const LSFunction*) const {
	return LSNull::get();
}
template <>
inline LSValue* LSArray<double>::operator + (const LSFunction*) const {
	return LSNull::get();
}

template <class T>
inline LSValue* LSArray<T>::operator + (const LSClass* clazz) const {
	LSArray* new_array = (LSArray*) this->clone();
	new_array->push_clone((T) clazz);
	return new_array;
}
template <>
inline LSValue* LSArray<int>::operator + (const LSClass*) const {
	return LSNull::get();
}
template <>
inline LSValue* LSArray<double>::operator + (const LSClass*) const {
	return LSNull::get();
}




template <class T>
LSValue* LSArray<T>::operator += (LSValue* value) {
	return value->operator += (this);
}

template <class T>
inline LSValue* LSArray<T>::operator += (const LSNull* null) {
	push_clone((T) null);
	return LSNull::get();
}
template <>
inline LSValue* LSArray<int>::operator += (const LSNull*) {
	return LSNull::get();
}
template <>
inline LSValue* LSArray<double>::operator += (const LSNull*) {
	return LSNull::get();
}

template <class T>
inline LSValue* LSArray<T>::operator += (const LSBoolean* boolean) {
	push_clone((T) boolean);
	return LSNull::get();
}
template <>
inline LSValue* LSArray<int>::operator += (const LSBoolean*) {
	return LSNull::get();
}
template <>
inline LSValue* LSArray<double>::operator += (const LSBoolean*) {
	return LSNull::get();
}

template <class T>
inline LSValue* LSArray<T>::operator += (const LSNumber* num) {
	push_clone((T) num);
	return LSNull::get();
}
template <>
inline LSValue* LSArray<int>::operator += (const LSNumber*) {
	return LSNull::get();
}
template <>
inline LSValue* LSArray<double>::operator += (const LSNumber*) {
	return LSNull::get();
}

template <class T>
inline LSValue* LSArray<T>::operator += (const LSString* string) {
	push_clone((T) string);
	return LSNull::get();
}
template <>
inline LSValue* LSArray<int>::operator += (const LSString*) {
	return LSNull::get();
}
template <>
inline LSValue* LSArray<double>::operator += (const LSString*) {
	return LSNull::get();
}

template <class T>
inline LSValue* LSArray<T>::operator += (const LSArray<LSValue*>* array) {

	LSArray* arr = (LSArray*) array;

	if (array == (void*)this) {
		arr = (LSArray*) array->clone();
	}
	for (auto i = arr->begin(); i != arr->end(); ++i) {
		push_clone(*i);
	}
	if (array == (void*)this) {
		delete arr;
	}
	return LSNull::get();
}

template <class T>
inline LSValue* LSArray<T>::operator += (const LSObject* object) {
	push_clone((T) object);
	return LSNull::get();
}
template <>
inline LSValue* LSArray<int>::operator += (const LSObject*) {
	return LSNull::get();
}
template <>
inline LSValue* LSArray<double>::operator += (const LSObject*) {
	return LSNull::get();
}

template <class T>
inline LSValue* LSArray<T>::operator += (const LSFunction* fun) {
	push_clone((T) fun);
	return LSNull::get();
}
template <>
inline LSValue* LSArray<int>::operator += (const LSFunction*) {
	return LSNull::get();
}
template <>
inline LSValue* LSArray<double>::operator += (const LSFunction*) {
	return LSNull::get();
}

template <class T>
inline LSValue* LSArray<T>::operator += (const LSClass* clazz) {
	push_clone((T) clazz);
	return LSNull::get();
}
template <>
inline LSValue* LSArray<int>::operator += (const LSClass*) {
	return LSNull::get();
}
template <>
inline LSValue* LSArray<double>::operator += (const LSClass*) {
	return LSNull::get();
}


template <class T>
LSValue* LSArray<T>::operator - (const LSValue* value) const {
	return value->operator - (this);
}
template <class T>
LSValue* LSArray<T>::operator -= (LSValue* value) {
	return value->operator -= (this);
}
template <class T>
LSValue* LSArray<T>::operator * (const LSValue* value) const {
	return value->operator * (this);
}
template <class T>
LSValue* LSArray<T>::operator *= (LSValue* value) {
	return value->operator *= (this);
}
template <class T>
LSValue* LSArray<T>::operator / (const LSValue* value) const {
	return value->operator / (value);
}
template <class T>
LSValue* LSArray<T>::operator /= (LSValue* value) {
	return value->operator /= (this);
}
template <class T>
LSValue* LSArray<T>::poww(const LSValue* value) const {
	return value->poww(this);
}
template <class T>
LSValue* LSArray<T>::pow_eq(LSValue* value) {
	return value->pow_eq(this);
}
template <class T>
LSValue* LSArray<T>::operator % (const LSValue* value) const {
	return value->operator % (this);
}
template <class T>
LSValue* LSArray<T>::operator %= (LSValue* value) {
	return value->operator %= (this);
}

template <class T>
bool LSArray<T>::operator == (const LSValue* v) const {
	return v->operator == (this);
}

template <class T>
bool LSArray<T>::operator == (const LSArray<LSValue*>* v) const {

	if (this->size() != v->size()) {
		return false;
	}
	auto i = this->begin();
	auto j = v->begin();

	for (; i != this->end(); i++, j++) {
		if ((*i)->operator != (*j)) return false;
	}
	return true;
}

template <>
inline bool LSArray<int>::operator == (const LSArray<LSValue*>* v) const {

	if (this->size() != v->size()) {
		return false;
	}
	auto i = this->begin();
	auto j = v->begin();

	for (; i != this->end(); i++, j++) {
		const LSNumber* n = dynamic_cast<const LSNumber*>(*j);
		if (!n) return false;
		if (n->value != *i) return false;
	}
	return true;
}

template <>
inline bool LSArray<double>::operator == (const LSArray<LSValue*>* v) const {

	if (this->size() != v->size()) {
		return false;
	}
	auto i = this->begin();
	auto j = v->begin();

	for (; i != this->end(); i++, j++) {
		const LSNumber* n = dynamic_cast<const LSNumber*>(*j);
		if (!n) return false;
		if (n->value != *i) return false;
	}
	return true;
}

template <class T>
inline bool LSArray<T>::operator < (const LSValue* v) const {
	return v->operator < (this);
}

template <class T>
inline bool LSArray<T>::operator < (const LSNull*) const {
	return false;
}

template <class T>
inline bool LSArray<T>::operator < (const LSBoolean*) const {
	return false;
}

template <class T>
inline bool LSArray<T>::operator < (const LSNumber*) const {
	return false;
}

template <class T>
inline bool LSArray<T>::operator < (const LSString*) const {
	return false;
}

template <>
inline bool LSArray<LSValue*>::operator < (const LSArray<LSValue*>* v) const {
	return std::lexicographical_compare(begin(), end(), v->begin(), v->end(), [](LSValue* a, LSValue* b) -> bool {
		return a->operator > (b);
	});
}
template <>
inline bool LSArray<int>::operator < (const LSArray<LSValue*>* v) const {
	auto it1 = begin();
	auto it2 = v->begin();
	while (it1 != end()) {
		if (it2 == v->end()) return false;
		if ((*it2)->typeID() < 3) return false;
		if (3 < (*it2)->typeID()) return true;
		if (*it1 < ((LSNumber*) *it2)->value) return true;
		if (((LSNumber*) *it2)->value < *it1) return false;
		++it1; ++it2;
	}
	return (it2 != v->end());
}
template <>
inline bool LSArray<double>::operator < (const LSArray<LSValue*>* v) const {
	auto it1 = begin();
	auto it2 = v->begin();
	while (it1 != end()) {
		if (it2 == v->end()) return false;
		if ((*it2)->typeID() < 3) return false;
		if (3 < (*it2)->typeID()) return true;
		if (*it1 < ((LSNumber*) *it2)->value) return true;
		if (((LSNumber*) *it2)->value < *it1) return false;
		++it1; ++it2;
	}
	return (it2 != v->end());
}
template <>
inline bool LSArray<LSValue*>::operator < (const LSArray<int>* v) const {
	auto it1 = begin();
	auto it2 = v->begin();
	while (it1 != end()) {
		if (it2 == v->end()) return false;
		if (3 < (*it1)->typeID()) return false;
		if ((*it1)->typeID() < 3) return true;
		if (((LSNumber*) *it1)->value < *it2) return true;
		if (*it2 < ((LSNumber*) *it1)->value) return false;
		++it1; ++it2;
	}
	return (it2 != v->end());
}
template <>
inline bool LSArray<int>::operator < (const LSArray<int>* v) const {
	return std::lexicographical_compare(begin(), end(), v->begin(), v->end());
}
template <>
inline bool LSArray<double>::operator < (const LSArray<int>* v) const {
	return std::lexicographical_compare(begin(), end(), v->begin(), v->end());
}
template <>
inline bool LSArray<LSValue*>::operator < (const LSArray<double>* v) const {
	auto it1 = begin();
	auto it2 = v->begin();
	while (it1 != end()) {
		if (it2 == v->end()) return false;
		if (3 < (*it1)->typeID()) return false;
		if ((*it1)->typeID() < 3) return true;
		if (((LSNumber*) *it1)->value < *it2) return true;
		if (*it2 < ((LSNumber*) *it1)->value) return false;
		++it1; ++it2;
	}
	return (it2 != v->end());
}
template <>
inline bool LSArray<int>::operator < (const LSArray<double>* v) const {
	return std::lexicographical_compare(begin(), end(), v->begin(), v->end());
}
template <>
inline bool LSArray<double>::operator < (const LSArray<double>* v) const {
	return std::lexicographical_compare(begin(), end(), v->begin(), v->end());
}


template <class T>
inline bool LSArray<T>::operator < (const LSObject*) const {
	return true;
}

template <class T>
inline bool LSArray<T>::operator < (const LSFunction*) const {
	return true;
}

template <class T>
inline bool LSArray<T>::operator < (const LSClass*) const {
	return true;
}


template <class T>
inline bool LSArray<T>::in(const LSValue* key) const {
	for (auto i = this->begin(); i != this->end(); i++) {
		if ((*i)->operator == (key)) {
			return true;
		}
	}
	return false;
}

template <>
inline bool LSArray<int>::in(const LSValue* key) const {
	if (const LSNumber* n = dynamic_cast<const LSNumber*>(key)) {
		for (auto i = this->begin(); i != this->end(); i++) {
			if ((*i) == n->value) {
				return true;
			}
		}
	}
	return false;
}

template <>
inline bool LSArray<double>::in(const LSValue* key) const {
	if (const LSNumber* n = dynamic_cast<const LSNumber*>(key)) {
		for (auto i = this->begin(); i != this->end(); i++) {
			if ((*i) == n->value) {
				return true;
			}
		}
	}
	return false;
}

template <>
inline int LSArray<int>::atv(const int i) {
	return this->operator[] (i);
}

template <>
inline int* LSArray<int>::atLv(int i) {
	return &this->operator[] (i);
}

template <class T>
LSValue* LSArray<T>::range(int start, int end) const {

	LSArray<T>* range = new LSArray<T>();

	unsigned start_i = std::max(0, (int) start);
	unsigned end_i = std::min((int) size() - 1, (int) end);

	for (unsigned i = start_i; i <= end_i; i++) {
		range->push_clone(this->operator [] (i));
	}
	return range;
}

template <>
inline LSValue* LSArray<int>::range(int start, int end) const {

	LSArray<int>* range = new LSArray<int>();

	unsigned start_i = std::max(0, (int) start);
	unsigned end_i = std::min((int) size() - 1, (int) end);

	for (unsigned i = start_i; i <= end_i; i++) {
		range->push_no_clone(this->operator [] (i));
	}
	return range;
}

template <class T>
LSValue* LSArray<T>::rangeL(int, int) {
	return this;
}


template <class T>
LSValue* LSArray<T>::clone() const {
	LSArray<T>* new_array = new LSArray<T>();
	new_array->reserve(this->size());

	for (auto i = this->begin(); i != this->end(); i++) {
		new_array->push_clone(*i);
	}
	return new_array;
}

template <>
inline std::ostream& LSArray<LSValue*>::print(std::ostream& os) const {
	os << "array[";
	for (auto i = this->begin(); i != this->end(); i++) {
		if (i != this->begin()) os << ", ";
		(*i)->print(os);
//		os << " " << *i;
//		os << " " << (*i)->refs;
	}
	os << "]";
	return os;
}

template <>
inline std::ostream& LSArray<int>::print(std::ostream& os) const {
	os << "array[";
	for (auto i = this->begin(); i != this->end(); i++) {
		if (i != this->begin()) os << ", ";
		os << (*i);
	}
	os << "]";
	return os;
}

template <>
inline std::ostream& LSArray<double>::print(std::ostream& os) const {
	os << "array[";
	for (auto i = this->begin(); i != this->end(); i++) {
		if (i != this->begin()) os << ", ";
		os << (*i);
	}
	os << "]";
	return os;
}

template <class T>
std::string LSArray<T>::json() const {
	std::string res = "[";
	for (auto i = this->begin(); i != this->end(); i++) {
		if (i != this->begin()) res += ",";
		std::string json = (*i)->to_json();
		res += json;
	}
	return res + "]";
}

template <>
inline std::string LSArray<int>::json() const {
	std::string res = "[";
	for (auto i = this->begin(); i != this->end(); i++) {
		if (i != this->begin()) res += ",";
		std::string json = std::to_string(*i);
		res += json;
	}
	return res + "]";
}

template <>
inline std::string LSArray<double>::json() const {
	std::string res = "[";
	for (auto i = this->begin(); i != this->end(); i++) {
		if (i != this->begin()) res += ",";
		std::string json = std::to_string(*i);
		res += json;
	}
	return res + "]";
}

template <class T>
LSValue* LSArray<T>::getClass() const {
	return LSArray<T>::array_class;
}

template <class T>
int LSArray<T>::typeID() const {
	return 5;
}

template <class T>
const BaseRawType* LSArray<T>::getRawType() const {
	return RawType::ARRAY;
}

template <class T>
LSValue* LSArray<T>::at(const LSValue* key) const {

	if (const LSNumber* n = dynamic_cast<const LSNumber*>(key)) {
		try {
			return (LSValue*) ((std::vector<T>*) this)->at((int) n->value)->clone();
		} catch (std::exception& e) {
			return LSNull::get();
		}
	}
	return LSNull::get();
}

template <>
inline LSValue* LSArray<int>::at(const LSValue* key) const {

	if (const LSNumber* n = dynamic_cast<const LSNumber*>(key)) {
		try {
			return LSNumber::get(((std::vector<int>*) this)->at((int) n->value));
		} catch (std::exception& e) {
			return LSNull::get();
		}
	}
	return LSNull::get();
}

template <>
inline LSValue* LSArray<double>::at(const LSValue* key) const {

	if (const LSNumber* n = dynamic_cast<const LSNumber*>(key)) {
		try {
			return LSNumber::get(((std::vector<double>*) this)->at((int) n->value));
		} catch (std::exception& e) {
			return LSNull::get();
		}
	}
	return LSNull::get();
}

//template <>
//inline LSValue* LSArray<int>::operator += (const LSNumber* number) {
//	push_clone(number->value);
//	return this;
//}

template <>
inline LSArray<int>* LSArray<int>::push_all(const LSArray<LSValue*>* array) {
	for (auto v : *array) {
		if (const LSNumber* n = dynamic_cast<const LSNumber*>(v)) {
			push_clone(n->value);
		}
	}
	return this;
}

template <class T>
const LSArray<T>* LSArray<T>::push_all_int(const LSArray<int>* array) {
	for (auto v : *array) {
		push_clone((T) LSNumber::get(v));
	}
	return this;
}

template <class T>
LSValue* LSArray<T>::attr(const LSValue* key) const {

	if (*((LSString*) key) == "size") {
		return LSNumber::get(this->size());
	}
	if (*((LSString*) key) == "class") {
		return getClass();
	}
	return LSNull::get();
}


template <class T>
LSValue** LSArray<T>::atL(const LSValue* key) {
	if (const LSNumber* n = dynamic_cast<const LSNumber*>(key)) {
		try {
			LSValue** v = (LSValue**) &(((std::vector<T>*)this)->at((int) n->value));
			return v;
		} catch (std::exception& e) {
			return nullptr;
		}
	}
	return nullptr;
}

template <class T>
LSValue** LSArray<T>::attrL(const LSValue*) {
	return nullptr;
}

template <class T>
LSValue* LSArray<T>::abso() const {
	return LSNumber::get(size());
}

} // end of namespace ls

#endif

