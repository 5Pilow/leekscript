#ifndef WHILE_HPP
#define WHILE_HPP

#include <vector>
#include "../../compiler/value/Value.hpp"
#include "../value/Block.hpp"
class Variable;

namespace ls {

class While : public Instruction {
public:

	Token* token;
	std::unique_ptr<Value> condition;
	std::unique_ptr<Block> body;
	std::unique_ptr<Block> body2;
	bool body2_activated = false;
	std::vector<std::pair<Variable*, Variable*>> assignments;
	std::vector<Variable*> mutations;

	While();

	virtual void print(std::ostream&, int indent, bool debug, bool condensed) const override;
	virtual Location location() const override;

	virtual void pre_analyze(SemanticAnalyzer* analyzer) override;
	virtual void analyze(SemanticAnalyzer*, const Type* req_type) override;

	virtual Compiler::value compile(Compiler&) const override;

	virtual std::unique_ptr<Instruction> clone() const override;
};

}

#endif
