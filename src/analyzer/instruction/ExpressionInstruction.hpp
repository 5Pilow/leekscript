#ifndef EXPRESSIONINSTRUCTION_HPP
#define EXPRESSIONINSTRUCTION_HPP

#include "Instruction.hpp"

namespace ls {

class ExpressionInstruction : public Instruction {
public:

	std::unique_ptr<Value> value;

	ExpressionInstruction(Environment& env, std::unique_ptr<Value> expression);

	virtual void print(std::ostream&, int indent, PrintOptions options) const override;
	virtual Location location() const override;

	virtual void set_end_section(Section*) override;

	virtual void pre_analyze(SemanticAnalyzer* analyzer) override;

	virtual void analyze(SemanticAnalyzer*, const Type* type) override;
	virtual std::vector<std::string> autocomplete(SemanticAnalyzer& analyzer, size_t position) const override;

	#if COMPILER
	virtual Compiler::value compile(Compiler&) const override;
	virtual Compiler::value compile_end(Compiler&) const override;
	#endif

	virtual std::unique_ptr<Instruction> clone(Block* parent) const override;
};

}

#endif
