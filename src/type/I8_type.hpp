#ifndef I8_TYPE_HPP
#define I8_TYPE_HPP

#include "Number_type.hpp"

namespace ls {

class I8_type : public Number_type {
public:
	I8_type() {}
	virtual const std::string getName() const override { return "i8"; }
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