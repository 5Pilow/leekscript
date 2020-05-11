/*
 * Describe an array in LeekScript
 */
#ifndef LS_ARRAY_BASE
#define LS_ARRAY_BASE

#include <vector>
#include <iterator>
#include "../LSValue.hpp"

namespace ls {

template <typename T>
class LSArray : public LSValue, public std::vector<T> {
public:

	static LSArray<T>* constructor(int);

	LSArray();
	LSArray(std::initializer_list<T>);
	LSArray(const std::vector<T>&);
	LSArray(const LSArray<T>&);
	LSArray(size_t);

	virtual ~LSArray();

	/*
	 * Array functions
	 */
	void push_clone(T value); // clone increment and push
	void push_move(T value); // clone (if not temporary) increment and push
	void push_inc(T value); // increment (if not native) and push
	static void std_push_inc(LSArray<T>* array, T value);
	static int int_size(LSArray<T>* array);
	static LSArray<LSValue*>* ls_clear(LSArray<T>* array);
	static T ls_remove(LSArray<T>* array, int index);
	static bool ls_remove_element(LSArray<T>* array, T element);
	static bool ls_empty(LSArray<T>* array);
	static T ls_pop(LSArray<T>* array);
	static int ls_size(LSArray<T>* array);
	static LSValue* ls_size_ptr(LSArray<T>* array);
	static T ls_sum(LSArray<T>* array);
	static T ls_product(LSArray<T>* array);
	static double ls_average(LSArray<T>* array);
	static T ls_first(LSArray<T>* array);
	static T ls_last(LSArray<T>* array);
	template <class F, class R>
	static LSArray<R>* ls_map(LSArray<T>* array, F function);
	static LSArray<LSValue*>* ls_chunk(LSArray<T>* array, int size = 1);
	static LSArray<T>* ls_unique(LSArray<T>* array);
	static LSArray<T>* ls_sort(LSArray<T>* array);
	template <class F>
	static LSArray<T>* ls_sort_fun(LSArray<T>* array, F function);
	template <class F>
	static void ls_iter(LSArray<T>* array, F fun);
	static bool ls_contains(LSArray<T>* array, T val);
	static LSArray<T>* ls_push(LSArray<T>* array, T val);
	static LSArray<T>* ls_push_all_ptr(LSArray<T>* array1, LSArray<LSValue*>* array);
	static LSArray<T>* ls_push_all_int(LSArray<T>* array1, LSArray<int>* array);
	static LSArray<T>* ls_push_all_flo(LSArray<T>* array1, LSArray<double>* array);
	static LSArray<T>* ls_shuffle(LSArray<T>* array);
	static LSArray<T>* ls_reverse(LSArray<T>* array);
	template <class F>
	LSArray<T>* ls_filter(LSArray<T>* array, F fun);
	template <class F, class R>
	static R ls_foldLeft(LSArray<T>* array, F function, R initial);
	template <class F, class R>
	static R ls_foldRight(LSArray<T>* array, F function, R initial);
	static LSArray<T>* ls_insert(LSArray<T>* array, T value, int pos);
	template <class F, class R, class T2>
	static LSArray<R>* ls_map2(LSArray<T>* array, LSArray<T2>*, F function);
	static int ls_search(LSArray<T>* array, T needle, int start);
	static LSString* ls_join(LSArray<T>* array);
	static LSString* ls_join_glue(LSArray<T>* array, LSString* glue);
	static LSArray<T>* ls_fill(LSArray<T>* array, T element, int size);
	static T ls_max(LSArray<T>* array);
	static T ls_min(LSArray<T>* array);
	template <class T2>
	static bool is_permutation(LSArray<T>* array, LSArray<T2>* other);
	template <class T2>
	LSValue* add_set(LSSet<T2>* set);
	static LSArray<T>* ls_random(LSArray<T>* array, int n);
	static LSArray<LSValue*>* to_any_array(LSArray<T>* array);
	static LSArray<double>* to_real_array(LSArray<T>* array);
	static LSArray<long>* to_long_array(LSArray<T>* array);
	static bool next_permutation(LSArray<T>* array);
	static LSArray<T>* repeat(LSArray<T>* array, int n);
	static bool std_in(LSArray<T>* array, const LSValue* const);
	static bool std_in_i(LSArray<T>* array, const int);
	static bool std_to_bool(LSArray<T>* array);

	/*
	 * LSValue methods
	 */
	bool to_bool() const override;
	bool ls_not() const override;
	LSValue* ls_tilde() override;

	LSValue* add(LSValue* v) override;
	LSValue* add_eq(LSValue* v) override;
	LSValue* add_eq_double(double v);
	LSValue* add_eq_int(int v);
	bool eq(const LSValue*) const override;
	bool lt(const LSValue*) const override;

	virtual bool in(const LSValue* const) const override;
	virtual bool in_i(const int) const override;

	LSValue* at(const LSValue* value) const override;
	int at_i_i(const int key) const override;
	LSValue** atL(const LSValue* value) override;

	LSValue* range(int start, int end) const override;

	int abso() const override;

	std::ostream& dump(std::ostream& os, int level) const override;
	std::string json() const override;
	LSValue* clone() const override;
	LSValue* getClass(VM* vm) const override;
};

}

#ifndef _GLIBCXX_EXPORT_TEMPLATE
#include "LSArray.tcc"
#endif

#endif
