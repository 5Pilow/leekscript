#ifndef COMPOUND_TYPE_HPP
#define COMPOUND_TYPE_HPP

#include "Type.hpp"

namespace ls {

class Compound_type : public Type {
public:
	std::vector<const Type*> types;
	Compound_type(std::set<const Type*> types, const Type* folded);
	virtual int id() const { return 0; }
	virtual const Type* element() const override;
	virtual const Type* pointed() const override;
	virtual const std::string getName() const override;
	virtual const std::string getJsonName() const { return "compound"; }
	virtual bool operator == (const Type*) const override;
	virtual bool callable() const override;
	virtual bool container() const override;
	bool all(std::function<bool(const Type*)> fun) const;
	bool some(std::function<bool(const Type*)> fun) const;
	virtual int distance(const Type* type) const override;
	virtual void implement(const Type* type) const override;
	#if COMPILER
	virtual llvm::Type* llvm(const Compiler& c) const override;
	#endif
	virtual std::string class_name() const override;
	virtual std::ostream& print(std::ostream& os) const override;
	virtual Type* clone() const override;
};

}

#endif