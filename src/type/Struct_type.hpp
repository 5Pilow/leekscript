#ifndef STRUCT_TYPE_HPP
#define STRUCT_TYPE_HPP

#include "Type.hpp"
#if COMPILER
#include "llvm/IR/DerivedTypes.h"
#endif

namespace ls {

class Struct_type : public Type {
	std::string _name;
	std::vector<const Type*> _types;
	#if COMPILER
	llvm::StructType* _llvm_type = nullptr;
	#endif
public:
	Struct_type(const std::string name, std::initializer_list<const Type*> types);
	Struct_type(const std::string name, std::vector<const Type*> types);
	virtual const std::string getName() const override { return "struct"; }
	virtual Json json() const override;
	virtual const Type* member(int) const override;
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