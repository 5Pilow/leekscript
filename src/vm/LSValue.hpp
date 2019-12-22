#ifndef LSVALUE_H_
#define LSVALUE_H_

#include <iostream>
#include <string>
#include <cstddef>
#include <cstdint>
#include <map>
#include "../util/json.hpp"
#include "../constants.h"

namespace ls {

class LSNull;
class LSBoolean;
class LSNumber;
class LSString;
template <typename T> class LSArray;
template <typename K, typename T> class LSMap;
template <typename T> class LSSet;
class LSFunction;
class LSObject;
class LSClass;
class Context;
class Type;
class VM;

typedef u_int8_t LSValueType;

class LSValue {
public:

	static LSValueType NULLL;
	static LSValueType BOOLEAN;
	static LSValueType NUMBER;
	static LSValueType STRING;
	static LSValueType ARRAY;
	static LSValueType MAP;
	static LSValueType SET;
	static LSValueType INTERVAL;
	static LSValueType FUNCTION;
	static LSValueType CLOSURE;
	static LSValueType OBJECT;
	static LSValueType CLASS;
	static LSValueType MPZ;
	static LSValueType LEGACY_ARRAY;

	static int obj_count;
	static int obj_deleted;
	#if DEBUG_LEAKS
		static std::unordered_map<void*, LSValue*>& objs() {
			static std::unordered_map<void*, LSValue*> objs;
			return objs;
		}
	#endif

	LSValueType type;
	int refs = 0;
	bool native = false;

	LSValue(LSValueType type, int refs = 0, bool native = false);
	LSValue(const LSValue& other);
	virtual ~LSValue() = 0;

	virtual bool to_bool() const = 0;
	virtual bool ls_not() const = 0;
	virtual LSValue* ls_minus();
	virtual LSValue* ls_tilde();
	virtual LSValue* ls_preinc(); // ++x
	virtual LSValue* ls_inc(); // x++
	virtual LSValue* ls_predec();
	virtual LSValue* ls_dec();

	virtual LSValue* add(LSValue*);
	virtual LSValue* add_eq(LSValue*);
	virtual LSValue* sub(LSValue*);
	virtual LSValue* sub_eq(LSValue*);
	virtual LSValue* mul(LSValue*);
	virtual LSValue* mul_eq(LSValue*);
	virtual LSValue* div(LSValue*);
	virtual LSValue* div_eq(LSValue*);
	virtual LSValue* int_div(LSValue*);
	virtual LSValue* int_div_eq(LSValue*);
	virtual LSValue* pow(LSValue*);
	virtual LSValue* pow_eq(LSValue*);
	virtual LSValue* mod(LSValue*);
	virtual LSValue* mod_eq(LSValue*);
	virtual LSValue* double_mod(LSValue*);
	virtual LSValue* double_mod_eq(LSValue*);

	bool operator == (const LSValue& value) const {
		return this->eq(&value);
	}
	virtual bool operator == (int) const { return false; }
	virtual bool operator == (long) const { return false; }
	virtual bool operator == (double) const { return false; }
	virtual bool operator < (int) const { return false; }
	virtual bool operator < (double) const { return false; }

	bool operator != (const LSValue& value) const {
		return !this->eq(&value);
	}
	virtual bool eq(const LSValue*) const = 0;

	bool operator < (const LSValue& value) const {
		return this->lt(&value);
	}
	bool operator > (const LSValue& value) const {
		return !(*this == value) && !(*this < value);
	}
	bool operator <= (const LSValue& value) const {
		return (*this == value) || (*this < value);
	}
	bool operator >= (const LSValue& value) const {
		return !(*this < value);
	}
	virtual bool lt(const LSValue*) const;

	virtual bool in(const LSValue* const) const;
	virtual bool in_i(const int) const;

	virtual LSValue* at(const LSValue* key) const;
	virtual int at_i_i(const int key) const;
	virtual LSValue** atL(const LSValue* key);

	virtual LSValue* attr(VM* vm, const std::string& key) const;
	virtual LSValue** attrL(const std::string& key);

