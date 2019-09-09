#include "Phi.hpp"
#include "../value/Block.hpp"
#include "../semantic/Variable.hpp"
#include "../semantic/SemanticAnalyzer.hpp"
#include "../semantic/FunctionVersion.hpp"

namespace ls {

Phi::Phi(Variable* variable, Block* block1, Variable* variable1, Block* block2, Variable* variable2) : variable(variable), block1(block1), variable1(variable1), block2(block2), variable2(variable2) {}

Phi* create_phi(SemanticAnalyzer* analyzer, Block* block1, Variable* variable1, Block* block2, Variable* variable2) {
	if (variable1->parent == variable2->parent or variable1 == variable2->parent or variable2 == variable1->parent) {
		auto parent = variable1 == variable2->parent ? variable1 : variable1->parent;
		auto new_var = analyzer->update_var(parent);
		auto phi = new Phi { new_var, block1, variable1, block2, variable2 };
		variable1->phi = phi;
		variable2->phi = phi;
		return phi;
	}
	return nullptr;
}

std::vector<Phi*> Phi::build_phis(SemanticAnalyzer* analyzer, Block* block1, Block* block2) {
	std::vector<Phi*> phis;
	for (const auto& variable : block1->variables) {
		auto i = block2->variables.find(variable.second->name);
		if (i != block2->variables.end()) {
			if (auto phi = create_phi(analyzer, block1, variable.second, block2, i->second)) {
				phis.push_back(phi);
			}
		} else if (block1 != analyzer->current_block()) {
			i = analyzer->current_block()->variables.find(variable.second->name);
			if (i != analyzer->current_block()->variables.end()) {
				if (auto phi = create_phi(analyzer, block2, i->second, block1, variable.second)) {
					phis.push_back(phi);
				}
			}
		}
	}
	for (const auto& variable : block2->variables) {
		auto i = block1->variables.find(variable.second->name);
		if (i == block1->variables.end()) {
			// std::cout << "variable " << variable.second << " not in block 1" << std::endl;
			if (auto v = analyzer->get_var(variable.second->name)) {
				if (auto phi = create_phi(analyzer, block1, v, block2, variable.second)) {
					phis.push_back(phi);
				}
			}
		}
	}
	return phis;
}

}