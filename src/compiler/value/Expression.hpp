#ifndef EXPRESSION_HPP
#define EXPRESSION_HPP

#include <vector>

#include "../../compiler/lexical/Operator.hpp"
#include "../../compiler/value/Value.hpp"

namespace ls {

class Expression : public Value {
public:

	Value* v1;
	Value* v2;
	Operator* op;

	bool ignorev2;
	bool no_op;
	int operations;
	bool reference_v2;

	Expression();
	Expression(Value*);
	virtual ~Expression();

	void append(Operator*, Value*, bool ref_v2);

	void print(std::ostream&, int indent, bool debug) const override;
	virtual unsigned line() const override;

	virtual void analyse(SemanticAnalyser*, const Type&) override;

	virtual jit_value_t compile(Compiler&) const override;
};

}

#endif
