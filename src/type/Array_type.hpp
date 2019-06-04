#ifndef ARRAY_TYPE_HPP
#define ARRAY_TYPE_HPP

#include "Type.hpp"
#include "Pointer_type.hpp"
#include <map>

namespace ls {

class Array_type : public Pointer_type {
	const Type* const _element;
public:
	Array_type(const Type* element);
	virtual int id() const override { return 5; }
	virtual const std::string getName() const { return "array"; }
	virtual const std::string getJsonName() const { return "array"; }
	virtual bool iterable() const { return true; }
	virtual bool is_container() const { return true; }
	virtual const Type* key() const override;
	virtual const Type* element() const override;
	virtual bool operator == (const Base_type*) const override;
	virtual int distance(const Base_type* type) const override;
	virtual const Type* iterator() const override;
	virtual std::string clazz() const override;
	virtual std::ostream& print(std::ostream&) const override;

	static std::map<const Type*, const Array_type*> cache;
	static const Array_type* create(const Type* element);
};

}

#endif