#ifndef SET_TYPE_HPP
#define SET_TYPE_HPP

#include <unordered_map>
#include "Pointer_type.hpp"

namespace ls {

class Type;

class Set_type : public Pointer_type {
	const Type* const _element;
public:
	Set_type(const Type* element);
	virtual int id() const override { return 7; }
	virtual const std::string getName() const override;
	virtual const std::string getJsonName() const override { return "set"; }
	virtual bool iterable() const override { return true; }
	virtual const Type* iterator() const override;
	virtual bool container() const override { return true; }
	virtual const Type* key() const override;
	virtual const Type* element() const override;
	virtual bool operator == (const Type*) const override;
	virtual int distance(const Type* type) const override;
	virtual std::string class_name() const override;
	virtual std::ostream& print(std::ostream&) const override;
	virtual Type* clone() const override;

	static std::unordered_map<const Type*, const Type*> nodes;
	static std::unordered_map<const Type*, const Type*> iterators;
	static const Type* get_node_type(const Type* element);
	static const Type* get_iterator(const Type* element);
};

}

#endif