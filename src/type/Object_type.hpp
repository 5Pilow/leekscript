#ifndef OBJECT_TYPE_HPP
#define OBJECT_TYPE_HPP

#include "Pointer_type.hpp"

namespace ls {

class Object_type : public Pointer_type {
public:
	Object_type(Environment& env, bool native = false);
	virtual int id() const override { return 10; }
	virtual const std::string getName() const override { return "object"; }
	virtual const std::string getJsonName() const override { return "object"; }
	virtual bool iterable() const { return false; } // TODO not iterable for now
	virtual bool container() const override { return true; }
	virtual bool operator == (const Type*) const override;
	virtual int distance(const Type* type) const override;
	virtual std::string class_name() const override;
	virtual std::ostream& print(std::ostream& os) const override;
	virtual Type* clone() const override;
};

}

#endif