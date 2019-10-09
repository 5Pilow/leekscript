#ifndef NULL_TYPE_HPP
#define NULL_TYPE_HPP

#include "Pointer_type.hpp"

namespace ls {

class Null_type : public Pointer_type {
public:
	Null_type(Environment& env);
	virtual int id() const override { return 1; }
	virtual const std::string getName() const override { return "null"; }
	virtual Json json() const override;
	virtual bool operator == (const Type*) const override;
	virtual int distance(const Type* type) const override;
	virtual std::string class_name() const override;
	virtual std::ostream& print(std::ostream& os) const override;
	virtual Type* clone() const override;
};

}

#endif