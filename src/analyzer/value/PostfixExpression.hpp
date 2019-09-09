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

	PostfixExpression();

	virtual void print(std::ostream&, int indent, bool debug, bool condensed) const override;
	virtual Location location() const override;

	virtual void pre_analyze(SemanticAnalyzer*) override;
	virtual void analyze(SemanticAnalyzer*) override;

	virtual Compiler::value compile(Compiler&) const override;

	virtual std::unique_ptr<Value> clone() const override;
};

}

#endif
