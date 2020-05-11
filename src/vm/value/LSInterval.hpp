#ifndef VM_VALUE_LSINTERVAL_HPP_
#define VM_VALUE_LSINTERVAL_HPP_

#include "../LSValue.hpp"

namespace ls {

class LSFunction;

class LSInterval : public LSValue {
public:

	static LSInterval* constructor(int a, int b);

	int a = 0;
	int b = 0;

	LSInterval();
	virtual ~LSInterval();

	/*
	 * Array methods
	 */
	template <class F>
	static LSArray<int>* std_filter(LSInterval* interval, F function);
	static long std_sum(LSInterval* interval);
	static long std_product(LSInterval* interval);
	static int std_at_i_i(LSInterval* interval, const int key);
	static bool std_in_i(LSInterval* interval, const int);

	/*
	 * LSValue methods
	 */
	virtual bool to_bool() const override;
	virtual bool ls_not() const override;
	virtual bool eq(const LSValue*) const override;
	virtual bool in(const LSValue* const) const override;
	virtual bool in_i(const int) const override;

	virtual LSValue* at(const LSValue* key) const override;
	int at_i_i(const int key) const override;
	LSValue* range(int start, int end) const override;
	virtual int abso() const override;

	virtual LSValue* clone() const override;
	virtual std::ostream& dump(std::ostream& os, int level) const override;
	LSValue* getClass(VM* vm) const override;
};

}

#endif
