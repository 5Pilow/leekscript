#ifndef FUNCTION_TYPE_HPP
#define FUNCTION_TYPE_HPP

#include "Type.hpp"

namespace ls {

class Type;
class Value;

class Function_type : public Type {
	#if COMPILER
	llvm::Type* llvm_type = nullptr;
	#endif
	const Type* const _return_type;
	std::vector<const Type*> _arguments;
	const Value* _function;
public:
	bool closure() const { return false; }
	Function_type(const Type*, const std::vector<const Type*>&, const Value* function = nullptr);
	const Value* function() const override { return _function; }
	virtual int id() const override { return 9; }
	virtual const std::string getName() const override { return "function"; }
	virtual Json json() const override;
	virtual bool callable() const override { return true; }
	virtual bool operator == (const Type*) const override;
	virtual int distance(const Type* type) const override;
	virtual const Type* return_type() const override;
	virtual const std::vector<const Type*>& arguments() const override;
	virtual const Type* argument(size_t) const override;
	#if COMPILER
	virtual llvm::Type* llvm(Compiler& c) const override;
	#endif
	virtual std::string class_name() const override;
	virtual std::ostream& print(std::ostream& os) const override;
	virtual Type* clone() const override;
};

}

#endif