#ifndef FUNCTION_HPP
#define FUNCTION_HPP

#include <vector>
#include "../lexical/Ident.hpp"
#include "Value.hpp"
#include "Block.hpp"

namespace ls {

class Variable;
class SemanticAnalyzer;
class FunctionVersion;

class Function : public Value {
public:

	static int id_counter;

	std::string name;
	std::string internal_name;
	bool lambda = false;
	Token* token;
	std::vector<Token*> arguments;
	std::vector<std::unique_ptr<Value>> defaultValues;
	Block* body;

	bool function_added;
	Function* parent;
	bool is_main_function = false;
	FunctionVersion* default_version = nullptr;
	std::map<std::vector<const Type*>, FunctionVersion*> versions;
	bool generate_default_version = false;
	FunctionVersion* current_version = nullptr;
	bool analyzed = false;
	Compiler* compiler = nullptr;
	int default_values_count = 0;
	std::vector<Variable*> captures;
	std::unordered_map<std::string, Variable*> captures_map;
	bool captures_compiled = false;

	Function(Token* token);
	virtual ~Function();

	void addArgument(Token* token, Value* defaultValue);

	virtual void print(std::ostream&, int indent, bool debug, bool condensed) const override;
	virtual Location location() const override;

	virtual void pre_analyze(SemanticAnalyzer*) override;
	void create_default_version(SemanticAnalyzer* analyzer);
	void analyse_default_method(SemanticAnalyzer* analyzer);
	const Type* will_take(SemanticAnalyzer*, const std::vector<const Type*>&, int level) override;
	void set_version(SemanticAnalyzer* analyser, const std::vector<const Type*>& args, int level) override;
	virtual const Type* version_type(std::vector<const Type*>) const override;
	virtual void must_return_any(SemanticAnalyzer*) override;
	virtual Call get_callable(SemanticAnalyzer*, int argument_count) const override;
	virtual void analyze(SemanticAnalyzer*) override;
	void create_version(SemanticAnalyzer* analyzer, const std::vector<const Type*>& args);

	virtual Compiler::value compile(Compiler&) const override;
	virtual Compiler::value compile_version(Compiler&, std::vector<const Type*>) const override;
	Compiler::value compile_default_version(Compiler&) const;
	void compile_captures(Compiler& c) const;
	void export_context(const Compiler& c) const;

	virtual std::unique_ptr<Value> clone() const override;
};

}

#endif
