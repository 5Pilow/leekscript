#ifndef VARIABLEVALUE_HPP
#define VARIABLEVALUE_HPP

#include "../lexical/Ident.hpp"
#include "../value/LeftValue.hpp"
#include "../Compiler.hpp"

namespace ls {

class Value;
class SemanticVar;

class VariableValue : public LeftValue {
public:

	std::string name;
	Token* token;
	SemanticVar* var;

	VariableValue(Token* token);
	virtual ~VariableValue();

	virtual void print(std::ostream&, int indent, bool debug) const override;
	virtual unsigned line() const override;

	virtual void analyse(SemanticAnalyser*, const Type&) override;
	virtual bool will_take(SemanticAnalyser* analyser, const std::vector<Type>& args_type) override;
	void must_return(SemanticAnalyser* analyser, const Type& type) override;
	virtual void change_type(SemanticAnalyser*, const Type&) override;

	virtual jit_value_t compile(Compiler&) const override;
	virtual jit_value_t compile_l(Compiler&) const override;
};

}

#endif
