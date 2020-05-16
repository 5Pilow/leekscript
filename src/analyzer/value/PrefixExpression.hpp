#ifndef PREFIXEXPRESSION_HPP
#define PREFIXEXPRESSION_HPP

#include "Expression.hpp"
#include "Value.hpp"
#include "../lexical/Operator.hpp"

namespace ls {

class PrefixExpression : public Value {
public:

	std::shared_ptr<Operator> operatorr;
	std::unique_ptr<Value> expression;

	PrefixExpression(Environment& env, std::shared_ptr<Operator> op, std::unique_ptr<Value> expression);

	virtual void print(std::ostream&, int indent, PrintOptions options) const override;
	virtual Location location() const override;

	virtual void pre_analyze(SemanticAnalyzer*) override;
	virtual void analyze(SemanticAnalyzer*) override;
	virtual Hover hover(SemanticAnalyzer& analyzer, size_t position) const override;

	#if COMPILER
	virtual Compiler::value compile(Compiler&) const override;
	#endif

	virtual std::unique_ptr<Value> clone(Block* parent) const override;
};

}

#endif
