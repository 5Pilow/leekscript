#ifndef CLASS_TYPE_HPP
#define CLASS_TYPE_HPP

#include "Pointer_type.hpp"

namespace ls {

class Class_type : public Pointer_type {
	std::string name;
public:
	Class_type(Environment& env, std::string name);
	virtual int id() const override { return 11; }
	virtual const std::string getName() const override;
	virtual Json json() const override;
	virtual bool operator == (const Type*) const override;
	virtual bool callable() const override { return true; }
	virtual int distance(const Type* type) const override;
	virtual std::string class_name() const override;
	virtual std::ostream& print(std::ostream& os) const override;
	virtual Type* clone() const override;
};

}

#endif