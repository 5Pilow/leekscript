#ifndef FIXED_ARRAY_TYPE_HPP
#define FIXED_ARRAY_TYPE_HPP

#include "Array_type.hpp"
#include <map>

namespace ls {

class Type;

class Fixed_array_type : public Array_type {
	std::vector<const Type*> _elements;
	const Type* _element;
public:
	Fixed_array_type(const std::vector<const Type*>&);
	virtual int id() const override { return 5; }
	virtual const std::string getName() const override;
	virtual Json json() const override;
	virtual bool iterable() const override { return true; }
	virtual bool container() const override { return true; }
	virtual const Type* key() const override;
	virtual const Type* element() const override;
	virtual const Type* element(size_t index) const override;
	virtual const std::vector<const Type*>& elements() const override;
	virtual size_t size() const override;
	virtual bool operator == (const Type*) const override;
	virtual int distance(const Type* type) const override;
	virtual const Type* iterator() const override;
	virtual std::string class_name() const override;
	virtual std::ostream& print(std::ostream&) const override;
	virtual Type* clone() const override;
};

}

#endif