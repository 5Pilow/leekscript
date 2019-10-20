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

	AbsoluteValue(Environment& env);

	virtual void print(std::ostream&, int indent, PrintOptions options) const override;
	virtual Location location() const override;

	virtual void pre_analyze(SemanticAnalyzer*) override;
	virtual void analyze(SemanticAnalyzer*) override;
	virtual Json hover(SemanticAnalyzer& analyzer, size_t position) const override;

	#if COMPILER
	virtual Compiler::value compile(Compiler&) const override;
	#endif

	virtual std::unique_ptr<Value> clone(Block* parent) const override;
};

}

#endif
