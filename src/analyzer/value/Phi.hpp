#ifndef PHI_HPP
#define PHI_HPP

#include <vector>
#include "../../compiler/Compiler.hpp"

namespace ls {

class Variable;
class Block;
class SemanticAnalyzer;

class Phi {
public:
	Variable* variable;
	Block* block1;
	Variable* variable1;
	Compiler::value value1;
	Block* block2;
	Variable* variable2;
	Compiler::value value2;

	Phi(Variable* variable, Block* block1, Variable* value1, Block* block2, Variable* value2);

	static std::vector<Phi*> build_phis(SemanticAnalyzer* analyzer, Block* block1, Block* block2);
};

}

#endif