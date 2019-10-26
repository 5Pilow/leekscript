#ifndef NULLL_HPP
#define NULLL_HPP

#include <memory>
#include "Value.hpp"
#include "../lexical/Token.hpp"

namespace ls {

class Nulll : public Value {
public:

	Token* token;

	Nulll(Environment& env, Token* token);

	virtual void print(std::ostream&, int indent, PrintOptions options) const override;
	virtual Location location() const override;

	#if COMPILER
	virtual Compiler::value compile(Compiler&) const override;
	#endif

	virtual std::unique_ptr<Value> clone(Block* parent) const override;
};

}

#endif
