#ifndef EXPRESSIONINSTRUCTION_HPP
#define EXPRESSIONINSTRUCTION_HPP

#include "Instruction.hpp"

namespace ls {

class ExpressionInstruction : public Instruction {
public:

	std::unique_ptr<Value> value;

	ExpressionInstruction(std::unique_ptr<Value> expression);

	virtual void print(std::ostream&, int indent, bool debug, bool condensed) const override;
	virtual Location location() const override;

	virtual void pre_analyze(SemanticAnalyzer* analyzer) override;
	virtual void analyze(SemanticAnalyzer*, const Type* type) override;

	virtual Compiler::value compile(Compiler&) const override;
	virtual Compiler::value compile_end(Compiler&) const override;

	virtual std::unique_ptr<Instruction> clone() const override;
};

}

#endif
