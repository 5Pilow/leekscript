#ifndef STRING_TYPE_HPP
#define STRING_TYPE_HPP

#include "Pointer_type.hpp"

namespace ls {

class String_type : public Pointer_type {
public:
	String_type(Environment& env);
	virtual int id() const override { return 4; }
	virtual const std::string getName() const override { return "string"; }
	virtual const std::string getJsonName() const { return "string"; }
	virtual bool iterable() const { return true; }
	virtual const Type* iterator() const override;
	virtual const Type* key() const override;
	virtual const Type* element() const override;
	virtual bool container() const override { return true; }
	virtual bool operator == (const Type*) const override;
	virtual int distance(const Type* type) const override;
	virtual std::string class_name() const override;
	virtual std::ostream& print(std::ostream& os) const override;
	virtual Type* clone() const override;
};

}

#endif