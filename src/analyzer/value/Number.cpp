#include "Number.hpp"
#include <limits.h>
#include "../../type/Type.hpp"
#include "../semantic/SemanticAnalyzer.hpp"

namespace ls {

Number::Number(Environment& env, std::string value, Token* token) : Value(env), token(token), value(value) {
	constant = true;
}

Number::~Number() {
	#if COMPILER
	if (mpz_value_initialized) {
		mpz_clear(mpz_value);
	}
	#endif
}

void Number::print(std::ostream& os, int, PrintOptions options) const {
	os << value;
	if (options.debug) {
		os << " " << type;
	}
}

Location Number::location() const {
	return token->location;
}

void Number::analyze(SemanticAnalyzer* analyzer) {
	auto& env = analyzer->env;
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
			#if COMPILER
			// LCOV_EXCL_START
			if (!mpz_value_initialized) {
				mpf_init_set_str(mpf_value, clean_value.c_str(), base);
				mpz_value_initialized = true;
			}
			assert(false && "No support for mpf numbers yet");
			#endif
			// LCOV_EXCL_STOP
		} else {
			type = env.real;
		}
	} else {
		#if COMPILER
		if (!mpz_value_initialized) {
			mpz_init_set_str(mpz_value, clean_value.c_str(), base);
			mpz_value_initialized = true;
		}
		if (!mp_number and !long_number and mpz_fits_sint_p(mpz_value)) {
			type = env.integer;
			int_value = mpz_get_si(mpz_value);
			double_value = int_value;
		} else if (!mp_number and mpz_fits_slong_p(mpz_value)) {
			type = env.long_;
			long_value = mpz_get_si(mpz_value);
			double_value = long_value;
		} else {
			type = env.tmp_mpz_ptr;
		}
		#else
		try {
			int_value = std::stoi(clean_value);
			type = env.integer;
		} catch (const std::out_of_range&) {
			try {
				long_value = std::stol(clean_value);
				type = env.long_;
			} catch (const std::out_of_range&) {
				type = env.tmp_mpz_ptr;
			}
		}
		if (long_number) type = env.long_;
		if (mp_number) type = env.tmp_mpz_ptr;
		#endif
	}
	if (pointer) {
		type = env.any;
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
		#if COMPILER
		return mpz_cmp_ui(mpz_value, 0) == 0;
		#else
		return false;
		#endif
	} else {
		return int_value == 0;
	}
}

Json Number::hover(SemanticAnalyzer& analyzer, size_t position) const {
	return {
		{ "type", type->json() }
	};
}

#if COMPILER
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
	if (type == c.env.tmp_mpz_ptr) {
		auto s = c.new_const_string(clean_value);
		auto r = c.create_entry("m", c.env.tmp_mpz);
		c.insn_call(c.env.void_, {r, s, c.new_integer(base)}, "Number.mpz_init_str");
		c.increment_mpz_created();
		return r;
	}
	return c.new_integer(int_value);
}
#endif

std::unique_ptr<Value> Number::clone(Block* parent) const {
	return std::make_unique<Number>(type->env, value, token);
}

}
