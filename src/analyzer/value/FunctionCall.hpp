#ifndef FUNCTIONCALL_HPP
#define FUNCTIONCALL_HPP

#include <vector>
#include <memory>
#include "Value.hpp"
#include "../lexical/Token.hpp"
#include "Function.hpp"

namespace ls {

class Callable;
class CallableVersion;

class FunctionCall : public Value {
public:
	Token* token;
	std::unique_ptr<Value> function;
	std::vector<std::unique_ptr<Value>> arguments;
	Token* opening_parenthesis;
	Token* closing_parenthesis;
	bool is_native = false;
	const Type* return_type;
	void* std_func;
	Value* this_ptr;
	bool is_native_method = false;
	bool is_unknown_method = false;
	Value* object = nullptr;
	Function* function_object;
	const Type* function_type;
	std::vector<const Type*> arg_types;
	std::string function_name;
	Call call;
	const CallableVersion* callable_version;

	FunctionCall(Environment& env, Token* t);

	virtual void print(std::ostream&, int indent, PrintOptions options) const override;
	virtual Location location() const override;

	virtual void pre_analyze(SemanticAnalyzer*) override;
	virtual Call get_callable(SemanticAnalyzer*, int argument_count) const override;
	const Type* will_take(SemanticAnalyzer*, const std::vector<const Type*>& args, int level) override;
	void set_version(SemanticAnalyzer*, const std::vector<const Type*>& args, int level) override;
	virtual const Type* version_type(std::vector<const Type*>) const override;
	virtual void analyze(SemanticAnalyzer*) override;
	virtual std::vector<Completion> autocomplete(SemanticAnalyzer& analyzer, size_t position) const override;
	virtual Hover hover(SemanticAnalyzer& analyzer, size_t position) const override;

	#if COMPILER
	virtual Compiler::value compile(Compiler&) const override;
	#endif

	virtual std::unique_ptr<Value> clone(Block* parent) const override;
};

}

#endif
