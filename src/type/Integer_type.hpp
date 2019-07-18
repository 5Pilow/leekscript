#ifndef INTEGER_TYPE_HPP
#define INTEGER_TYPE_HPP

#include "Number_type.hpp"

namespace ls {

class Integer_type : public Number_type {
public:
	Integer_type() {}
	virtual const std::string getName() const override { return "int"; }
	virtual const Type* key() const override;
	virtual const Type* element() const override;
	virtual const Type* iterator() const override;
	virtual bool operator == (const Type*) const;
	virtual int distance(const Type* type) const override;
	#if COMPILER
	virtual llvm::Type* llvm(const Compiler& c) const override;
	#endif
	virtual std::string class_name() const override;
	virtual std::ostream& print(std::ostream&) const override;
	virtual Type* clone() const override;
};

}

#endif