#ifndef META_TEMPORARY_TYPE_HPP
#define META_TEMPORARY_TYPE_HPP

#include "Type.hpp"

namespace ls {

class Meta_temporary_type : public Type {
public:
	const Type* type;
	Meta_temporary_type(const Type* type) : Type(type->env), type(type) {}
	virtual int id() const override { return 0; }
	virtual const std::string getName() const override { return "meta_temporary"; }
	virtual Json json() const override;
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