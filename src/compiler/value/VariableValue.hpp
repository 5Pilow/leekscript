#ifndef VARIABLEVALUE_HPP
#define VARIABLEVALUE_HPP

#include <memory>
#include "LeftValue.hpp"
#include "../semantic/SemanticAnalyser.hpp"
#include "../Compiler.hpp"

namespace ls {

class Value;
class SemanticVar;
enum class VarScope;

class VariableValue : public LeftValue {
public:

	std::string name;
	std::shared_ptr<Token> token;
	std::shared_ptr<SemanticVar> var;
	int capture_index = 0;
	VarScope scope;

	VariableValue(std::shared_ptr<Token> token);

	virtual bool isLeftValue() const override;

	virtual void print(std::ostream&, int indent, bool debug, bool condensed) const override;
	virtual Location location() const override;

	virtual Callable* get_callable(SemanticAnalyser*) const override;
	virtual void analyse(SemanticAnalyser*) override;
	virtual bool will_take(SemanticAnalyser* analyser, const std::vector<Type>&, int level) override;
	void set_version(const std::vector<Type>& args, int level) override;
	virtual bool will_store(SemanticAnalyser* analyser, const Type& type) override;
	virtual bool elements_will_store(SemanticAnalyser* analyser, const Type& type, int level) override;
	virtual void change_value(SemanticAnalyser*, Value* value) override;
	virtual Type version_type(std::vector<Type>) const override;

	virtual Compiler::value compile(Compiler&) const override;
	virtual Compiler::value compile_l(Compiler&) const override;

	virtual Value* clone() const override;
};

}

#endif
