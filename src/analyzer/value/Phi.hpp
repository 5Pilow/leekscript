#ifndef PHI_HPP
#define PHI_HPP

#include <vector>
#include "../../constants.h"
#if COMPILER
#include "../../compiler/Compiler.hpp"
#endif

namespace ls {

class Variable;
class Block;
class SemanticAnalyzer;
class Environment;
class Section;

class Phi {
public:
	Variable* variable;
	Block* block1;
	Section* section1;
	Variable* variable1;
	Block* block2;
	Section* section2;
	Variable* variable2;
	bool active = true;
	#if COMPILER
	Compiler::value value1;
	Compiler::value value2;
	Compiler::value phi_node;
	#endif

	Phi(Environment& env, Variable* variable, Block* block1, Variable* value1, Block* block2, Variable* value2);
	Phi(Environment& env, Variable* variable, Section* section1, Variable* value1, Section* section2, Variable* value2);
};

}

#endif