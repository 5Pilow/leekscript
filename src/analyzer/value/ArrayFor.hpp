#ifndef ARRAYFOR_HPP
#define ARRAYFOR_HPP

#include "Value.hpp"
#include "../instruction/For.hpp"

namespace ls {

class ArrayFor : public Value {
public:
	std::unique_ptr<Instruction> forr;

	ArrayFor(Environment& env);

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
#endif // ARRAYFOR_H
