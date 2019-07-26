#ifndef BOOL_TYPE_HPP
#define BOOL_TYPE_HPP

#include "Type.hpp"

namespace ls {

class Bool_type : public Type {
public:
	Bool_type(Environment& env) : Type(env, true) {}
	virtual int id() const override { return 2; }
	virtual const std::string getName() const override { return "bool"; }
	virtual const std::string getJsonName() const { return "boolean"; }
	virtual bool operator == (const Type*) const override;
	virtual int distance(const Type*) const override;
	#if COMPILER
	virtual llvm::Type* llvm(Compiler& c) const override;
	#endif
	virtual std::string class_name() const override;
	virtual std::ostream& print(std::ostream& os) const override;
	virtual Type* clone() const override;
};

}

#endif