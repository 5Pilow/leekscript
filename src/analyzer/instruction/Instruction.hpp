#ifndef INSTRUCTION_HPP
#define INSTRUCTION_HPP

#include <ostream>
#include "../Compiler.hpp"
#include "../lexical/Token.hpp"
#include "../../type/Type.hpp"

namespace ls {

class Value;
class SemanticAnalyzer;

class Instruction {
public:

	const Type* type = Type::void_;
	const Type* return_type = Type::void_;
	bool returning = false;
	bool may_return = false;
	bool is_void = false;
	bool throws = false;

	virtual ~Instruction() {}

	virtual void print(std::ostream&, int indent, bool debug, bool condensed) const = 0;
	virtual Location location() const = 0;

	virtual void pre_analyze(SemanticAnalyzer* analyzer);
	virtual void analyze(SemanticAnalyzer* analyzer, const Type* type = Type::any) = 0;

	virtual Compiler::value compile(Compiler&) const = 0;
	virtual Compiler::value compile_end(Compiler&) const;

	virtual std::unique_ptr<Instruction> clone() const = 0;

	std::string tabs(int indent) const;
};

}

#endif
