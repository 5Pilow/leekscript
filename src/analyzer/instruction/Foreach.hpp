#ifndef FOREACH_HPP
#define FOREACH_HPP

#include <vector>
#include "../value/Value.hpp"
#include "../value/Block.hpp"
#include "../semantic/SemanticAnalyzer.hpp"
#include "../semantic/Mutation.hpp"

namespace ls {

class Foreach : public Instruction {
public:
	std::unique_ptr<Block> wrapper_block;
	Token* key = nullptr;
	Token* value = nullptr;
	std::unique_ptr<Value> container;
	std::unique_ptr<Block> body;
	std::vector<Mutation> mutations;
	std::vector<std::tuple<Variable*, Variable*, const Section*>> conversions;
	Section* condition_section = nullptr;
	Section* increment_section = nullptr;

	const Type* key_type;
	const Type* value_type;
	Variable* value_var;
	Variable* key_var;

	Foreach(Environment& env);

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
