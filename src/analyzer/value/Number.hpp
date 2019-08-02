#ifndef NUMBER_HPP
#define NUMBER_HPP

#include <memory>
#include "Value.hpp"
#include "../lexical/Token.hpp"
#if COMPILER
#include <gmp.h>
#endif

namespace ls {

class Number : public Value {
public:

	Token* token;
	std::string value;
	std::string clean_value;
	int base = 10;
	int int_value = 0;
	long long_value = 0;
	double double_value = 0;
	#if COMPILER
	mpz_t mpz_value;
	mpf_t mpf_value;
	bool mpz_value_initialized = false;
	#endif
	bool pointer = false;

	Number(Environment& env, std::string value, Token* token);
	virtual ~Number();

	virtual void print(std::ostream&, int indent, PrintOptions options) const override;
	virtual Location location() const override;

	virtual void analyze(SemanticAnalyzer*) override;
	virtual bool is_zero() const override;

	#if COMPILER
	virtual Compiler::value compile(Compiler&) const override;
	#endif

	virtual std::unique_ptr<Value> clone() const override;
};

}

#endif
