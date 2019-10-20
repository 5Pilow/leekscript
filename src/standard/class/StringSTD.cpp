#include <sstream>
#include <vector>
#include <math.h>
#include <string.h>
#include "../../util/utf8.h"
#include "StringSTD.hpp"
#include "ValueSTD.hpp"
#include "../../type/Type.hpp"
#include "../../analyzer/semantic/Variable.hpp"
#include "../../environment/Environment.hpp"
#if COMPILER
#include "../../vm/value/LSNumber.hpp"
#include "../../vm/value/LSArray.hpp"
#include "../../vm/value/LSMap.hpp"
#include "../../vm/VM.hpp"
#endif

namespace ls {

#if COMPILER
LSValue* string_charAt(LSString* string, int index);
bool string_contains(LSString* haystack, LSString* needle);
bool string_endsWith(LSString* string, LSString* ending);
int string_indexOf(LSString* haystack, LSString* needle);
int string_length(LSString* string);
LSArray<LSValue*>* string_split(LSString* string, LSString* delimiter);
bool string_startsWith(const LSString* string, const LSString* starting);
LSValue* string_substring_1(LSString* string, int start);
LSValue* string_substring_2(LSString* string, int start, int length);
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

LSString* internal_plus_mpz_tmp(VM* vm, LSString* s, __mpz_struct* mpz) {
	char buff[1000];
	mpz_get_str(buff, 10, mpz);
	LSString* res = new LSString(*s + buff);
	LSValue::delete_temporary(s);
	mpz_clear(mpz);
	vm->mpz_deleted++;
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
	// if (previous != nullptr) {
		// LSValue::delete_ref(previous);
	// }
	char dest[5];
	u8_toutf8(dest, 5, &c, 1);
	auto s = new LSString(dest);
	s->refs = 1;
	return s;
}
#endif

StringSTD::StringSTD(Environment& env) : Module(env, "String") {

	#if COMPILER
	env.string_class = std::make_unique<LSClass>(clazz.get());
	lsclass = env.string_class.get();
	#endif

	/*
	 * Constructor
	 */
	constructor_({
		{env.tmp_string, {}, ADDR((void*) &LSString::constructor_1)},
		{env.tmp_string, {env.i8_ptr}, ADDR((void*) &LSString::constructor_2)},
		{env.tmp_string, {env.string}, ADDR((void*) &LSString::constructor_1)},
	});

	/*
	 * Operators
	 */
	operator_("+", {
		{env.string, env.any, env.tmp_string, ADDR((void*) plus_any)},
		{env.string, env.mpz_ptr, env.tmp_string, ADDR((void*) plus_mpz)},
		{env.string, env.tmp_mpz_ptr, env.tmp_string, ADDR(plus_mpz_tmp)},
		{env.string, env.real, env.tmp_string, ADDR((void*) add_real)},
		{env.string, env.long_, env.tmp_string, ADDR((void*) add_long)},
		{env.string, env.integer, env.tmp_string, ADDR((void*) add_int)},
		{env.string, env.boolean, env.tmp_string, ADDR((void*) add_bool)},
	});

	auto aeT = env.template_("T");
	template_(aeT).
	operator_("+=", {
		{env.string, aeT, env.string, ADDR(add_eq), 0, {}, true},
	});

	operator_("<", {
		{env.string, env.string, env.boolean, ADDR(lt)}
	});
	operator_("/", {
		{env.string, env.string, Type::tmp_array(env.string), ADDR(div)}
	});

	/*
	 * Methods
	 */
	method("copy", {
		{env.string, {env.const_string}, ADDR(ValueSTD::copy)}
	});
	method("charAt", {
		{env.string, {env.const_string, env.const_integer}, ADDR((void*) string_charAt)},
	});
	method("chunk", {
		{Type::array(env.string), {env.const_string, env.const_integer}, ADDR((void*) chunk)}
	});
	method("contains", {
		{env.boolean, {env.string, env.const_string}, ADDR((void*) string_contains)},
	});
	method("count", {
		{env.integer, {env.const_string, env.const_string}, ADDR((void*) count)}
	});
	method("endsWith", {
		{env.boolean, {env.string, env.string}, ADDR((void*) string_endsWith)},
	});
	auto fold_fun_type = Type::fun(env.any, {env.any, env.string});
	auto fold_clo_type = Type::closure(env.any, {env.any, env.string});
	method("fold", {
		{env.any, {env.string, fold_fun_type, env.any}, ADDR(fold_fun)},
		{env.any, {env.string, fold_clo_type, env.any}, ADDR(fold_fun)},
	});
	method("frequencies", {
		{Type::map(env.integer, env.integer), {env.const_string}, ADDR((void*) frequencies)}
	});
	method("indexOf", {
		{env.integer, {env.string, env.string}, ADDR((void*) string_indexOf)},
	});
	method("isPermutation", {
		{env.boolean, {env.string, env.const_any}, ADDR((void*) &LSString::is_permutation)},
	});
	method("isPalindrome", {
		{env.boolean, {env.string}, ADDR((void*) &LSString::is_palindrome)},
	});
	method("left", {
		{env.tmp_string, {env.string, env.integer}, ADDR((void*) string_left)},
		{env.tmp_string, {env.tmp_string, env.integer}, ADDR((void*) string_left_tmp)},
	});
	method("length", {
		{env.integer, {env.string}, ADDR((void*) string_length)},
	});
	method("lines", {
		{Type::tmp_array(env.string), {env.string}, ADDR((void*) &LSString::ls_lines)},
	});
	method("size", {
		{env.any, {env.string}, ADDR((void*) &LSString::ls_size_ptr)},
		{env.integer, {env.string}, ADDR((void*) &LSString::ls_size)},
	});
	method("replace", {
		{env.tmp_string, {env.string, env.string, env.string}, ADDR((void*) replace)},
		{env.tmp_string, {env.string, env.string, env.string}, ADDR((void*) v1_replace), LEGACY},
	});
	method("reverse", {
		{env.tmp_string, {env.string}, ADDR((void*) &LSString::ls_tilde)},
	});
	method("right", {
		{env.tmp_string, {env.string, env.integer}, ADDR((void*) string_right)},
		{env.tmp_string, {env.tmp_string, env.integer}, ADDR((void*) string_right_tmp)},
	});
	method("substring", {
		{env.tmp_string, {env.string, env.const_integer}, ADDR((void*) string_substring_1)},
		{env.tmp_string, {env.string, env.const_integer, env.const_integer}, ADDR((void*) string_substring_2)},
	});
	method("toArray", {
		{Type::tmp_array(env.any), {env.string}, ADDR((void*) string_toArray)},
	});
	method("toLower", {
		{env.tmp_string, {env.string}, ADDR((void*) string_toLower)},
	});
	method("toUpper", {
		{env.tmp_string, {env.string}, ADDR((void*) string_toUpper)},
	});
	method("split", {
		{Type::tmp_array(env.string), {env.string, env.string}, ADDR((void*) string_split)},
		{Type::tmp_array(env.string), {env.const_any, env.const_any}, ADDR((void*) string_split)},
	});
	method("startsWith", {
		{env.boolean, {env.string, env.string}, ADDR((void*) string_startsWith)},
	});
	method("code", {
		{env.integer, {env.const_string}, ADDR((void*) string_begin_code)},
		{env.integer, {env.const_string, env.const_integer}, ADDR((void*) string_code)},
	});
	method("number", {
		{env.long_, {env.const_string}, ADDR((void*) string_number)},
		{env.long_, {env.const_any}, ADDR((void*) string_number)},
	});
	auto map_fun = ADDR(&LSString::ls_map<LSFunction*>);
	method("map", {
		{env.tmp_string, {env.string, Type::fun_object(env.string, {env.string})}, (void*) map_fun},
	});
	method("sort", {
		{env.tmp_string, {env.string}, ADDR((void*) &LSString::sort)},
	});
	method("wordCount", {
		{env.any, {env.string}, ADDR((void*) &LSString::word_count_ptr)},
		{env.integer, {env.string}, ADDR((void*) &LSString::word_count)},
	});

	/** Internal **/
	method("to_bool", {
		{env.boolean, {env.string}, ADDR((void*) &LSString::to_bool)}
	}, PRIVATE);
	method("codePointAt", {
		{env.tmp_string, {env.string, env.integer}, ADDR((void*) &LSString::codePointAt)}
	}, PRIVATE);
	method("isize", {
		{env.integer, {env.string}, ADDR((void*) &LSString::int_size)}
	}, PRIVATE);
	method("iterator_begin", {
		{env.void_, {env.string, env.i8_ptr}, ADDR((void*) iterator_begin)}
	}, PRIVATE);
	method("iterator_end", {
		{env.void_, {env.i8_ptr}, ADDR((void*) &LSString::iterator_end)}
	}, PRIVATE);
	method("iterator_get", {
		{env.integer, {env.i8_ptr}, ADDR((void*) &LSString::iterator_get)},
		{env.tmp_string, {env.integer, env.string}, ADDR((void*) iterator_get)},
	}, PRIVATE);
	method("iterator_key", {
		{env.integer, {env.i8_ptr}, ADDR((void*) &LSString::iterator_key)}
	}, PRIVATE);
	method("iterator_next", {
		{env.void_, {env.i8_ptr}, ADDR((void*) &LSString::iterator_next)}
	}, PRIVATE);
	method("internal_plus_mpz_tmp", {
		{env.string, {env.i8_ptr, env.tmp_mpz_ptr, env.tmp_string}, ADDR((void*) internal_plus_mpz_tmp)}
	}, PRIVATE);
}

StringSTD::~StringSTD() {}

#if COMPILER
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

LSString* StringSTD::add_long(LSString* s, long i) {
	if (s->refs == 0) {
		s->append(std::to_string(i));
		return s;
	} else {
		return new LSString(*s + std::to_string(i));
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
	return c.insn_call(c.env.any, args, "Value.operator+=");
}

Compiler::value StringSTD::lt(Compiler& c, std::vector<Compiler::value> args, int) {
	auto res = c.insn_call(c.env.boolean, args, "Value.operator<");
	c.insn_delete_temporary(args[0]);
	c.insn_delete_temporary(args[1]);
	return res;
}

Compiler::value StringSTD::div(Compiler& c, std::vector<Compiler::value> args, int) {
	return c.insn_call(Type::tmp_array(c.env.string), args, "Value.operator/");
}

Compiler::value StringSTD::plus_mpz_tmp(Compiler& c, std::vector<Compiler::value> args, int) {
	return c.insn_call(c.env.tmp_string, { c.get_vm(), args[0], args[1] }, "String.internal_plus_mpz_tmp");
}

/*
 * Methods
 */
LSValue* string_charAt(LSString* string, int index) {
	LSValue* r = new LSString(string->operator[] (index));
	LSValue::delete_temporary(string);
	return r;
}

LSValue* StringSTD::chunk(LSString* string, int size) {
	auto result = new LSArray<LSValue*>();
	auto str = string->c_str();
	auto l = string->size();
	for (size_t i = 0; i < l; i += size) {
		result->push_move(new LSString(string->substr(i, size)));
	}
	LSValue::delete_temporary(string);
	return result;
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

LSValue* string_substring_1(LSString* string, int start) {
	LSValue* r = new LSString(string->substr(start));
	if (string->refs == 0) {
		delete string;
	}
	return r;
}
LSValue* string_substring_2(LSString* string, int start, int length) {
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
	c.insn_store(result->entry, c.insn_move_inc(args[2]));
	auto v = Variable::new_temporary("v", args[0].t->element());
	// v->create_entry(c);
	// c.add_temporary_variable(v);
	c.insn_foreach(args[0], c.env.void_, v, nullptr, [&](Compiler::value v, Compiler::value k) -> Compiler::value {
		auto r = c.insn_call(function, {c.insn_load(result->entry), v});
		c.insn_delete(c.insn_load(result->entry));
		c.insn_store(result->entry, c.insn_move_inc(r));
		return { c.env };
	});
	return c.insn_load(result->entry);
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

LSMap<int, int>* StringSTD::frequencies(LSString* string) {
	auto frequencies = new LSMap<int, int>();
	for (char c : *string) {
		frequencies->operator[] (c)++;
	}
	LSValue::delete_temporary(string);
	return frequencies;
}
int StringSTD::count(LSString* string, LSString* element) {
	int count = 0;
	for (char c : *string) {
		if (element->operator [] (0) == c) count++;
	}
	LSValue::delete_temporary(string);
	LSValue::delete_temporary(element);
	return count;
}

#endif

}
