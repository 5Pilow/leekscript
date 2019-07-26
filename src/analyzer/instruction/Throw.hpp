#ifndef THROW_HPP
#define THROW_HPP

#include "Instruction.hpp"

namespace ls {

class Throw : public Instruction {
public:

	Token* token;
	std::unique_ptr<Value> expression;

	Throw(Environment& env, Token* token, std::unique_ptr<Value> = nullptr);

	virtual void print(std::ostream&, int indent, PrintOptions options) const override;
	virtual Location location() const override;

	virtual void pre_analyze(SemanticAnalyzer* analyzer) override;
	virtual void analyze(SemanticAnalyzer*, const Type* req_type) override;

	#if COMPILER
	virtual Compiler::value compile(Compiler&) const override;
	#endif

	virtual std::unique_ptr<Instruction> clone() const override;
};

}

#endif
