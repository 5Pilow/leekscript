#include <sstream>
#include <algorithm>
#include "LSString.hpp"
#include "LSNull.hpp"
#include "LSBoolean.hpp"
#include "LSClass.hpp"
#include "LSNumber.hpp"
#include "LSArray.hpp"
#include <string.h>
#include "../../util/utf8.h"
#include "../VM.hpp"
#include "../../environment/Environment.hpp"

namespace ls {

LSString::iterator LSString::iterator_begin(LSString* s) {
	return {(char*) s->c_str(), 0, 0, 0, 0};
}

void LSString::iterator_next(LSString::iterator* it) {
	if (it->pos == it->next_pos) {
		assert(false); // LCOV_EXCL_LINE
		// Uncomment to be able to call next without having call get() before
		// u8_nextchar((char*) it->buffer, &it->pos);
		// it->next_pos = it->pos;
	} else {
		it->pos = it->next_pos;
	}
	it->index++;
}

u_int32_t LSString::iterator_get(LSString::iterator* it) {
	if (it->pos == it->next_pos) {
		int pos = it->pos;
		auto c = u8_nextchar(it->buffer, &pos);
		it->next_pos = pos;
		it->character = c;
	}
	return it->character;
}

int LSString::iterator_key(LSString::iterator* it) {
	return it->index;
}

bool LSString::iterator_end(LSString::iterator* it) {
	bool end = it->buffer[it->pos] == 0;
	return end;
}
LSString* LSString::constructor_1() {
	return new LSString();
}
LSString* LSString::constructor_2(char* s) {
	return new LSString(s);
}

LSString::LSString() : LSValue(STRING) {}
LSString::LSString(const char value) : LSValue(STRING), std::string(std::string(1, value)) {}
LSString::LSString(const char* value) : LSValue(STRING), std::string(value) {}
LSString::LSString(const std::string& value) : LSValue(STRING), std::string(value) {}
LSString::LSString(const Json& json) : LSValue(STRING), std::string(json.get<std::string>()) {}

LSString::~LSString() {}

LSString* LSString::charAt(int index) const {
	return new LSString(this->operator[] (index));
}

LSString* LSString::codePointAt(int index) const {
	char buff[5];
	u_int32_t c = u8_char_at((char*) this->c_str(), index);
	u8_toutf8(buff, 5, &c, 1);
	return new LSString(buff);
}

int LSString::unicode_length() const {
	return u8_strlen(this->c_str());
}

bool LSString::is_permutation(LSString* other) {
	bool result = this->size() == other->size() and std::is_permutation(this->begin(), this->end(), other->begin());
	LSValue::delete_temporary(this);
	LSValue::delete_temporary(other);
	return result;
}

LSString* LSString::sort() {
	if (refs == 0) {
		std::sort(this->begin(), this->end());
		return this;
	} else {
		std::string res = *this;
		std::sort(res.begin(), res.end());
		return new LSString(res);
	}
}

bool LSString::is_palindrome() const {
	bool r = std::equal(this->begin(), this->begin() + this->size() / 2, this->rbegin());
	LSValue::delete_temporary(this);
	return r;
}

template <class F>
LSValue* string_base_fold(LSString* string, F function, LSValue* v0) {
	char buff[5];
	const char* string_chars = string->c_str();
	int i = 0;
	int l = strlen(string_chars);
	auto result = ls::move(v0);
	while (i < l) {
		u_int32_t c = u8_nextchar(string_chars, &i);
		u8_toutf8(buff, 5, &c, 1);
		LSString* ch = new LSString(buff);
		ch->refs = 1;
		result = ls::call<F>(function, result, ch);
		LSValue::delete_ref(ch);
	}
	LSValue::delete_temporary(string);
	return result;
}
template <>
LSValue* LSString::ls_foldLeft(LSFunction* function, LSValue* v0) {
	return string_base_fold(this, function, v0);
}
template <>
LSValue* LSString::ls_foldLeft(LSClosure* closure, LSValue* v0) {
	return string_base_fold(this, closure, v0);
}

int LSString::int_size() const {
	return size();
}

int LSString::ls_size() const {
	auto s = unicode_length();
	LSValue::delete_temporary(this);
	return s;
}
LSValue* LSString::ls_size_ptr() const {
	return LSNumber::get(ls_size());
}

int LSString::word_count() const {
	auto c = std::distance(std::istream_iterator<std::string>(std::istringstream(*this) >> std::ws), std::istream_iterator<std::string>());
	LSValue::delete_temporary(this);
	return c;
}

LSValue* LSString::word_count_ptr() const {
	return LSNumber::get(word_count());
}

LSArray<LSValue*>* LSString::ls_lines() const {
	auto results = new LSArray<LSValue*>();
	std::stringstream ss(*this);
	std::string item;
	while (std::getline(ss, item, '\n')) {
		results->push_inc(new LSString(item));
	}
	LSValue::delete_temporary(this);
	return results;
}

template <class F>
LSString* base_map(LSString* s, F function) {
	char buff[5];
	LSValue* r = new LSString();
	const char* string_chars = s->c_str();
	int i = 0;
	int l = strlen(string_chars);
	while (i < l) {
		u_int32_t c = u8_nextchar(string_chars, &i);
		u8_toutf8(buff, 5, &c, 1);
		LSString* ch = new LSString(buff);
		ch->refs = 1;
		r->add_eq(ls::call<LSString*>(function, ch));
		LSValue::delete_ref(ch);
	}
	LSValue::delete_temporary(s);
	return (LSString*) r;
}

template <>
LSString* LSString::ls_map(LSFunction* function) {
	return base_map(this, function);
}
template <>
LSString* LSString::ls_map(LSClosure* closure) {
	return base_map(this, closure);
}

/*
 * LSValue methods
 */
bool LSString::to_bool() const {
	return size() > 0;
}

bool LSString::ls_not() const {
	auto r = size() == 0;
	LSValue::delete_temporary(this);
	return r;
}

LSValue* LSString::ls_tilde() {
	char buff[5];
	char* string_chars = (char*) this->c_str();
	int i = 0;
	int l = strlen(string_chars);
	std::string reversed = "";
	while (i < l) {
		u_int32_t c = u8_nextchar(string_chars, &i);
		u8_toutf8(buff, 5, &c, 1);
		reversed = buff + reversed;
	}
	if (refs == 0) delete this;
	return new LSString(reversed);
}

LSValue* LSString::add(LSValue* v) {
	if (refs == 0) {
		this->append(v->to_string());
		LSValue::delete_temporary(v);
		return this;
	}
	auto r = new LSString(*this + v->to_string());
	LSValue::delete_temporary(v);
	return r;
}

LSValue* LSString::add_eq(LSValue* v) {
	if (auto s = dynamic_cast<LSString*>(v)) {
		append(*(const std::string*) s);
	} else {
		append(v->to_string());
	}
	LSValue::delete_temporary(v);
	return this;
}

LSValue* LSString::mul(LSValue* v) {
	if (v->type != NUMBER) {
		LSValue::delete_temporary(this);
		LSValue::delete_temporary(v);
		throw vm::ExceptionObj(vm::Exception::NO_SUCH_OPERATOR);
	}
	auto number = static_cast<LSNumber*>(v);
	std::string r;
	for (int i = 0; i < number->value; ++i) {
		r += *this;
	}
	if (number->refs == 0) delete number;
	if (refs == 0) {
		*this = r;
		return this;
	}
	return new LSString(r);
}

LSValue* LSString::div(LSValue* v) {
	if (v->type != STRING) {
		LSValue::delete_temporary(this);
		LSValue::delete_temporary(v);
		throw vm::ExceptionObj(vm::Exception::NO_SUCH_OPERATOR);
	}
	auto string = static_cast<LSString*>(v);
	char buff[5];
	char* string_chars = (char*) this->c_str();
	auto array = new LSArray<LSValue*>();
	if (string->size() == 0) {
		int i = 0;
		int l = strlen(string_chars);
		while (i < l) {
			u_int32_t c = u8_nextchar(string_chars, &i);
			u8_toutf8(buff, 5, &c, 1);
			LSString* ch = new LSString(buff);
			array->push_inc(ch);
		}
	} else {
		u_int32_t separator = u8_char_at((char*) string->c_str(), 0);
		int i = 0;
		int l = strlen(string_chars);
		std::string item = "";
		while (i < l) {
			u_int32_t c = u8_nextchar(string_chars, &i);
			if (c == separator) {
				array->push_inc(new LSString(item));
				item = "";
			} else {
				u8_toutf8(buff, 5, &c, 1);
				item += buff;
			}
		}
		array->push_inc(new LSString(item));
	}
	if (string->refs == 0) delete string;
	if (refs == 0) delete this;
	return array;
}

bool LSString::eq(const LSValue* v) const {
	if (v->type == STRING) {
		auto str = static_cast<const LSString*>(v);
		return compare(*str) == 0;
	}
	return false;
}

bool LSString::lt(const LSValue* v) const {
	if (v->type == STRING) {
		auto str = static_cast<const LSString*>(v);
		return compare(*str) < 0;
	}
	return LSValue::lt(v);
}

u_int32_t LSString::u8_char_at(char* s, int pos) {
	int i = 0;
	int j = 0;
	int l = strlen(s);
	while (i < l) {
		u_int32_t c = u8_nextchar(s, &i);
		if (j++ == pos) return c;
	}
	return 0;
}

LSValue* LSString::at(const LSValue* key) const {
	if (key->type == NUMBER) {
		const LSNumber* n = static_cast<const LSNumber*>(key);
		char buff[5];
		u_int32_t c = u8_char_at((char*) this->c_str(), (int) n->value);
		u8_toutf8(buff, 5, &c, 1);
		return new LSString(buff);
	}
	return LSNull::get();
}

LSValue* LSString::range(int start, int end) const {

	char buff[5];
	char* string_chars = (char*) this->c_str();
	std::string new_string;

	int i = 0;
	int j = 0;
	int l = strlen(string_chars);
	while (i < l) {
		j++;
		u_int32_t c = u8_nextchar(string_chars, &i);
		if (j <= start) continue;
		u8_toutf8(buff, 5, &c, 1);
		new_string += buff;
		if (j > end) break;
	}
	return new LSString(new_string);
//	return new LSString(this->substr(start, end - start + 1));
}

int LSString::abso() const {
	return unicode_length();
}

std::ostream& LSString::print(std::ostream& os) const {
	os << (const std::string&) *this;
	return os;
}

std::ostream& LSString::dump(std::ostream& os, int) const {
	os << "'" << escape_control_characters() << "'";
	return os;
}

std::string LSString::json() const {
	return "\"" + escaped('"') + "\"";
}

std::string LSString::escape_control_characters() const {
	std::string res = *this;
	res.erase(std::remove(res.begin(), res.end(), '\b'), res.end());
	res.erase(std::remove(res.begin(), res.end(), '\f'), res.end());
	res.erase(std::remove(res.begin(), res.end(), '\r'), res.end());
	return res;
}

std::string LSString::escaped(char quote) const {

	char buff[5];
	char* string_chars = (char*) this->c_str();
	std::string new_string;
	int i = 0;
	int l = strlen(string_chars);
	while (i < l) {
		u_int32_t c = u8_nextchar(string_chars, &i);

		if (c == '\b') {
			// don't print the backspace
		}
		/* else if (c == '\f') {
			new_string += "\\f";
		} else if (c == '\n') {
			new_string += "\\n";
		} else if (c == '\r') {
			new_string += "\\r";
		} else if (c == '\t') {
			new_string += "\\t";
		} else
		*/
		else if (c == (u_int32_t)quote) {
			new_string += '\\';
			new_string += quote;
		} else {
			u8_toutf8(buff, 5, &c, 1);
			new_string += buff;
		}
	}
	return new_string;
}

LSValue* LSString::clone() const {
	return new LSString((const std::string&) *this);
}

LSValue* LSString::getClass(VM* vm) const {
	return vm->env.string_class.get();
}

}
