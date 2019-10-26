#include "Instruction.hpp"
#include "../semantic/SemanticAnalyzer.hpp"

namespace ls {

Instruction::Instruction(Environment& env) : type(env.void_), return_type(env.void_) {}

void Instruction::set_end_section(Section*) {}

void Instruction::pre_analyze(SemanticAnalyzer*) {}

void Instruction::analyze(SemanticAnalyzer* analyzer) {
	analyze(analyzer, analyzer->env.any);
}

std::vector<Completion> Instruction::autocomplete(SemanticAnalyzer& analyzer, size_t position) const {
	return {};
}

Hover Instruction::hover(SemanticAnalyzer& analyzer, size_t position) const {
	return { type, location() };
}

#if COMPILER
Compiler::value Instruction::compile_end(Compiler& c) const {
	return { c.env };
}
#endif

std::string Instruction::tabs(int indent) const {
	return std::string(indent * 4, ' ');
}

}
