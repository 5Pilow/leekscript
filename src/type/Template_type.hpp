#ifndef TEMPLATE_TYPE_HPP
#define TEMPLATE_TYPE_HPP

#include "Any_type.hpp"

namespace ls {

class Type;

class Template_type : public Type {
	std::string _name;
public:
	const Type* _implementation = nullptr;
	virtual void reset() const override;
	Template_type(Environment& env, const std::string name);
	virtual void implement(const Type* type) const override;
	virtual bool operator == (const Type*) const override;
	virtual int distance(const Type* type) const override;
	#if COMPILER
	virtual llvm::Type* llvm(Compiler& c) const override;
	#endif
	virtual const std::string getJsonName() const override;
	virtual std::string class_name() const override;
	virtual const std::string getName() const override;
	virtual std::ostream& print(std::ostream& os) const override;
	virtual Type* clone() const override;

};

}

#endif