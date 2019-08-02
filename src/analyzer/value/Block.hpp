#ifndef BLOCK_HPP
#define BLOCK_HPP

#include <unordered_map>
#include <vector>
#include "../instruction/Instruction.hpp"
#include "Value.hpp"

namespace ls {

class Variable;

class Block : public Value {
public:

	bool is_function_block = false;
	bool is_loop_body = false;
	bool is_loop = false;
	std::vector<std::unique_ptr<Instruction>> instructions;
	bool temporary_mpz = false;
	bool mpz_pointer = false;
	bool was_reference = false;
	std::unordered_map<std::string, Variable*> variables;
	Block* branch = nullptr;
	std::vector<std::pair<Variable*, Variable*>> assignments;
	std::vector<Variable*> temporary_variables;
	std::vector<Variable*> mutations;
	bool enabled = true;
	#if COMPILER
	std::vector<llvm::BasicBlock*> blocks;
	std::vector<Compiler::value> temporary_values;
	std::vector<Compiler::value> temporary_expression_values;
	#endif

	Block(Environment& env, bool is_function_block = false);

	virtual void print(std::ostream&, int indent, PrintOptions options) const override;
	virtual Location location() const override;

	void analyze_global_functions(SemanticAnalyzer* analyzer);
	void setup_branch(SemanticAnalyzer* analyzer);
	virtual void pre_analyze(SemanticAnalyzer* analyzer) override;
	void create_assignments(SemanticAnalyzer* analyzer);
	virtual void analyze(SemanticAnalyzer* analyzer) override;

	#if COMPILER
	Compiler::value compile(Compiler&) const override;
	#endif

	virtual std::unique_ptr<Value> clone() const override;
};

}

#endif
