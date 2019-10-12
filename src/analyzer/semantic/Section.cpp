#include "Section.hpp"
#include "../../colors.h"
#include "Variable.hpp"
#include "SemanticAnalyzer.hpp"
#include "../value/Phi.hpp"
#include "../value/Block.hpp"

namespace ls {

const std::vector<std::string> Section::COLORS = {BLUE_BOLD, C_RED, C_YELLOW, C_GREEN, C_PURPLE};
size_t Section::current_id = 0;

Section::Section(Environment& env, std::string name) : name(name), condition(env) {
	id = current_id++;
	if (id >= COLORS.size()) {
		color = C_PURPLE;
	} else {
		color = COLORS[id];
	}
}
Section::Section(Environment& env, std::string name, Section* predecessor) : Section(env, name) {
	predecessors.push_back(predecessor);
}

void Section::add_successor(Section* successor) {
	successors.push_back(successor);
}

void Section::add_predecessor(Section* predecessor) {
	predecessors.push_back(predecessor);
}

std::string tabs(int indent) {
	return std::string(indent * 4, ' ');
}

void Section::print(std::ostream& os, int indent, PrintOptions options) const {
	os << color << "┃ " << id << END_COLOR << C_GREY " pred: <";
	int i = 0;
	for (const auto& predecessor : predecessors) {
		if (i++ > 0) os << C_GREY << ", " << END_COLOR;
		os << predecessor->color << predecessor->id << END_COLOR;
	}
	os << C_GREY << "> vars: [" << END_COLOR;
	i = 0;
	for (const auto& variable : variables) {
		if (i++ > 0) os << C_GREY << ", " << END_COLOR;
		os << variable.second;
	}
	os << C_GREY << "]" << END_COLOR << C_GREY " succ: <";
	i = 0;
	for (const auto& successors : successors) {
		if (i++ > 0) os << C_GREY << ", " << END_COLOR;
		os << successors->color << successors->id << END_COLOR;
	}
	os << C_GREY << ">" << std::endl;

	for (const auto& phi : phis) {
		os << color << "┃" << END_COLOR << tabs(indent + 1) << phi->variable << " = φ(" << phi->variable1 << " " << phi->variable1->type << " " << phi->section1->color << phi->section1->id << END_COLOR << ", " << phi->variable2 << " " << phi->variable2->type << " " << phi->section2->color << phi->section2->id << END_COLOR << ") " << phi->variable->type << std::endl;
	}

	for (const auto& assignment : assignments) {
		os << color << "┃" << END_COLOR << tabs(indent + 1) << assignment.first << " " << assignment.first->type << " = " << assignment.second << " " << assignment.second->type << std::endl;
	}

	for (auto& instruction : instructions) {
		if (!options.condensed) {
			os << color << "┃" << END_COLOR;
			os << tabs(indent + 1);
		}
		instruction->print(os, indent + 1, options);
		if (!options.condensed) os << std::endl;
	}
}

void Section::pre_analyze(SemanticAnalyzer* analyzer) {
	analyzer->enter_section(this);
	// Build phis
	auto search_build_phis = [&](Section* section1, Variable* variable, Section* section2, Section* section) {
		if (section1 == section) return true;
		auto i = section->variables.find(variable->name);
		if (i == section->variables.end()) return false;
		auto variable2 = i->second;
		auto parent = variable == variable2->parent ? variable : variable->parent;
		auto new_var = analyzer->update_var(parent);
		auto phi = new Phi { analyzer->env, new_var, section1, variable, section2, variable2 };
		variable->phis.push_back(phi);
		variable2->phis.push_back(phi);
		phis.push_back(phi);
		return true;
	};
	if (predecessors.size() == 2) {
		const auto& section1 = predecessors[0];
		const auto& section2 = predecessors[1];
		for (const auto& variable : section1->variables) {
			auto variable1 = variable.second;
			auto current = section2;
			while (current and not search_build_phis(section1, variable1, section2, current)) {
				current = current->predecessors.size() ? current->predecessors[0] : nullptr;
			}
		}
		for (const auto& variable : section2->variables) {
			auto variable2 = variable.second;
			// A phi already exist with this variable?
			auto var = variables.find(variable2->name);
			if (var != variables.end()) continue;
			auto current = section1;
			while (current and not search_build_phis(section2, variable2, section1, current)) {
				current = current->predecessors.size() ? current->predecessors[0] : nullptr;
			}
		}
	} else if (predecessors.size() == 1) {
		const auto& section1 = predecessors[0];
		for (const auto& variable : section1->variables) {
			auto variable1 = variable.second;
			if (variable1->root and variable1->root->section != section1) { // Variable not created in the predecessor
				auto new_var = analyzer->update_var(variable1->parent);
				variable.second->assignment = true;
				assignments.push_back({ new_var, variable.second });
			}
		}
	}
	// Analyze instructions
	for (const auto& instruction : instructions) {
		instruction->pre_analyze(analyzer);
	}
	analyzer->leave_section();
}

void Section::analyze(SemanticAnalyzer* analyzer) {
	for (const auto& phi : phis) {
		phi->variable->type = phi->variable1->type->operator + (phi->variable2->type);
	}
	for (const auto& assignment : assignments) {
		assignment.first->type = assignment.second->type;
	}
}

#if COMPILER

void Section::pre_compile(Compiler& c) {
	if (not basic_block) {
		basic_block = llvm::BasicBlock::Create(c.getContext(), name);
		first_basic_block = basic_block;
	}
}

Compiler::value Section::compile(Compiler& c) const {
	// std::cout << "Section<" << id << ">::compile " << std::endl;

	for (const auto& phi : phis) {
		std::cout << "phi " << phi->variable1 << " " << phi->value1.v << " " << phi->variable2 << " " << phi->value2.v << std::endl;
		if (phi->variable1->root == phi->variable2->root and phi->variable1->type == phi->variable2->type) {
			phi->variable->val = phi->variable1->val;
		} else {
			// std::cout << "compile phi " << phi->variable->type << std::endl;
			auto phi_node = c.insn_phi(phi->variable->type, c.insn_convert(phi->value1, phi->variable->type), phi->section1, c.insn_convert(phi->value2, phi->variable->type), phi->section2);
			phi->variable->val = c.create_entry(phi->variable->name, phi->variable->type);
			c.insn_store(phi->variable->val, phi_node);
		}
	}
	for (const auto& assignment : assignments) {
		std::cout << "Store variable " << assignment.first << " = " << assignment.second << std::endl;
		assignment.first->val = assignment.second->val;
	}

	((Section*) this)->first_basic_block = c.builder.GetInsertBlock(); // Update basic block before generation
}

void Section::compile_end(Compiler& c) const {
	// std::cout << "Section<" << id << ">::compile_end " << std::endl;
	// Export values for phis
	auto manage_phis = [&](const Section* section) {
		for (const auto& v : section->variables) {
			const auto& variable = v.second;
			for (const auto& phi : variable->phis) {
				// std::cout << "Variable export last value for phi " << variable.second << " " << variable.second->type << std::endl;
				if (phi->variable1 == variable and phi->section1 == this and phi->variable->block->enabled) {
					// std::cout << "Block export value1 " << variable.second << " convert to " <<  variable.second->phi->variable->type << std::endl;
					phi->value1 = c.insn_convert(c.insn_load(variable->val), phi->variable->type);
				}
				if (phi->variable2 == variable and phi->section2 == this and phi->variable->block->enabled) {
					// std::cout << "Block export value2 " << variable.second << " convert to " <<  variable.second->phi->variable->type << std::endl;
					phi->value2 = c.insn_convert(c.insn_load(variable->val), phi->variable->type);
				}
			}
		}
	};
	auto current = this;
	do {
		manage_phis(current);
		current = current->predecessors.size() == 1 ? current->predecessors[0] : nullptr;
	} while (current != nullptr);

	if (successors.size() == 2) {
		successors[0]->pre_compile(c);
		successors[1]->pre_compile(c);

		if (condition.v) {
			std::cout << "Section insn_if " << successors[0]->id << " " << successors[1]->id << std::endl;
			c.insn_if_sections(condition, successors[0], successors[1]);
		} else {
			c.insn_branch(successors[1]);
		}
	} else if (successors.size() == 1) {
		// std::cout << "Section go to successor 0 " << successors[0]->id << std::endl;

		successors[0]->pre_compile(c);
		c.insn_branch(successors[0]);
	}

	((Section*) this)->basic_block = c.builder.GetInsertBlock(); // Update basic block after generation
}

#endif

}