	virtual LSValue* range(int start, int end) const;
	virtual LSValue* rangeL(int start, int end);

	virtual int abso() const;

	virtual std::string to_string() const;
	virtual std::ostream& print(std::ostream&) const;
	virtual std::ostream& dump(std::ostream&, int level) const = 0;
	virtual std::string json() const;

	LSString* ls_json();

	virtual LSValue* clone() const;
	LSValue* clone_inc();
	LSValue* move();
	LSValue* move_inc();

	virtual LSValue* getClass(VM* vm) const = 0;

	static LSValue* get();
	template <class T> static LSValue* get(T v);
	static LSValue* parse(Json& json);
	static LSValue* get_from_json(Json& json);

	static void free(const LSValue*);
	static void delete_ref(LSValue* value);
	static void delete_ref2(LSValue* value);
	static void delete_temporary(const LSValue* const value);
	static void delete_not_temporary(LSValue* value);
};

template <> LSValue* LSValue::get(char v);
template <> LSValue* LSValue::get(int v);
template <> LSValue* LSValue::get(long v);
template <> LSValue* LSValue::get(double v);

inline LSValue* LSValue::clone_inc() {
	if (native) {
		return this;
	} else {
		LSValue* copy = clone();
		copy->refs++;
		return copy;
	}
}

inline LSValue* LSValue::move() {
	if (native) {
		return this;
	}
	if (refs == 0) {
		return this;
	}
	return clone();
}

inline LSValue* LSValue::move_inc() {
	if (native) {
		return this;
	}
	if (refs == 0) {
		refs++;
		return this;
	}
	auto v = clone();
	v->refs++;
	return v;
}

inline void LSValue::delete_ref(LSValue* value) {
	if (value->native) return;
	if (value->refs == 0 || --value->refs == 0) {
		delete value;
	}
}
inline void LSValue::delete_ref2(LSValue* value) {
	if (value->native) return;
	if (value->refs == 0) return;
	if (--value->refs == 0) {
		delete value;
	}
}

inline void LSValue::delete_temporary(const LSValue* const value) {
	if (value->refs == 0) {
		delete value;
	}
}

inline void LSValue::free(const LSValue* value) {
	if (value->native) return;
	delete value;
}

inline void LSValue::delete_not_temporary(LSValue* value) {
	if (value->native) return;
	if (value->refs == 0) return;
	value->refs--;
	if (value->refs == 0) {
		delete value;
	}
}

inline std::ostream& operator << (std::ostream& os, const LSValue& value) {
	return value.print(os);
}

}

namespace ls {
	template <typename T>
	std::string to_json(T v) {
		return std::to_string(v);
	}
	template <>
	inline std::string to_json(LSValue* v) {
		return v->json();
	}

	template <typename T>
	std::string print(T v) {
		return std::to_string(v);
	}
	template <>
	inline std::string print(LSValue* v) {
		std::ostringstream oss;
		oss << *v;
		return oss.str();
	}

	template <class T1, class T2>
	bool equals(const T1 v1, const T2 v2) {
		return v1 == v2;
	}
	template <>
	inline bool equals(double v1, ls::LSValue* v2) {
		return v2->operator == (v1);
	}
	template <>
	inline bool equals(ls::LSValue* v1, double v2) {
		return v1->operator == (v2);
	}
	template <>
	inline bool equals(int v1, ls::LSValue* v2) {
		return v2->operator == (v1);
	}
	template <>
	inline bool equals(char v1, ls::LSValue* v2) {
		return v2->operator == (v1);
	}
	template <>
	inline bool equals(ls::LSValue* v1, int v2) {
		return v1->operator == (v2);
	}
	template <>
	inline bool equals(ls::LSValue* v1, char v2) {
		return v1->operator == (v2);
	}
	template <>
	inline bool equals(ls::LSValue* v1, long v2) {
		return v1->operator == (v2);
	}
	template <>
	inline bool equals(ls::LSValue* v1, ls::LSValue* v2) {
		return v1->operator == (*v2);
	}

