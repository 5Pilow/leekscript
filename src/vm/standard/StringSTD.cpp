#include <sstream>
#include <vector>
#include <math.h>
#include <string.h>
#include "../../util/utf8.h"
#include "StringSTD.hpp"
#include "ValueSTD.hpp"
#include "../value/LSNumber.hpp"
#include "../value/LSArray.hpp"
#include "../VM.hpp"
#include "../../type/Type.hpp"
#include "../../analyzer/semantic/Variable.hpp"

namespace ls {

LSValue* string_charAt(LSString* string, int index);
bool string_contains(LSString* haystack, LSString* needle);
bool string_endsWith(LSString* string, LSString* ending);
int string_indexOf(LSString* haystack, LSString* needle);
int string_length(LSString* string);
LSArray<LSValue*>* string_split(LSString* string, LSString* delimiter);
bool string_startsWith(const LSString* string, const LSString* starting);
LSValue* string_substring(LSString* string, int start, int length);
LSValue* string_toLower(LSString* string);
LSValue* string_toUpper(LSString* string);
LSValue* string_toArray(const LSString* string);
int string_begin_code(const LSString*);
LSValue* string_begin_code_ptr(const LSString*);
int string_code(const LSString*, int pos);
long string_number(const LSString*);

LSString* plus_any(LSString* s, LSValue* v) {
	return (LSString*) s->add(v);
}

LSString* plus_mpz(LSString* s, __mpz_struct* mpz) {
	char buff[1000];
	mpz_get_str(buff, 10, mpz);
	LSString* res = new LSString(*s + buff);
	LSValue::delete_temporary(s);
	return res;
}

LSString* plus_mpz_tmp(LSString* s, __mpz_struct* mpz) {
	char buff[1000];
	mpz_get_str(buff, 10, mpz);
	LSString* res = new LSString(*s + buff);
	LSValue::delete_temporary(s);
	mpz_clear(mpz);
	VM::current()->mpz_deleted++;
	return res;
}

void iterator_begin(LSString* str, LSString::iterator* it) {
	auto i = LSString::iterator_begin(str);
	it->buffer = i.buffer;
	it->index = 0;
	it->pos = 0;
	it->next_pos = 0;
	it->character = 0;
}
LSString* iterator_get(unsigned int c, LSString* previous) {
	if (previous != nullptr) {
		LSValue::delete_ref(previous);
	}
	char dest[5];
	u8_toutf8(dest, 5, &c, 1);
	auto s = new LSString(dest);
	s->refs = 1;
	return s;
}

StringSTD::StringSTD(VM* vm) : Module(vm, "String") {

	LSString::string_class = lsclass.get();

	/*
	 * Constructor
	 */
	constructor_({
		{Type::tmp_string, {}, (void*) &LSString::constructor_1},
		{Type::tmp_string, {Type::i8_ptr}, (void*) &LSString::constructor_2},
	});

	/*
	 * Operators
	 */
	operator_("+", {
		{Type::string, Type::any, Type::tmp_string, (void*) plus_any},
		{Type::string, Type::mpz_ptr, Type::tmp_string, (void*) plus_mpz},
		{Type::string, Type::tmp_mpz_ptr, Type::tmp_string, (void*) plus_mpz_tmp},
		{Type::string, Type::real, Type::tmp_string, (void*) add_real},
		{Type::string, Type::integer, Type::tmp_string, (void*) add_int},
		{Type::string, Type::boolean, Type::tmp_string, (void*) add_bool},
	});

	auto aeT = Type::template_("T");
	template_(aeT).
	operator_("+=", {
		{Type::string, aeT, Type::string, add_eq, 0, {}, true},
	});

	operator_("<", {
		{Type::string, Type::string, Type::boolean, lt}
	});
	operator_("/", {
		{Type::string, Type::string, Type::tmp_array(Type::string), div}
	});

	/*
	 * Methods
	 */
	method("copy", {
		{Type::string, {Type::const_string}, ValueSTD::copy}
	});
	method("charAt", {
		{Type::string, {Type::const_string, Type::const_integer}, (void*) string_charAt},
	});
	method("contains", {
		{Type::boolean, {Type::string, Type::const_string}, (void*) string_contains},
	});
	method("endsWith", {
		{Type::boolean, {Type::string, Type::string}, (void*) string_endsWith},
	});
	auto fold_fun_type = Type::fun(Type::any, {Type::any, Type::string});
	auto fold_clo_type = Type::closure(Type::any, {Type::any, Type::string});
	method("fold", {
		{Type::any, {Type::string, fold_fun_type, Type::any}, fold_fun},
		{Type::any, {Type::string, fold_clo_type, Type::any}, fold_fun},
	});
	method("indexOf", {
		{Type::integer, {Type::string, Type::string}, (void*) string_indexOf},
	});
	method("isPermutation", {
		{Type::boolean, {Type::string, Type::const_any}, (void*) &LSString::is_permutation},
	});
	method("isPalindrome", {
		{Type::boolean, {Type::string}, (void*) &LSString::is_palindrome},
	});
	method("left", {
		{Type::tmp_string, {Type::string, Type::integer}, (void*) string_left},
		{Type::tmp_string, {Type::tmp_string, Type::integer}, (void*) string_left_tmp},
	});
	method("length", {
		{Type::integer, {Type::string}, (void*) string_length},
	});
	method("lines", {
		{Type::tmp_array(Type::string), {Type::string}, (void*) &LSString::ls_lines},
	});
	method("size", {
		{Type::any, {Type::string}, (void*) &LSString::ls_size_ptr},
		{Type::integer, {Type::string}, (void*) &LSString::ls_size},
	});
	method("replace", {
		{Type::tmp_string, {Type::string, Type::string, Type::string}, (void*) replace},
		{Type::tmp_string, {Type::string, Type::string, Type::string}, (void*) v1_replace, LEGACY},
	});
	method("reverse", {
		{Type::tmp_string, {Type::string}, (void*) &LSString::ls_tilde},
	});
	method("right", {
		{Type::tmp_string, {Type::string, Type::integer}, (void*) string_right},
		{Type::tmp_string, {Type::tmp_string, Type::integer}, (void*) string_right_tmp},
	});
	method("substring", {
		{Type::tmp_string, {Type::string, Type::const_integer, Type::const_integer}, (void*) string_substring},
	});
	method("toArray", {
		{Type::tmp_array(Type::any), {Type::string}, (void*) string_toArray},
	});
	method("toLower", {
		{Type::tmp_string, {Type::string}, (void*) string_toLower},
	});
	method("toUpper", {
		{Type::tmp_string, {Type::string}, (void*) string_toUpper},
	});
	method("split", {
		{Type::tmp_array(Type::string), {Type::string, Type::string}, (void*) string_split},
		{Type::tmp_array(Type::string), {Type::const_any, Type::const_any}, (void*) string_split},
	});
	method("startsWith", {
		{Type::boolean, {Type::string, Type::string}, (void*) string_startsWith},
	});
	method("code", {
		{Type::any, {Type::const_any}, (void*) string_begin_code_ptr},
		{Type::integer, {Type::string}, (void*) string_begin_code},
		{Type::integer, {Type::const_any}, (void*) string_begin_code},
		{Type::integer, {Type::string, Type::const_integer}, (void*) string_code},
	});
	method("number", {
		{Type::long_, {Type::string}, (void*) string_number},
		{Type::long_, {Type::const_any}, (void*) string_number},
	});
	auto map_fun = &LSString::ls_map<LSFunction*>;
	method("map", {
		{Type::tmp_string, {Type::string, Type::fun_object(Type::string, {Type::string})}, (void*) map_fun},
	});
	method("sort", {
		{Type::tmp_string, {Type::string}, (void*) &LSString::sort},
	});
	method("wordCount", {
		{Type::any, {Type::string}, (void*) &LSString::word_count_ptr},
		{Type::integer, {Type::string}, (void*) &LSString::word_count},
	});

	/** Internal **/
	method("to_bool", {
		{Type::boolean, {Type::string}, (void*) &LSString::to_bool}
	});
	method("codePointAt", {
		{Type::tmp_string, {Type::string, Type::integer}, (void*) &LSString::codePointAt}
	});
	method("isize", {
		{Type::integer, {Type::string}, (void*) &LSString::int_size}
	});
	method("iterator_begin", {
		{Type::void_, {Type::string, Type::i8_ptr}, (void*) iterator_begin}
	});
	method("iterator_end", {
		{Type::void_, {Type::i8_ptr}, (void*) &LSString::iterator_end}
	});
	method("iterator_get", {
		{Type::integer, {Type::i8_ptr}, (void*) &LSString::iterator_get},
		{Type::tmp_string, {Type::integer, Type::string}, (void*) iterator_get},
	});
	method("iterator_key", {
		{Type::integer, {Type::i8_ptr}, (void*) &LSString::iterator_key}
	});
	method("iterator_next", {
		{Type::void_, {Type::i8_ptr}, (void*) &LSString::iterator_next}
	});
}

StringSTD::~StringSTD() {}

/*
 * Operators
 */
LSString* StringSTD::add_int(LSString* s, int i) {
	if (s->refs == 0) {
		s->append(std::to_string(i));
		return s;
	} else {
		return new LSString(*s + std::to_string(i));
	}
}
LSString* StringSTD::add_int_r(int i, LSString* s) {
	if (s->refs == 0) {
		s->insert(0, std::to_string(i));
		return s;
	} else {
		return new LSString(std::to_string(i) + *s);
	}
}

LSString* StringSTD::add_bool(LSString* s, bool b) {
	if (s->refs == 0) {
		s->append(b ? "true" : "false");
		return s;
	} else {
		return new LSString(*s + (b ? "true" : "false"));
	}
}

LSString* StringSTD::add_real(LSString* s, double i) {
	if (s->refs == 0) {
		s->append(LSNumber::print(i));
		return s;
	} else {
		return new LSString(*s + LSNumber::print(i));
	}
}

Compiler::value StringSTD::add_eq(Compiler& c, std::vector<Compiler::value> args, int) {
	args[1] = c.insn_to_any(args[1]);
	return c.insn_call(Type::any, args, "Value.operator+=");
}

Compiler::value StringSTD::lt(Compiler& c, std::vector<Compiler::value> args, int) {
	auto res = c.insn_call(Type::boolean, args, "Value.operator<");
	c.insn_delete_temporary(args[0]);
	c.insn_delete_temporary(args[1]);
	return res;
}

Compiler::value StringSTD::div(Compiler& c, std::vector<Compiler::value> args, int) {
	return c.insn_call(Type::tmp_array(Type::string), args, "Value.operator/");
}

/*
 * Methods
 */
LSValue* string_charAt(LSString* string, int index) {
	LSValue* r = new LSString(string->operator[] (index));
	LSValue::delete_temporary(string);
	return r;
}

bool string_contains(LSString* haystack, LSString* needle) {
	bool r = haystack->find(*needle) != std::string::npos;
	LSValue::delete_temporary(haystack);
	LSValue::delete_temporary(needle);
	return r;
}

bool string_endsWith(LSString* string, LSString* ending) {
	if (ending->size() > string->size()) {
		LSValue::delete_temporary(string);
		LSValue::delete_temporary(ending);
		return false;
	}
	bool r = std::equal(ending->rbegin(), ending->rend(), string->rbegin());
	LSValue::delete_temporary(string);
	LSValue::delete_temporary(ending);
	return r;
}

int string_indexOf(LSString* string, LSString* needle) {
	int pos = -1;
	if (needle->size() <= string->size()) {
		pos = string->find(*needle);
	}
	LSValue::delete_temporary(string);
	LSValue::delete_temporary(needle);
	return pos;
}

int string_length(LSString* string) {
	int r = string->size();
	LSValue::delete_temporary(string);
	return r;
}

LSString* StringSTD::replace(LSString* string, LSString* from, LSString* to) {
	std::string str(*string);
	size_t start_pos = 0;
	while ((start_pos = str.find(*from, start_pos)) != std::string::npos) {
		str.replace(start_pos, from->length(), *to);
		start_pos += to->length();
	}
	if (string->refs == 0) { delete string; }
	if (from->refs == 0) { delete from; }
	if (to->refs == 0) { delete to; }
	return new LSString(str);
}

LSValue* StringSTD::v1_replace(LSString* string, LSString* from, LSString* to) {
	std::string str(*string);
	size_t start_pos = 0;
	// Replace \\ by \ (like Java does)
	std::string f = *from;
	while ((start_pos = f.find("\\\\", start_pos)) != std::string::npos) {
		f.replace(start_pos, 2, "\\");
		start_pos += 1;
	}
	start_pos = 0;
	std::string t = *to;
	while ((start_pos = t.find("\\\\", start_pos)) != std::string::npos) {
		t.replace(start_pos, 2, "\\");
		start_pos += 1;
	}
	start_pos = 0;
	while ((start_pos = str.find(f, start_pos)) != std::string::npos) {
		str.replace(start_pos, from->length(), t);
		start_pos += t.size();
	}
	if (string->refs == 0) { delete string; }
	if (from->refs == 0) { delete from; }
	if (to->refs == 0) { delete to; }
	return new LSString(str);
}

LSArray<LSValue*>* string_split(LSString* string, LSString* delimiter) {
	auto parts = new LSArray<LSValue*>();
	if (*delimiter == "") {
		for (char c : *string) {
			parts->push_inc(new LSString(c));
		}
		if (string->refs == 0) {
			delete string;
		}
		if (delimiter->refs == 0) {
			delete delimiter;
		}
		return parts;
	} else {
		size_t last = 0;
		size_t pos = 0;
		while ((pos = string->find(*delimiter, last)) != std::wstring::npos) {
			parts->push_inc(new LSString(string->substr(last, pos - last)));
			last = pos + delimiter->size();
		}
		parts->push_inc(new LSString(string->substr(last)));
		if (string->refs == 0) {
			delete string;
		}
		if (delimiter->refs == 0) {
			delete delimiter;
		}
		return parts;
	}
}

bool string_startsWith(const LSString* string, const LSString* starting) {
	if (starting->size() > string->size()) {
		LSValue::delete_temporary(string);
		LSValue::delete_temporary(starting);
		return false;
	}
	bool r = std::equal(starting->begin(), starting->end(), string->begin());
	LSValue::delete_temporary(string);
	LSValue::delete_temporary(starting);
	return r;
}

LSValue* string_substring(LSString* string, int start, int length) {
	LSValue* r = new LSString(string->substr(start, length));
	if (string->refs == 0) {
		delete string;
	}
	return r;
}

LSValue* string_toArray(const LSString* string) {
	LSArray<LSValue*>* parts = new LSArray<LSValue*>();
	for (char c : *string) {
		parts->push_inc(new LSString(c));
	}
	if (string->refs == 0) {
		delete string;
	}
	return parts;
}

LSValue* string_toLower(LSString* s) {
	std::string new_s = std::string(*s);
	for (auto& c : new_s) c = tolower(c);
	if (s->refs == 0) {
		delete s;
	}
	return new LSString(new_s);
}

LSValue* string_toUpper(LSString* s) {
	std::string new_s = std::string(*s);
	for (auto& c : new_s) c = toupper(c);
	if (s->refs == 0) {
		delete s;
	}
	return new LSString(new_s);
}

int string_begin_code(const LSString* v) {
	int r = LSString::u8_char_at((char*) v->c_str(), 0);
	LSValue::delete_temporary(v);
	return r;
}

LSValue* string_begin_code_ptr(const LSString* v) {
	auto code = string_begin_code(v);
	auto r = LSNumber::get(code);
	return r;
}

int string_code(const LSString* v, int pos) {
	int r = LSString::u8_char_at((char*) v->c_str(), pos);
	LSValue::delete_temporary(v);
	return r;
}

long string_number(const LSString* s) {
	long r = stol(*s);
	if (s->refs == 0) delete s;
	return r;
}

Compiler::value StringSTD::fold_fun(Compiler& c, std::vector<Compiler::value> args, int) {
	auto function = args[1];
	auto result = Variable::new_temporary("r", args[2].t);
	result->create_entry(c);
	c.add_temporary_variable(result);
	c.insn_store(result->val, c.insn_move_inc(args[2]));
	auto v = Variable::new_temporary("v", args[0].t->element());
	v->create_entry(c);
	c.add_temporary_variable(v);
	c.insn_foreach(args[0], Type::void_, v, nullptr, [&](Compiler::value v, Compiler::value k) -> Compiler::value {
		auto r = c.insn_call(function, {c.insn_load(result->val), v});
		c.insn_delete(c.insn_load(result->val));
		c.insn_store(result->val, c.insn_move_inc(r));
		return {};
	});
	return c.insn_load(result->val);
}

LSValue* StringSTD::string_right(LSString* string, int pos) {
	auto r = new LSString(string->substr(string->size() - std::min(string->size(), (size_t) std::max(0, pos))));
	LSValue::delete_temporary(string);
	return r;
}
LSValue* StringSTD::string_right_tmp(LSString* string, int pos) {
	return &string->operator = (string->substr(string->size() - std::min(string->size(), (size_t) std::max(0, pos))));
}

LSValue* StringSTD::string_left(LSString* string, int pos) {
	auto r = new LSString(string->substr(0, std::max(0, pos)));
	LSValue::delete_temporary(string);
	return r;
}
LSValue* StringSTD::string_left_tmp(LSString* string, int pos) {
	return &string->operator = (string->substr(0, std::max(0, pos)));
}

}
