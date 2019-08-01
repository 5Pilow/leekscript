#ifndef MAP_TYPE_HPP
#define MAP_TYPE_HPP

#include "Pointer_type.hpp"

namespace ls {

class Type;

class Map_type : public Pointer_type {
	const Type* const _key;
	const Type* const _element;
public:
	Map_type(const Type* key, const Type* element);
	virtual int id() const override { return 6; }
	virtual const std::string getName() const override;
	virtual const std::string getJsonName() const override { return "map"; }
	virtual bool iterable() const override { return true; }
	virtual const Type* iterator() const override;
	virtual bool container() const override { return true; }
	virtual const Type* element() const override;
	virtual const Type* key() const override;
	virtual bool operator == (const Type*) const override;
	virtual int distance(const Type*) const override;
	virtual std::string class_name() const override;
	virtual std::ostream& print(std::ostream& os) const override;
	virtual Type* clone() const override;
};

}

#endif