	template <class T1, class T2>
	bool lt(const T1 v1, const T2 v2) {
		return v1 < v2;
	}
	template <>
	inline bool lt(double v1, ls::LSValue* v2) {
		return !(v2->operator == (v1)) and !(v2->operator < (v1));
	}
	template <>
	inline bool lt(ls::LSValue* v1, double v2) {
		return v1->operator < (v2);
	}
	template <>
	inline bool lt(int v1, ls::LSValue* v2) {
		return !(v2->operator == (v1)) and !(v2->operator < (v1));
	}
	template <>
	inline bool lt(ls::LSValue* v1, int v2) {
		return v1->operator < (v2);
	}
	template <>
	inline bool lt(ls::LSValue* v1, ls::LSValue* v2) {
		return v1->operator < (*v2);
	}


	template <class T> void release(T) {}
	template <> inline void release(LSValue* v) {
		LSValue::delete_temporary(v);
	}
	template <> inline void release(const LSValue* v) {
		LSValue::delete_temporary(v);
	}

	template <class T> void unref(T) {}
	template <> inline void unref(LSValue* v) {
		LSValue::delete_ref(v);
	}
	template <> inline void unref(const LSValue* v) {
		LSValue::delete_ref((LSValue*) v);
	}

	template <class T> T oneref(T v) { return v; }
	template <> inline LSValue* oneref(LSValue* v) {
		v->refs = 1;
		return v;
	}
	template <> inline const LSValue* oneref(const LSValue* v) {
		((LSValue*) v)->refs = 1;
		return v;
	}

	template <class T> void increfs(T) {}
	template <> inline void increfs(LSValue* v) {
		v->refs++;
	}
	template <> inline void increfs(const LSValue* v) {
		((LSValue*) v)->refs++;
	}

	template <class T> T move(T v) { return v; }
	template <> inline LSValue* move(LSValue* v) {
		return v->move();
	}
	template <> inline const LSValue* move(const LSValue* v) {
		return ((LSValue*) v)->move();
	}

	template <class T> T move_inc(T v) { return v; }
	template <> inline LSValue* move_inc(LSValue* v) {
		return v->move_inc();
	}
	template <> inline const LSValue* move_inc(const LSValue* v) {
		return ((LSValue*) v)->move_inc();
	}

	template <class T> T clone(T v) { return v; }
	template <> inline LSValue* clone(LSValue* v) {
		return v->clone();
	}
	template <> inline const LSValue* clone(const LSValue* v) {
		return ((LSValue*) v)->clone();
	}

	template <class T> T clone_inc(T v) { return v; }
	template <> inline LSValue* clone_inc(LSValue* v) {
		return v->clone_inc();
	}
	template <> inline const LSValue* clone_inc(const LSValue* v) {
		return ((LSValue*) v)->clone_inc();
	}

	template <class R, class T> R convert(T v);
	template <class R, R> R convert(R v) { return v; }

	template <> inline int convert(int v) { return v; }
	template <> inline long convert(long v) { return v; }
	template <> inline double convert(double v) { return v; }
	template <> inline LSValue* convert(LSValue* v) { return v; }

	template <> inline int convert(double v) { return v; }
	template <> inline int convert(long v) { return v; }
	template <> inline long convert(int v) { return v; }
	template <> inline long convert(double v) { return v; }
	template <> inline double convert(int v) { return v; }
	template <> inline double convert(long v) { return v; }

	template <> inline LSValue* convert(char v) {
		return LSValue::get(v);
	}
	template <> inline LSValue* convert(long v) {
		return LSValue::get(v);
	}
	template <> inline LSValue* convert(int v) {
		return LSValue::get(v);
	}
	template <> inline LSValue* convert(double v) {
		return LSValue::get(v);
	}

	template <class T> T construct() { return {}; }
	template <> inline LSValue* construct() {
		return LSValue::get();
	}
	template <> inline const LSValue* construct() {
		return LSValue::get();
	}
}

namespace std {
	std::ostream& operator << (std::ostream&, const ls::LSValue*);
}

#endif
