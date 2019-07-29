#ifndef LS_SET_BASE
#define LS_SET_BASE

#include "../LSValue.hpp"
#include <set>

namespace ls {

template <typename K>
struct lsset_less {
	bool operator() (K lhs, K rhs) const;
};

template <typename T>
class LSSet : public LSValue, public std::set<T, lsset_less<T>> {
public:
	static LSSet<T>* constructor();

	LSSet();
	LSSet(std::initializer_list<T> values);
	LSSet(const LSSet<T>& other);
	virtual ~LSSet();

	/*
	 * LSSet methods
	 */
	int ls_size();
	bool ls_insert(T value);
	LSValue* ls_insert_ptr(T value);
	void vinsert(T value);
	LSSet<T>* ls_clear();
	bool ls_erase(T value);
	bool ls_contains(T value);
	template <class T2>
	bool set_lt(const LSSet<T2>* set) const;
	LSSet<LSValue*>* to_any_set() const;

	/*
	 * LSValue methods
	 */
	virtual bool to_bool() const override;
	virtual bool ls_not() const override;
	virtual LSValue* add_eq(LSValue* v) override;
	LSValue* add_eq_int(int v);
	LSValue* add_eq_double(double v);
	bool eq(const LSValue*) const override;
	bool lt(const LSValue*) const override;
	virtual bool in(const LSValue* const) const override;
	bool in_v(const T) const;
	int abso() const override;
	virtual std::ostream& dump(std::ostream&, int level) const override;
	virtual std::string json() const override;
	virtual LSValue* clone() const override;
	virtual LSValue* getClass(VM* vm) const override;
};

}

#ifndef _GLIBCXX_EXPORT_TEMPLATE
#include "LSSet.tcc"
#endif

#endif
