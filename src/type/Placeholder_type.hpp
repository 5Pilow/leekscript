#ifndef PLACEHOLDER_TYPE_HPP
#define PLACEHOLDER_TYPE_HPP

#include "Any_type.hpp"

namespace ls {

class Type;

class Placeholder_type : public Any_type {
	std::string _name;
	const Type* _implementation = nullptr;
	const Type* _element = nullptr;
public:
	Placeholder_type(Environment& env, const std::string name) : Any_type(env), _name(name) {
		placeholder = true;
	}
	void implement(const Type* type) const;
	virtual const Type* element() const override;
	virtual bool operator == (const Type*) const override;
	virtual int distance(const Type* type) const override;
	virtual std::ostream& print(std::ostream& os) const override;
	virtual Type* clone() const override;
};

}

#endif