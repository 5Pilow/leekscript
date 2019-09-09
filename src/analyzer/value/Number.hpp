#ifndef NUMBER_HPP
#define NUMBER_HPP

#include <memory>
#include <gmp.h>
#include "Value.hpp"
#include "../lexical/Token.hpp"

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
	mpz_t mpz_value;
	mpf_t mpf_value;
	bool mpz_value_initialized = false;
	bool pointer = false;

	Number(std::string value, Token* token);
	virtual ~Number();

	virtual void print(std::ostream&, int indent, bool debug, bool condensed) const override;
	virtual Location location() const override;

	virtual void analyze(SemanticAnalyzer*) override;
	virtual bool is_zero() const override;

	virtual Compiler::value compile(Compiler&) const override;

	virtual std::unique_ptr<Value> clone() const override;
};

}

#endif
