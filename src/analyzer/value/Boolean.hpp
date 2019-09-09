#ifndef BOOLEAN_HPP
#define BOOLEAN_HPP

#include <memory>
#include "Value.hpp"
#include "../lexical/Token.hpp"

namespace ls {

class Boolean : public Value {
public:

	Token* token;
	bool value;

	Boolean(Token* token);

	virtual void print(std::ostream&, int indent, bool debug, bool condensed) const override;
	virtual Location location() const override;

	virtual Compiler::value compile(Compiler&) const override;

	virtual std::unique_ptr<Value> clone() const override;
};

}

#endif
