#include "Number.hpp"
#include <limits.h>
#include "../../vm/value/LSNumber.hpp"
#include "../../type/Type.hpp"

namespace ls {

Number::Number(std::string value, Token* token) : token(token), value(value) {
	constant = true;
}

Number::~Number() {
	if (mpz_value_initialized) {
		mpz_clear(mpz_value);
	}
}

void Number::print(std::ostream& os, int, bool debug, bool condensed) const {
	os << value;
	if (debug) {
		os << " " << type;
	}
}

Location Number::location() const {
	return token->location;
}

void Number::analyze(SemanticAnalyzer*) {

	// Get the base
	base = 10;
	clean_value = value;
	if (value.size() > 2 and value[0] == '0') {
		if (value[1] == 'b') {
			base = 2;
			clean_value = value.substr(2);
		} else if (value[1] == 'x') {
			base = 16;
			clean_value = value.substr(2);
		}
	}
	// long and MP number?
	bool long_number = clean_value[clean_value.size() - 1] == 'l';
	bool mp_number = clean_value[clean_value.size() - 1] == 'm';
	if (long_number or mp_number) {
		clean_value = clean_value.substr(0, clean_value.size() - 1);
	}
	// Floating-point?
	bool floating = std::find(clean_value.begin(), clean_value.end(), '.') != clean_value.end();

	// Determine the possible container for the number
	if (floating) {
		if (!mp_number) {
			try {
				double_value = std::stod(clean_value);
			} catch (...) { // LCOV_EXCL_LINE
				mp_number = true; // number too large, GMP needed LCOV_EXCL_LINE
			}
		}
		if (mp_number) {
			// LCOV_EXCL_START
			if (!mpz_value_initialized) {
				mpf_init_set_str(mpf_value, clean_value.c_str(), base);
				mpz_value_initialized = true;
			}
			assert(false && "No support for mpf numbers yet");
			// LCOV_EXCL_STOP
		} else {
			type = Type::real;
		}
	} else {
		if (!mpz_value_initialized) {
			mpz_init_set_str(mpz_value, clean_value.c_str(), base);
			mpz_value_initialized = true;
		}
		if (!mp_number and !long_number and mpz_fits_sint_p(mpz_value)) {
			type = Type::integer;
			int_value = mpz_get_si(mpz_value);
			double_value = int_value;
		} else if (!mp_number and mpz_fits_slong_p(mpz_value)) {
			type = Type::long_;
			long_value = mpz_get_si(mpz_value);
			double_value = long_value;
		} else {
			type = Type::tmp_mpz_ptr;
		}
	}
	if (pointer) {
		type = Type::any;
	}
	// TODO ?
	// type.constant = true;
}

bool Number::is_zero() const {
	if (type->is_any() or type->is_real()) {
		return double_value == 0;
	} else if (type->is_long()) {
		return long_value == 0;
	} else if (type->is_mpz_ptr()) {
		return mpz_cmp_ui(mpz_value, 0) == 0;
	} else {
		return int_value == 0;
	}
}

Compiler::value Number::compile(Compiler& c) const {
	if (type->is_any()) {
		return c.insn_to_any(c.new_real(double_value));
	}
	if (type->is_long()) {
		return c.new_long(long_value);
	}
	if (type->is_real()) {
		return c.new_real(double_value);
	}
	if (type == Type::tmp_mpz_ptr) {
		auto s = c.new_const_string(clean_value);
		auto r = c.create_entry("m", Type::tmp_mpz);
		c.insn_call(Type::void_, {r, s, c.new_integer(base)}, "Number.mpz_init_str");
		c.increment_mpz_created();
		return r;
	}
	return c.new_integer(int_value);
}

std::unique_ptr<Value> Number::clone() const {
	return std::make_unique<Number>(value, token);
}

}
