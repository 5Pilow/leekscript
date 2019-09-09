#ifndef IF_HPP_
#define IF_HPP_

#include "Expression.hpp"
#include "../value/Block.hpp"

namespace ls {

class Phi;

class If : public Value {
public:

	std::unique_ptr<Value> condition;
	std::unique_ptr<Block> then;
	std::unique_ptr<Block> elze;
	bool ternary;
	std::vector<Phi*> phis;

	If(bool ternary = false);

	virtual void print(std::ostream&, int indent, bool debug, bool condensed) const override;
	virtual Location location() const override;

	virtual void pre_analyze(SemanticAnalyzer*) override;
	virtual void analyze(SemanticAnalyzer*) override;

	virtual Compiler::value compile(Compiler&) const override;

	virtual std::unique_ptr<Value> clone() const override;
};

}

#endif
