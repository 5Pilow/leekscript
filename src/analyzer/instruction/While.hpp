#ifndef WHILE_HPP
#define WHILE_HPP

#include <vector>
#include "../value/Value.hpp"
#include "../value/Block.hpp"
#include "../semantic/Mutation.hpp"

namespace ls {

class Variable;

class While : public Instruction {
public:

	Token* token;
	std::unique_ptr<Block> condition;
	std::unique_ptr<Block> body;
	std::vector<std::tuple<Variable*, Variable*, const Section*>> conversions;
	std::vector<Mutation> mutations;

	While(Environment& env);

	virtual void print(std::ostream&, int indent, PrintOptions options) const override;
	virtual Location location() const override;

	virtual void pre_analyze(SemanticAnalyzer* analyzer) override;
	virtual void analyze(SemanticAnalyzer*, const Type* req_type) override;

	#if COMPILER
	virtual Compiler::value compile(Compiler&) const override;
	#endif

	virtual std::unique_ptr<Instruction> clone(Block* parent) const override;
};

}

#endif
