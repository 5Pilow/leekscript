#ifndef BREAK_HPP
#define BREAK_HPP

#include <memory>
#include "Instruction.hpp"
#include "../lexical/Token.hpp"

namespace ls {

class Break : public Instruction {
public:

	Token* token;
	int deepness;

	Break();

	virtual void print(std::ostream&, int indent, bool debug, bool condensed) const override;
	virtual Location location() const override;

	virtual void analyze(SemanticAnalyzer*, const Type* req_type) override;

	virtual Compiler::value compile(Compiler&) const override;

	virtual std::unique_ptr<Instruction> clone() const override;
};

}

#endif
