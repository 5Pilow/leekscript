#ifndef POINTER_TYPE_HPP
#define POINTER_TYPE_HPP

#include "Type.hpp"

namespace ls {

class Pointer_type : public Type {
	const Type* const _type;
	#if COMPILER
	llvm::Type* _llvm_type = nullptr;
	#endif
public:
	Pointer_type(const Type* type, bool native = false);
	virtual ~Pointer_type() {}
	virtual const Type* pointed() const override;
	virtual const Value* function() const override;
	virtual const Type* return_type() const override;
	virtual const Type* argument(size_t) const override;
	virtual const std::vector<const Type*>& arguments() const override;
	virtual const std::string getName() const override { return _type->getName() + "*"; }
	virtual const std::string getJsonName() const override { return _type->getJsonName() + "*"; }
	virtual bool callable() const override;
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