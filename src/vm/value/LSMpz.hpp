#ifndef LSMPZ_H_
#define LSMPZ_H_

#include <iostream>
#include <gmp.h>
#include <gmpxx.h>
#include "../LSValue.hpp"
#include "LSClosure.hpp"

namespace ls {

class LSClass;
class VM;

class LSMpz : public LSValue {
public:
	
	__mpz_struct value;
	VM* vm; // For ref-counting

	static LSClass* clazz;
	static LSMpz* get_from_mpz(VM* vm, __mpz_struct);
	static LSMpz* get_from_tmp(VM* vm, __mpz_struct);
	static LSMpz* get(VM* vm);
	static LSMpz* get(VM* vm, long);

	LSMpz(VM* vm);
	LSMpz(VM* vm, __mpz_struct value);
	LSMpz(VM* vm, long value);

	virtual ~LSMpz();

	/*
	 * LSValue
	 */
	bool to_bool() const override;
	bool ls_not() const override;
	LSValue* ls_minus() override;
	LSValue* ls_tilde() override;
	LSValue* ls_preinc() override;
	LSValue* ls_inc() override;
	LSValue* ls_predec() override;
	LSValue* ls_dec() override;

	virtual LSValue* add(LSValue*) override;
	virtual LSValue* add_eq(LSValue*) override;
	virtual LSValue* sub(LSValue*) override;
	virtual LSValue* sub_eq(LSValue*) override;
	virtual LSValue* mul(LSValue*) override;
	virtual LSValue* mul_eq(LSValue*) override;
	virtual LSValue* div(LSValue*) override;
	virtual LSValue* div_eq(LSValue*) override;
	virtual LSValue* int_div(LSValue*) override;
	virtual LSValue* int_div_eq(LSValue*) override;
	virtual LSValue* pow(LSValue*) override;
	virtual LSValue* pow_eq(LSValue*) override;
	virtual LSValue* mod(LSValue*) override;
	virtual LSValue* mod_eq(LSValue*) override;
	virtual LSValue* double_mod(LSValue*) override;
	virtual LSValue* double_mod_eq(LSValue*) override;
	bool eq(const LSValue*) const override;
	bool lt(const LSValue*) const override;

	bool operator == (int value) const override;
	bool operator == (double value) const override;
	bool operator < (int value) const override;
	bool operator < (double value) const override;

	int abso() const override;

	LSValue* clone() const override;

	std::ostream& dump(std::ostream& os, int level) const override;
	std::string json() const override;

	LSValue* getClass() const override;
};

}

#endif
