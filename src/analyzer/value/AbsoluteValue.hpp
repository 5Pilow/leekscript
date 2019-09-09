#ifndef ABSOLUTEVALUE_HPP
#define ABSOLUTEVALUE_HPP

#include <memory>
#include "Value.hpp"
#include "../lexical/Token.hpp"

namespace ls {

class AbsoluteValue : public Value {
public:

	std::unique_ptr<Value> expression;
	Token* open_pipe;
	Token* close_pipe;

	AbsoluteValue();

	virtual void print(std::ostream&, int indent, bool debug, bool condensed) const override;
	virtual Location location() const override;

	virtual void pre_analyze(SemanticAnalyzer*) override;
	virtual void analyze(SemanticAnalyzer*) override;

	virtual Compiler::value compile(Compiler&) const override;

	virtual std::unique_ptr<Value> clone() const override;
};

}

#endif
