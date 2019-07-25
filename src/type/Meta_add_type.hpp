#ifndef META_ADD_TYPE_HPP
#define META_ADD_TYPE_HPP

#include "Type.hpp"

namespace ls {

class Meta_add_type : public Type {
public:
	const Type* t1;
	const Type* t2;
	Meta_add_type(const Type* t1, const Type* t2) : t1(t1), t2(t2) {}
	virtual int id() const { return 0; }
	virtual const std::string getName() const override { return "meta_add"; }
	virtual const std::string getJsonName() const { return "meta_add"; }
	virtual bool operator == (const Type*) const override;
	virtual int distance(const Type* type) const override;
	#if COMPILER
	virtual llvm::Type* llvm(Compiler& c) const override;
	#endif
	virtual std::string class_name() const override;
	virtual std::ostream& print(std::ostream& os) const override;
	virtual Type* clone() const override;
};

}

#endif