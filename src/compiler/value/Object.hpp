#ifndef OBJECT_HPP
#define OBJECT_HPP

#include <vector>

#include "../../compiler/value/Expression.hpp"
#include "../lexical/Ident.hpp"

namespace ls {

class Object : public Value {
public:

	std::vector<std::unique_ptr<Token>> keys;
	std::vector<Value*> values;

	Object();
	virtual ~Object();

	virtual void print(std::ostream&, int indent, bool debug) const override;

	virtual void analyse(SemanticAnalyser*, const Type&) override;

	virtual Compiler::value compile(Compiler&) const override;
};

}

#endif
