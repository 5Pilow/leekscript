#include "Section.hpp"
#include "../../colors.h"
#include "Variable.hpp"
#include "SemanticAnalyzer.hpp"
#include "../value/Phi.hpp"
#include "../value/Block.hpp"
#include "../semantic/Mutation.hpp"
#include "../instruction/Foreach.hpp"

namespace ls {

const std::vector<std::string> Section::COLORS = { BLUE_BOLD, C_RED, C_YELLOW, GREEN_BOLD, C_PURPLE, C_CYAN, "\033[1;38;5;207m", "\033[1;38;5;208m", "\033[1;38;5;34m", C_PINK };
size_t Section::current_id = 0;

Section::Section(Environment& env, std::string name, Block* block) : env(env), name(name), block(block)
#if COMPILER
, condition(env)
#endif
{
	id = current_id++;
	color = COLORS[id % COLORS.size()];
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

Section* Section::common_ancestor(Section* section) const {
	if (this == section) { return section; }
	auto s1 = this;
	while (s1) {
		auto s2 = section;
		while (s2) {
			if (s1 == s2) { return s2; }
			s2 = s2->predecessors.size() ? s2->predecessors[0] : nullptr;
		}
		s1 = s1->predecessors.size() ? s1->predecessors[0] : nullptr;
	}
	return nullptr;
}

std::string tabs(int indent) {
	return std::string(indent * 4, ' ');
}

void Section::print(std::ostream& os, int indent, PrintOptions options) const {
	if (options.sections) {
		os << color << "┃ " << id << " " << name << END_COLOR << C_GREY " pred: <";
		int i = 0;
		for (const auto& predecessor : predecessors) {
			if (i++ > 0) os << C_GREY << ", " << END_COLOR;
			os << predecessor->color << predecessor->id << " " << predecessor->name << END_COLOR;
		}
		os << C_GREY << "> vars: [" << END_COLOR;
		i = 0;
		for (const auto& variable : variables) {
			if (i++ > 0) os << C_GREY << ", " << END_COLOR;
			if (!variable.second) os << "nullptr!!";
			else os << variable.second;
		}
		os << C_GREY << "]" << END_COLOR << C_GREY " succ: <";
		i = 0;
		for (const auto& successor : successors) {
			if (i++ > 0) os << C_GREY << ", " << END_COLOR;
			os << successor->color << successor->id << " " << successor->name << END_COLOR;
		}
		os << C_GREY << ">" << END_COLOR << std::endl;

		for (const auto& phi : phis) {
			os << color << "┃" << END_COLOR << tabs(indent + 1) << phi->variable << " = φ(" << phi->variable1 << " " << phi->variable1->type << " " << phi->section1->color << phi->section1->id << END_COLOR << ", " << phi->variable2 << " " << phi->variable2->type << " " << phi->section2->color << phi->section2->id << END_COLOR << ") " << phi->variable->type << std::endl;
		}
	}

	for (auto& instruction : instructions) {
		if (options.sections) {
			os << color << "┃" << END_COLOR;
		}
		if (!options.condensed) {
			os << tabs(indent + 1);
		}
		instruction->print(os, indent + 1, options);

		bool already_endl = options.sections and dynamic_cast<Foreach*>(instruction);
		if (!options.condensed and not already_endl) {
			os << std::endl;
		}
	}
}

void Section::pre_analyze(SemanticAnalyzer* analyzer) {

	for (auto it = variables.begin(); it != variables.end(); ) {
		if (not it->second->injected and not it->second->global) it = variables.erase(it);
		else ++it;
    }
	phis.clear();

	// std::cout << "pre_analyze section " << this << " preds = " << predecessors.size() << std::endl;

	analyzer->enter_section(this);
	// Build phis
	auto search_build_phis = [&](Section* section1, Variable* variable, Section* section2, Variable* variable2, Section* original_section1, Section* original_section2) {
		// std::cout << variable << " " << variable->get_root() << " " << variable2 << " " << variable2->get_root() << std::endl;
		auto new_var = analyzer->update_var(variable2);
		auto phi = new Phi { analyzer->env, new_var, original_section1, variable, original_section2, variable2 };
		variable->phis.push_back(phi);
		variable2->phis.push_back(phi);
		// std::cout << "new phi = " << phi << " " << new_var << " " << variable << " " << phi->variable->injected << std::endl;
		phis.emplace_back(phi);
		return true;
	};
	if (predecessors.size() == 2) {
		const auto& section1 = predecessors[0];
		const auto& section2 = predecessors[1];
		auto current_s1 = section1;
		std::set<std::string> found_vars;
		while (current_s1) {
			// std::cout << "search phi section " << current_s1->color << current_s1->id << END_COLOR << std::endl;
			for (const auto& variable : current_s1->variables) {
				auto variable1 = variable.second;
				if (found_vars.find(variable1->name) != found_vars.end()) continue;
				bool found = false;
				for (const auto& phi : phis) {
					if (phi->variable->name == variable1->name) { found = true; break; }
				}
				if (found) continue;
				// std::cout << "variable1 " << variable1 << std::endl;

				auto current_s2 = section2;
				while (current_s2) {
					auto i = current_s2->variables.find(variable1->name);
					if (i != current_s2->variables.end()) { // Trouvé
						if (variable1->get_root() == i->second->get_root()) {
							// std::cout << "var " << i->second << " found in " << current_s2 << std::endl;
							found_vars.insert(variable1->name);
							// Si même variable, pas besoin de phi
							if (i->second != variable1) {
								search_build_phis(current_s1, variable1, current_s2, i->second, section1, section2);
							}
						}
						break;
					} else { // Non trouvé
						current_s2 = current_s2->predecessors.size() ? current_s2->predecessors[0] : nullptr;
						if (current_s2 == this) break; // on a bouclé
					}
				}
			}
			current_s1 = current_s1->predecessors.size() ? current_s1->predecessors[0] : nullptr;
		}
	}
	// Analyze instructions
	for (const auto& instruction : instructions) {
		instruction->pre_analyze(analyzer);
	}
	if (analyzer->sections.back().size() and analyzer->current_section() == this) {
		analyzer->leave_section();
	}
}

void Section::analyze(SemanticAnalyzer* analyzer) {
	// std::cout << "Section::analyze " << color << id << END_COLOR << std::endl;
	for (const auto& phi : phis) {
		// std::cout << "Section phi analyze " << phi->variable1 << " + " << (phi->variable2) << std::endl;
		phi->variable->type = phi->variable1->type->operator + (phi->variable2->type);
		// std::cout << "Section phi " << phi->variable << " " << (void*) phi->variable << " analyze " << phi->variable1->type << " + " << (phi->variable2->type) << " = " << phi->variable->type << std::endl;
	}
}

void Section::analyze_end(SemanticAnalyzer* analyzer) {
	// std::cout << "Section::analyze_end " << color << id << END_COLOR << std::endl;
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
	((Section*) this)->pre_compile(c);

	// First pass of phi : create phi nodes
	for (const auto& phi : phis) {
		// std::cout << "phi " << phi->variable1 << " " << phi->value1.v << " " << phi->variable2 << " " << phi->value2.v << " " << phi->variable1->type << " " << phi->variable2->type << std::endl;
		if (not phi->variable1->type->is_mpz() and not phi->variable1->type->is_mpz_ptr()) {
			// std::cout << "compile phi " << phi->variable << " " << phi->variable->type << " " << phi->value1.t << " " << phi->value2.t << std::endl;

			// std::cout << "phi " << phi->variable << " type " << phi->variable->type << std::endl;
			auto phi_type = phi->variable->type;
			phi->phi_node = c.insn_phi(phi_type, phi->value1, phi->section1, phi->value2, phi->section2);
		}
	}

	// Second pass of phis : store values in variables
	for (const auto& phi : phis) {
		if (not phi->phi_node.v) {
			phi->variable->entry = phi->variable1->entry;
			phi->active = false;
		} else {
			auto phi_type = phi->variable->type->is_mpz_ptr() ? env.mpz : phi->variable->type;
			phi->variable->entry = c.create_entry(phi->variable->name, phi_type);
			c.insn_store(phi->variable->entry, phi->phi_node);
		}
	}

	((Section*) this)->first_basic_block = c.builder.GetInsertBlock(); // Update basic block before generation
}

void Section::compile_end(Compiler& c) const {
	// std::cout << "Section<" << color << id << END_COLOR << ">::compile_end " << std::endl;

	// Export values for phis
	auto manage_phis = [&](const Section* section) {
		// std::cout << "Manage phis of section " << section->id << std::endl;
		for (const auto& phi : section->phis) {
			const auto& variable = phi->variable;
			if (not phi->active) continue;
			// std::cout << "Variable export last value for phi " << variable << " " << variable->type << " from section " << color << id << END_COLOR << std::endl;
			if (phi->section1 == this) {
				const auto& var1 = phi->variable1;
				// std::cout << "Block export value1 " << var1 << " " << var1->type << " " << var1->val.t << " convert to " << phi->variable->type << std::endl;
				if (var1->entry.v) {
					// TODO : normalement pas besoin de faire une condition ici
					if (var1->type != phi->variable->type) {
						phi->value1 = c.insn_convert(var1->get_value(c), phi->variable->type, true);
					} else {
						phi->value1 = var1->get_value(c);
					}
					if (phi->phi_node.v) {
						((llvm::PHINode*) phi->phi_node.v)->addIncoming(phi->value1.v, c.builder.GetInsertBlock());
					}
				}
			}
			if (phi->section2 == this) {
				const auto& var2 = phi->variable2;
				// std::cout << "Block export value2 " << var2 << " " << var2->type << " convert to " <<  phi->variable->type << std::endl;
				// TODO : normalement pas besoin de faire une condition ici
				if (var2->type != phi->variable->type) {
					phi->value2 = c.insn_convert(var2->get_value(c), phi->variable->type, true);
				} else {
					phi->value2 = var2->get_value(c);
				}
				if (phi->phi_node.v) {
					// TODO add check
					// if (phi->phi_node.t != phi->value2.t) {
					// 	std::cout << "phi: " << phi->phi_node.t << " v2: " << phi->value2.t << std::endl;
					// 	assert(phi->phi_node.t == phi->value2.t);
					// }
					((llvm::PHINode*) phi->phi_node.v)->addIncoming(phi->value2.v, c.builder.GetInsertBlock());
				}
			}
		}
	};
	for (const auto& successor : successors) {
		manage_phis(successor);
	}

	if (successors.size() == 2) {
		successors[0]->pre_compile(c);
		successors[1]->pre_compile(c);

		if (condition.v) {
			// std::cout << "Section insn_if " << successors[0]->id << " " << successors[1]->id << std::endl;
			assert(condition.t->is_bool());
			c.insn_if_sections(condition, successors[0], successors[1]);
		} else {
			c.insn_branch(successors[0]);
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

namespace ls {

std::ostream& operator << (std::ostream& os, const Section* section) {
	os << section->color << section->id << " " << section->name << END_COLOR;
	return os;
}

}