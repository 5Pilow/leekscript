#include "Instruction.hpp"

namespace ls {

void Instruction::pre_analyze(SemanticAnalyzer*) {}

Compiler::value Instruction::compile_end(Compiler&) const {}

std::string Instruction::tabs(int indent) const {
	return std::string(indent * 4, ' ');
}

}
