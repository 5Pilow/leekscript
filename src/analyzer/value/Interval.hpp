#ifndef INTERVAL_HPP
#define INTERVAL_HPP

#include <vector>
#include <memory>
#include "Value.hpp"
#include "../lexical/Token.hpp"

namespace ls {

class Interval : public Value {
public:

	Token* opening_bracket;
	Token* closing_bracket;
	std::unique_ptr<Value> start;
	std::unique_ptr<Value> end;

	Interval();

	virtual void print(std::ostream&, int indent, bool debug, bool condensed) const override;
	virtual Location location() const override;

	virtual void pre_analyze(SemanticAnalyzer*) override;
	virtual void analyze(SemanticAnalyzer*) override;

	virtual Compiler::value compile(Compiler&) const override;

	virtual std::unique_ptr<Value> clone() const override;
};

}

#endif
