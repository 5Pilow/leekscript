#ifndef VARIABLEDECLARATION_HPP
#define VARIABLEDECLARATION_HPP

#include <vector>
#include "Instruction.hpp"
#include "../semantic/SemanticAnalyzer.hpp"

namespace ls {

class Variable;

class VariableDeclaration : public Instruction {
public:

	Token* keyword;
	bool global;
	bool constant;
	bool function;
	std::vector<Token*> variables;
	std::vector<std::unique_ptr<Value>> expressions;
	std::map<std::string, Variable*> vars;

	VariableDeclaration();

	virtual void print(std::ostream&, int indent, bool debug, bool condensed) const override;
	virtual Location location() const override;

	void analyze_global_functions(SemanticAnalyzer* analyzer) const;
	virtual void pre_analyze(SemanticAnalyzer* analyzer) override;
	virtual void analyze(SemanticAnalyzer*, const Type* req_type) override;

	virtual Compiler::value compile(Compiler&) const override;

	virtual std::unique_ptr<Instruction> clone() const override;
};

}

#endif
