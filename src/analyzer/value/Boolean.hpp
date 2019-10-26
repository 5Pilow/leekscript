#ifndef BOOLEAN_HPP
#define BOOLEAN_HPP

#include <memory>
#include "Value.hpp"
#include "../lexical/Token.hpp"
#include "../semantic/SemanticAnalyzer.hpp"

namespace ls {

class Boolean : public Value {
public:

	Token* token;
	bool value;

	Boolean(Environment& env, Token* token);

	virtual void print(std::ostream&, int indent, PrintOptions options) const override;
	virtual Location location() const override;

	#if COMPILER
	virtual Compiler::value compile(Compiler&) const override;
	#endif

	virtual std::unique_ptr<Value> clone(Block* parent) const override;
};

}

#endif
