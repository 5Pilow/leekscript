#ifndef LS_MAP_TCC
#define LS_MAP_TCC

#include "LSNumber.hpp"
#include "LSArray.hpp"

#include <exception>

namespace ls {

template <typename K>
inline bool lsmap_less<K>::operator()(K lhs, K rhs) const {
	return lhs < rhs;
}

template <>
inline bool lsmap_less<LSValue*>::operator()(LSValue* lhs, LSValue* rhs) const {
	return *lhs < *rhs;
}

template <class K, class T>
LSMap<K, T>* LSMap<K, T>::constructor() {
	return new LSMap<K, T>();
}

template <class K, class T>
LSMap<K, T>::LSMap() : LSValue(MAP) {}

template <class K, class V>
LSMap<K, V>::~LSMap() {
	for (auto it = this->begin(); it != this->end(); ++it) {
		ls::unref(it->first);
		ls::unref(it->second);
	}
}

/*
 * Map methods
 */
template <class K, class V>
bool LSMap<K, V>::ls_insert(K key, V value) {
	auto it = this->lower_bound(key);
	if (it == this->end() || !ls::equals(it->first, key)) {
		this->emplace_hint(it, ls::move_inc(key), ls::move_inc(value));
		if (refs == 0) delete this;
		return true;
	}
	ls::release(key);
	ls::release(value);
	if (refs == 0) delete this;
	return false;
}

template <class K, class V>
void LSMap<K, V>::ls_emplace(K key, V value) {
	auto it = this->lower_bound(key);
	if (it == this->end() || !ls::equals(it->first, key)) {
		this->emplace_hint(it, ls::move_inc(key), ls::move_inc(value));
	} else {
		ls::release(key);
		ls::release(value);
	}
}

template <class K, class V>
LSMap<K, V>* LSMap<K, V>::ls_clear() {
	for (auto it = this->begin(); it != this->end(); ++it) {
		ls::unref(it->first);
		ls::unref(it->second);
	}
	this->clear();
	return this;
}

template <class K, class V>
bool LSMap<K, V>::ls_erase(K key) {
	auto it = this->find(key);
	ls::release(key);
	if (it != this->end()) {
		ls::unref(it->first);
		ls::unref(it->second);
		this->erase(it);
		LSValue::delete_temporary(this);
		return true;
	}
	LSValue::delete_temporary(this);
	return false;
}

template <class K, class V>
template <class V2>
V2 LSMap<K, V>::ls_look(K key, V2 def) {
	auto it = this->find(key);
	ls::release(key);
	if (it != this->end()) {
		ls::release(def);
		if (refs == 0) {
			V r = ls::clone(it->second);
			LSValue::delete_temporary(this);
			return ls::convert<V2>(r);
		}
		return ls::convert<V2>(it->second);
	}
	LSValue::delete_temporary(this);
	return def;
}

template <class K, class V>
V LSMap<K, V>::ls_max() {
	if (this->empty()) {
		LSValue::delete_temporary(this);
		throw vm::ExceptionObj(vm::Exception::ARRAY_OUT_OF_BOUNDS);
	}
	auto it = this->begin();
	auto max = it->second;
	for (; it != this->end(); ++it) {
		if (ls::lt(max, it->second)) {
			max = it->second;
		}
	}
	if (refs == 0) {
		max = ls::clone(max);
		LSValue::delete_temporary(this);
	}
	return max;
}

template <class K, class V>
K LSMap<K, V>::ls_maxKey() {
	if (this->empty()) {
		LSValue::delete_temporary(this);
		throw vm::ExceptionObj(vm::Exception::ARRAY_OUT_OF_BOUNDS);
	}
	auto it = this->begin();
	auto max = it->first;
	for (; it != this->end(); ++it) {
		if (ls::lt(max, it->first)) {
			max = it->first;
		}
	}
	if (refs == 0) {
		max = ls::clone(max);
		LSValue::delete_temporary(this);
	}
	return max;
}

template <class K, class V>
V LSMap<K, V>::ls_min() {
	if (this->empty()) {
		LSValue::delete_temporary(this);
		throw vm::ExceptionObj(vm::Exception::ARRAY_OUT_OF_BOUNDS);
	}
	auto it = this->begin();
	auto min = it->second;
	for (; it != this->end(); ++it) {
		if (ls::lt(it->second, min)) {
			min = it->second;
		}
	}
	if (refs == 0) {
		min = ls::clone(min);
		LSValue::delete_temporary(this);
	}
	return min;
}

template <class K, class V>
K LSMap<K, V>::ls_minKey() {
	if (this->empty()) {
		LSValue::delete_temporary(this);
		throw vm::ExceptionObj(vm::Exception::ARRAY_OUT_OF_BOUNDS);
	}
	auto it = this->begin();
	auto min = it->first;
	for (; it != this->end(); ++it) {
		if (ls::lt(it->first, min)) {
			min = it->first;
		}
	}
	if (refs == 0) {
		min = ls::clone(min);
		LSValue::delete_temporary(this);
	}
	return min;
}

template <typename K, typename V>
LSArray<V>* LSMap<K, V>::values() const {
	auto array = new LSArray<V>();
	for (auto i : *this) {
		array->push_clone(i.second);
	}
	LSValue::delete_temporary(this);
	return array;
}

template <class K, class V>
template <class F>
void LSMap<K, V>::ls_iter(F function) const {
	for (auto v : *this) {
		ls::call<void>(function, v.first, v.second);
	}
	LSValue::delete_temporary(this);
}

/*
 * LSValue methods
 */
template <class K, class T>
bool LSMap<K, T>::to_bool() const {
	return not this->empty();
}

template <class K, class T>
bool LSMap<K, T>::ls_not() const {
	auto r = this->size() == 0;
	LSValue::delete_temporary(this);
	return r;
}

template <class K, class T>
bool LSMap<K, T>::in(K key) const {
	bool r = this->find(key) != this->end();
	LSValue::delete_temporary(this);
	ls::release(key);
	return r;
}

template <class K, class T>
template <class K2, class T2>
bool LSMap<K, T>::map_equals(const LSMap<K2, T2>* map) const {
	if (this->size() != map->size()) {
		return false;
	}
	auto i = this->begin();
	auto j = map->begin();
	while (i != this->end()) {
		if (!ls::equals(i->first, j->first)) return false;
		if (!ls::equals(i->second, j->second)) return false;
		++j;
		++i;
	}
	return j == map->end();
}

template <class K, class T>
bool LSMap<K, T>::eq(const LSValue* v) const {
	if (auto map = dynamic_cast<const LSMap<LSValue*, LSValue*>*>(v))
		return map_equals(map);
	if (auto map = dynamic_cast<const LSMap<LSValue*, double>*>(v))
		return map_equals(map);
	if (auto map = dynamic_cast<const LSMap<LSValue*, int>*>(v))
		return map_equals(map);
	if (auto map = dynamic_cast<const LSMap<int, LSValue*>*>(v))
		return map_equals(map);
	if (auto map = dynamic_cast<const LSMap<double, LSValue*>*>(v))
		return map_equals(map);
	if (auto map = dynamic_cast<const LSMap<int, double>*>(v))
		return map_equals(map);
	if (auto map = dynamic_cast<const LSMap<int, int>*>(v))
		return map_equals(map);
	if (auto map = dynamic_cast<const LSMap<double, double>*>(v))
		return map_equals(map);
	if (auto map = dynamic_cast<const LSMap<double, int>*>(v))
		return map_equals(map);
	return false;
}

template <class K, class T>
template <class K2, class T2>
bool LSMap<K, T>::map_lt(const LSMap<K2, T2>* map) const {
	auto i = this->begin();
	auto j = map->begin();
	for (; i != this->end(); ++i, ++j) {
		if (j == map->end()) return false;
		if (ls::lt(i->first, j->first)) return true;
		if (ls::lt(j->first, i->first)) return false;
		if (ls::lt(i->second, j->second)) return true;
		if (ls::lt(j->second, i->second)) return false;
	}
	return j != map->end();
}

template <class K, class T>
bool LSMap<K, T>::lt(const LSValue* v) const {
	if (auto map = dynamic_cast<const LSMap<LSValue*, LSValue*>*>(v))
		return map_lt(map);
	if (auto map = dynamic_cast<const LSMap<LSValue*, double>*>(v))
		return map_lt(map);
	if (auto map = dynamic_cast<const LSMap<LSValue*, int>*>(v))
		return map_lt(map);
	if (auto map = dynamic_cast<const LSMap<int, LSValue*>*>(v))
		return map_lt(map);
	if (auto map = dynamic_cast<const LSMap<double, LSValue*>*>(v))
		return map_lt(map);
	if (auto map = dynamic_cast<const LSMap<int, double>*>(v))
		return map_lt(map);
	if (auto map = dynamic_cast<const LSMap<int, int>*>(v))
		return map_lt(map);
	if (auto map = dynamic_cast<const LSMap<double, double>*>(v))
		return map_lt(map);
	if (auto map = dynamic_cast<const LSMap<double, int>*>(v))
		return map_lt(map);
	return LSValue::lt(v);
}

template <class K, class V>
V LSMap<K, V>::at(const K key) const {
	bool ex = false;
	try {
		auto map = (std::map<K, V, lsmap_less<K>>*) this;
		return map->at(key);
	} catch (std::exception&) {
		ex = true;
	}
	if (ex)
		throw vm::ExceptionObj(vm::Exception::ARRAY_OUT_OF_BOUNDS);
	assert(false); // LCOV_EXCL_LINE
}

template <typename K, typename T>
inline LSValue** LSMap<K, T>::atL(const LSValue* key) {
	// Can't apply default atL operator on maps with non-pointer values,
	// like map<K, int> and map<K, double>
	LSValue::delete_temporary(key);
	throw vm::ExceptionObj(vm::Exception::NO_SUCH_OPERATOR);
	assert(false); // LCOV_EXCL_LINE
}

template <>
inline LSValue** LSMap<LSValue*, LSValue*>::atL(const LSValue* key) {
	return this->atL_base((LSValue*) key);
}

template <>
inline LSValue** LSMap<int, LSValue*>::atL(const LSValue* key) {
	if (key->type == NUMBER) {
		auto r = this->atL_base(static_cast<const LSNumber*>(key)->value);
		LSValue::delete_temporary(key);
		return r;
	}
	LSValue::delete_temporary(key);
	throw vm::ExceptionObj(vm::Exception::NO_SUCH_OPERATOR);
	assert(false); // LCOV_EXCL_LINE
}

template <>
inline LSValue** LSMap<double, LSValue*>::atL(const LSValue* key) {
	if (key->type == NUMBER) {
		auto r = this->atL_base(static_cast<const LSNumber*>(key)->value);
		LSValue::delete_temporary(key);
		return r;
	}
	LSValue::delete_temporary(key);
	throw vm::ExceptionObj(vm::Exception::NO_SUCH_OPERATOR);
	assert(false); // LCOV_EXCL_LINE
}

template <class K, class T>
inline T* LSMap<K, T>::atL_base(K key) const {
	// std::cout << "atL_base " << key << std::endl;
	auto map = (std::map<K, T, lsmap_less<K>>*) this;
	try {
		auto r = &map->at(key);
		ls::release(key);
		return r;
	} catch (std::exception&) {
		auto k = ls::move_inc(key);
		auto r = map->insert({k, ls::construct<T>()});
		return &r.first->second;
	}
}

template <class K, class V>
std::ostream& LSMap<K, V>::dump(std::ostream& os, int level) const {
	os << "[";
	if (this->empty()) os << ':';
	else {
		if (level > 0) {
			for (auto it = this->begin(); it != this->end(); ++it) {
				if (it != this->begin()) os << ", ";
				os << it->first << ": " << it->second;
			}
		} else {
			os << " ... ";
		}
	}
	return os << "]";
}

template <class K, class V>
std::string LSMap<K, V>::json() const {
	std::string res = "{";
	for (auto it = this->begin(); it != this->end(); ++it) {
		if (it != this->begin()) res += ",";
		res += "\"" + ls::print(it->first) + "\"";
		res += ": ";
		res += ls::to_json(it->second);
	}
	return res + "}";
}

template <class K, class V>
LSValue* LSMap<K, V>::clone() const {
	LSMap<K, V>* map = new LSMap<K, V>();
	for (auto it = this->begin(); it != this->end(); ++it) {
		map->emplace(ls::clone_inc(it->first), ls::clone_inc(it->second));
	}
	return map;
}

template <typename K, typename T>
inline LSValue* LSMap<K, T>::getClass(VM* vm) const {
	return vm->env.map_class.get();
}

}

#endif
