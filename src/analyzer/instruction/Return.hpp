#ifndef RETURN_HPP
#define RETURN_HPP

#include "Instruction.hpp"

namespace ls {

class Return : public Instruction {
public:

	Token* token = nullptr;
	std::unique_ptr<Value> expression;

	Return(Environment& env, Token* token, std::unique_ptr<Value> = nullptr);

	virtual void print(std::ostream&, int indent, PrintOptions options) const override;
	virtual Location location() const override;

	virtual void pre_analyze(SemanticAnalyzer* analyzer) override;
	virtual void analyze(SemanticAnalyzer*, const Type* req_type) override;

	virtual Hover hover(SemanticAnalyzer& analyzer, size_t position) const override;

	#if COMPILER
	virtual Compiler::value compile(Compiler&) const override;
	#endif

	virtual std::unique_ptr<Instruction> clone(Block* parent) const override;
};

}

#endif
