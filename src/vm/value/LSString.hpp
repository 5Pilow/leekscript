#ifndef LSSTRING_H_
#define LSSTRING_H_

#include <iostream>
#include <string>
#include "../LSValue.hpp"

namespace ls {

class LSString : public LSValue, public std::string {
public:

	struct iterator {
		char* buffer;
		int index;
		int pos;
		int next_pos;
		u_int32_t character;
	};

	static u_int32_t u8_char_at(char* s, int pos);
	static iterator iterator_begin(LSString* s);
	static void iterator_next(iterator* it);
	static u_int32_t iterator_get(iterator* it);
	static int iterator_key(LSString::iterator* it);
	static bool iterator_end(iterator* it);
	static LSString* constructor_1();
	static LSString* constructor_2(char* s);

	LSString();
	LSString(char);
	LSString(const char*);
	LSString(const std::string&);
	LSString(const Json&);

	virtual ~LSString();

	static LSString* charAt(const LSString* const string, int index);
	static LSString* codePointAt(const LSString* const string, int index);
	int unicode_length() const;
	static bool is_permutation(const LSString* const string, LSString* other);
	static LSString* sort(LSString* string);
	static bool is_palindrome(const LSString* const string);
	template <class F> LSValue* ls_foldLeft(F, LSValue* v0);
	static int int_size(const LSString* const string);
	static int std_size(const LSString* const string);
	static LSValue* std_size_ptr(const LSString* const string);
	static int word_count(const LSString* const string);
	static LSValue* word_count_ptr(const LSString* const string);
	static LSArray<LSValue*>* std_lines(const LSString* const string);
	template <class F>
	static LSString* std_map(const LSString* const string, F function);
	static LSValue* std_tilde(LSString* string);
	static bool std_to_bool(const LSString* const string);

	/*
	 * LSValue methods
	 */
	bool to_bool() const override;
	bool ls_not() const override;
	LSValue* ls_tilde() override;

	virtual LSValue* add(LSValue* v) override;
	virtual LSValue* add_eq(LSValue* v) override;
	virtual LSValue* mul(LSValue* v) override;
	virtual LSValue* div(LSValue* v) override;
	bool eq(const LSValue*) const override;
	bool lt(const LSValue*) const override;

	LSValue* at(const LSValue* value) const override;

	LSValue* range(int start, int end) const override;

	int abso() const override;

	LSValue* clone() const override;

	std::ostream& print(std::ostream& os) const override;
	std::ostream& dump(std::ostream& os, int level) const override;
	std::string json() const override;
	std::string escaped(char quote) const;
	std::string escape_control_characters() const;

	LSValue* getClass(VM* vm) const override;
};

}

#endif
