#ifndef MPZ_TYPE_HPP
#define MPZ_TYPE_HPP

#include "Number_type.hpp"

namespace ls {

class Mpz_type : public Number_type {
public:
	Mpz_type(Environment& env) : Number_type(env) {}
	virtual const std::string getName() const override { return "mpz"; }
	virtual Json json() const override;
	virtual bool operator == (const Type*) const override;
	virtual int distance(const Type* type) const override;
	#if COMPILER
	virtual llvm::Type* llvm(Compiler& c) const override;
	#endif
	virtual std::string class_name() const override;
	virtual std::ostream& print(std::ostream& os) const override;
	virtual Type* clone() const override;

	#if COMPILER
	static llvm::Type* mpz_type;
	static llvm::Type* get_mpz_type(Compiler& c);
	#endif
};

}

#endif