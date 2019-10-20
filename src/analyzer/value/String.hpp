#ifndef STRING_HPP_
#define STRING_HPP_

#include "Value.hpp"

namespace ls {

class Token;

class String : public Value {
public:

	Token* token;

	String(Environment& env, Token* token);

	virtual void print(std::ostream&, int indent, PrintOptions options) const override;
	virtual Location location() const override;

	virtual bool will_store(SemanticAnalyzer* analyzer, const Type* type) override;
	virtual Json hover(SemanticAnalyzer& analyzer, size_t position) const override;

	#if COMPILER
	virtual Compiler::value compile(Compiler&) const override;
	#endif

	virtual std::unique_ptr<Value> clone(Block* parent) const override;
};

}

#endif
