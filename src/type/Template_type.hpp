#ifndef TEMPLATE_TYPE_HPP
#define TEMPLATE_TYPE_HPP

#include "Any_type.hpp"

namespace ls {

class Type;

class Template_type : public Any_type {
	std::string _name;
public:
	const Type* _implementation = Type::void_;
	virtual void reset() const override;
	Template_type(const std::string name) : _name(name) {}
	virtual void implement(const Type* type) const override;
	virtual bool operator == (const Type*) const override;
	#if COMPILER
	virtual llvm::Type* llvm(Compiler& c) const override;
	#endif
	virtual std::ostream& print(std::ostream& os) const override;
	virtual Type* clone() const override;
	
};

}

#endif