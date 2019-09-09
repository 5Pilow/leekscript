#ifndef FOREACH_HPP
#define FOREACH_HPP

#include <vector>
#include "../value/Value.hpp"
#include "../value/Block.hpp"
#include "../semantic/SemanticAnalyzer.hpp"

namespace ls {

class Foreach : public Instruction {
public:
	std::unique_ptr<Block> wrapper_block;
	Token* key = nullptr;
	Token* value = nullptr;
	std::unique_ptr<Value> container;
	std::unique_ptr<Block> body;
	std::unique_ptr<Block> body2;
	bool body2_activated = false;
	std::vector<std::pair<Variable*, Variable*>> assignments;
	std::vector<Variable*> mutations;
	
	const Type* key_type;
	const Type* value_type;
	Variable* value_var;
	Variable* key_var;

	Foreach();

	virtual void print(std::ostream&, int indent, bool debug, bool condensed) const override;
	virtual Location location() const override;

	virtual void pre_analyze(SemanticAnalyzer* analyzer) override;
	virtual void analyze(SemanticAnalyzer*, const Type* req_type) override;

	virtual Compiler::value compile(Compiler&) const override;

	virtual std::unique_ptr<Instruction> clone() const override;
};

}

#endif
