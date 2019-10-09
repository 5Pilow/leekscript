#ifndef LONG_TYPE_HPP
#define LONG_TYPE_HPP

#include "Number_type.hpp"

namespace ls {

class Long_type : public Number_type {
public:
	Long_type(Environment& env) : Number_type(env) {}
	virtual const std::string getName() const override { return "long"; }
	virtual Json json() const override;
	virtual const Type* key() const override;
	virtual const Type* element() const override;
	virtual const Type* iterator() const override;
	virtual bool operator == (const Type*) const override;
	virtual int distance(const Type* type) const override;
	#if COMPILER
	virtual llvm::Type* llvm(Compiler& c) const override;
	#endif
	virtual std::string class_name() const override;
	virtual std::ostream& print(std::ostream& os) const override;
	virtual Type* clone() const override;
};

}

#endif