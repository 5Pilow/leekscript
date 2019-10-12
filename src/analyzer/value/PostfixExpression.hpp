#ifndef POSTFIXEXPRESSION_HPP
#define POSTFIXEXPRESSION_HPP

#include "Expression.hpp"
#include "LeftValue.hpp"
#include "Value.hpp"

namespace ls {

class PostfixExpression : public Value {
public:

	std::unique_ptr<LeftValue> expression;
	std::shared_ptr<Operator> operatorr;
	bool return_value;

	PostfixExpression(Environment& env);

	virtual void print(std::ostream&, int indent, PrintOptions options) const override;
	virtual Location location() const override;

	virtual void pre_analyze(SemanticAnalyzer*) override;
	virtual void analyze(SemanticAnalyzer*) override;

	#if COMPILER
	virtual Compiler::value compile(Compiler&) const override;
	#endif

	virtual std::unique_ptr<Value> clone(Block* parent) const override;
};

}

#endif
