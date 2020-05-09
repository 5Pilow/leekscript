#include "Phi.hpp"
#include "../value/Block.hpp"
#include "../semantic/Variable.hpp"
#include "../semantic/SemanticAnalyzer.hpp"
#include "../semantic/FunctionVersion.hpp"

namespace ls {

Phi::Phi(Environment& env, Variable* variable, Block* block1, Variable* variable1, Block* block2, Variable* variable2) : variable(variable), block1(block1), variable1(variable1), block2(block2)
, variable2(variable2)
#if COMPILER
, value1(env), value2(env), phi_node(env)
#endif
{}

Phi::Phi(Environment& env, Variable* variable, Section* section1, Variable* variable1, Section* section2, Variable* variable2) : variable(variable), section1(section1), variable1(variable1), section2(section2)
, variable2(variable2)
#if COMPILER
, value1(env), value2(env), phi_node(env)
#endif
{}

}