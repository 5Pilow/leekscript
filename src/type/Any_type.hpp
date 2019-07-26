#ifndef ANY_TYPE_HPP
#define ANY_TYPE_HPP

#include "Pointer_type.hpp"

namespace ls {

class Any_type : public Pointer_type {
public:
	Any_type(Environment& env);
	virtual ~Any_type() {}
	virtual const std::string getName() const override { return "any"; }
	virtual bool operator == (const Type*) const override;
	virtual bool callable() const override { return true; }
	virtual bool iterable() const override { return true; }
	virtual bool container() const override { return true; }
	virtual int distance(const Type* type) const override;
	virtual std::string class_name() const override;
	virtual std::ostream& print(std::ostream& os) const override;
	virtual Type* clone() const override;
};

}

#endif