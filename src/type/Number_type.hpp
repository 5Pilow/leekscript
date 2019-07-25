#ifndef NUMBER_TYPE_HPP
#define NUMBER_TYPE_HPP

#include "Pointer_type.hpp"

namespace ls {

class Number_type : public Type {
public:
	Number_type();
	virtual int id() const override { return 3; }
	virtual const std::string getName() const override { return "number"; }
	virtual const std::string getJsonName() const { return "number"; }
	virtual bool iterable() const { return true; }
	virtual bool operator == (const Type*) const override;
	virtual int distance(const Type* type) const override;
	virtual std::string class_name() const override;
	#if COMPILER
	virtual llvm::Type* llvm(Compiler& c) const override;
	#endif
	virtual std::ostream& print(std::ostream& os) const override;
	virtual Type* clone() const override;
};

}

#endif