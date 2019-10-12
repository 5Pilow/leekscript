#ifndef BLOCK_HPP
#define BLOCK_HPP

#include <unordered_map>
#include <vector>
#include "../instruction/Instruction.hpp"
#include "Value.hpp"
#include "../semantic/Section.hpp"

namespace ls {

class Variable;

class Block : public Value {
public:

	std::vector<Section*> sections;
	bool is_function_block = false;
	bool temporary_mpz = false;
	bool mpz_pointer = false;
	bool was_reference = false;
	std::unordered_map<std::string, Variable*> variables;
	Block* branch = nullptr;
	std::vector<std::pair<Variable*, Variable*>> assignments;
	std::vector<Variable*> temporary_variables;
	std::vector<Variable*> mutations;
	#if COMPILER
	std::vector<Compiler::value> temporary_values;
	std::vector<Compiler::value> temporary_expression_values;
	Compiler::value return_value;
	#endif

	Block(Environment& env, bool is_function_block = false, bool init_first_section = true);

	virtual void print(std::ostream&, int indent, PrintOptions options) const override;
	virtual Location location() const override;

	void add_instruction(Instruction* instruction);
	void add_instruction(std::unique_ptr<Instruction> instruction);
	virtual void set_end_section(Section* end_section) override;

	void analyze_global_functions(SemanticAnalyzer* analyzer);
	void setup_branch(SemanticAnalyzer* analyzer);
	virtual void pre_analyze(SemanticAnalyzer* analyzer) override;
	void create_assignments(SemanticAnalyzer* analyzer);
	virtual void analyze(SemanticAnalyzer* analyzer) override;

	#if COMPILER
	Compiler::value compile(Compiler&) const override;
	void compile_end(Compiler&) const override;
	#endif

	virtual std::unique_ptr<Value> clone() const override;
};

}

#endif
