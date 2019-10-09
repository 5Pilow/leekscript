#ifndef ARRAY_TYPE_HPP
#define ARRAY_TYPE_HPP

#include "Pointer_type.hpp"
#include <map>

namespace ls {

class Type;

class Array_type : public Pointer_type {
	const Type* const _element;
public:
	Array_type(const Type* element);
	virtual int id() const override { return 5; }
	virtual const std::string getName() const override;
	virtual Json json() const override;
	virtual bool iterable() const override { return true; }
	virtual bool container() const override { return true; }
	virtual const Type* key() const override;
	virtual const Type* element() const override;
	virtual bool operator == (const Type*) const override;
	virtual int distance(const Type* type) const override;
	virtual const Type* iterator() const override;
	virtual std::string class_name() const override;
	virtual std::ostream& print(std::ostream&) const override;
	virtual Type* clone() const override;
};

}

#endif