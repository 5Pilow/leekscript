#ifndef INSTRUCTION_HPP
#define INSTRUCTION_HPP

#include <ostream>
#include <memory>
#include "../../constants.h"
#include "../lexical/Token.hpp"
#include "../../type/Type.hpp"
#include "../PrintOptions.hpp"
#include "../semantic/Completion.hpp"
#include "../semantic/Hover.hpp"
#if COMPILER
#include "../../compiler/Compiler.hpp"
#endif

namespace ls {

class Value;
class SemanticAnalyzer;
class Environment;
class Section;
class Block;

class Instruction {
public:

	const Type* type = nullptr;
	const Type* return_type = nullptr;
	bool returning = false;
	bool may_return = false;
	bool is_void = false;
	bool throws = false;
	bool jumping = false; // Indicates the instruction contains a jump
	bool jump_to_existing_section = false;
	bool breaking = false; // Break or continue instruction
	Section* end_section = nullptr;
	Section* continue_section = nullptr;

	Instruction(Environment& env);
	virtual ~Instruction() {}

	virtual void print(std::ostream&, int indent, PrintOptions options) const = 0;
	virtual Location location() const = 0;

	virtual void set_end_section(Section* end_section);

	virtual void pre_analyze(SemanticAnalyzer* analyzer);

	virtual void analyze(SemanticAnalyzer* analyzer);
	virtual void analyze(SemanticAnalyzer* analyzer, const Type* type) = 0;
	virtual Completion autocomplete(SemanticAnalyzer& analyzer, size_t position) const;
	virtual Hover hover(SemanticAnalyzer& analyzer, size_t position) const;

	#if COMPILER
	virtual Compiler::value compile(Compiler&) const = 0;
	virtual Compiler::value compile_end(Compiler&) const;
	#endif

	virtual std::unique_ptr<Instruction> clone(Block* parent) const = 0;

	std::string tabs(int indent) const;
};

}

#endif
