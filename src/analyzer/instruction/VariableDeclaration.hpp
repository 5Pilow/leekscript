#ifndef VARIABLEDECLARATION_HPP
#define VARIABLEDECLARATION_HPP

#include <vector>
#include "Instruction.hpp"
#include "../semantic/SemanticAnalyzer.hpp"

namespace ls {

class Variable;

class VariableDeclaration : public Instruction {
public:

	Token* keyword = nullptr;
	bool global = false;
	bool constant = false;
	bool function = false;
	std::vector<Token*> variables;
	std::vector<std::unique_ptr<Value>> expressions;
	std::map<std::string, Variable*> global_vars;
	std::map<std::string, Variable*> vars;

	VariableDeclaration(Environment& env);

	virtual void print(std::ostream&, int indent, PrintOptions options) const override;
	virtual Location location() const override;

	virtual void set_end_section(Section* end_section) override;

	void analyze_global_functions(SemanticAnalyzer* analyzer) const;
	virtual void pre_analyze(SemanticAnalyzer* analyzer) override;
	virtual void analyze(SemanticAnalyzer*, const Type* req_type) override;
	virtual Completion autocomplete(SemanticAnalyzer& analyzer, size_t position) const override;
	virtual Hover hover(SemanticAnalyzer& analyzer, size_t position) const override;

	#if COMPILER
	virtual Compiler::value compile(Compiler&) const override;
	#endif

	virtual std::unique_ptr<Instruction> clone(Block* parent) const override;
};

}

#endif
