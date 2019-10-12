#include "Section.hpp"
#include "../../colors.h"
#include "Variable.hpp"
#include "SemanticAnalyzer.hpp"
#include "../value/Phi.hpp"
#include "../value/Block.hpp"
#include "../semantic/Mutation.hpp"

namespace ls {

const std::vector<std::string> Section::COLORS = { BLUE_BOLD, C_RED, C_YELLOW, GREEN_BOLD, C_PURPLE, C_CYAN, "\033[1;38;5;207m", "\033[1;38;5;208m", "\033[1;38;5;34m", C_PINK };
size_t Section::current_id = 0;

Section::Section(Environment& env, std::string name) : env(env), name(name), condition(env) {
	id = current_id++;
	if (id >= COLORS.size()) {
		color = C_PINK;
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

void Section::add_conversion(Variable* variable, Variable* variable2, const Section* mutation) {
	conversions.push_back({ variable, variable2, mutation });
}

std::string tabs(int indent) {
	return std::string(indent * 4, ' ');
}

void Section::print(std::ostream& os, int indent, PrintOptions options) const {
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

	auto print_conversions = [&]() {
		i = 0;
		for (const auto& conversion : conversions) {
			os << color << "┃" << END_COLOR << tabs(indent + 1);
			os << std::get<0>(conversion) << " " << std::get<0>(conversion)->type << " = (" << std::get<1>(conversion)->type << ") " << std::get<0>(conversion)->parent << " [" << std::get<1>(conversion) << " " << std::get<1>(conversion)->type << " " << std::get<2>(conversion)->color << std::get<2>(conversion)->id << END_COLOR << "]";
			i++;
			if (not (i == conversions.size() && options.condensed)) {
				os << std::endl;
			}
		}
	};

	bool first_jump = true;
	for (auto& instruction : instructions) {
		if (instruction->jumping and first_jump) {
			first_jump = false;
			print_conversions();
		}
		os << color << "┃" << END_COLOR;
		os << tabs(indent + 1);
		instruction->print(os, indent + 1, options);
		if (!options.condensed or conversions.size()) os << std::endl;
	}
	if (first_jump) {
		print_conversions();
	}
}

void Section::pre_analyze(SemanticAnalyzer* analyzer) {

	variables.clear();
	conversions.clear();
	phis.clear();

	// std::cout << "pre_analyze section " << id << " preds = " << predecessors.size() << std::endl;

	analyzer->enter_section(this);
	// Build phis
	auto search_build_phis = [&](Section* section1, Variable* variable, Section* section2, Section* original_section1, Section* original_section2) {
		if (section1 == section2) return true;
		auto i = section2->variables.find(variable->name);
		if (i == section2->variables.end()) return false;
		auto variable2 = i->second;
		// auto parent = variable == variable2->parent ? variable : variable->parent;
		auto new_var = analyzer->update_var(variable2);
		auto phi = new Phi { analyzer->env, new_var, original_section1, variable, original_section2, variable2 };
		variable->phis.push_back(phi);
		variable2->phis.push_back(phi);
		// std::cout << "new phi = " << phi << " " << new_var << " " << variable << std::endl;
		phis.push_back(phi);
		return true;
	};
	if (predecessors.size() == 2) {
		const auto& section1 = predecessors[0];
		const auto& section2 = predecessors[1];
		auto current_s1 = section1;
		while (current_s1) {
			for (const auto& variable : current_s1->variables) {
				auto variable1 = variable.second;
				bool found = false;
				for (const auto& phi : phis) {
					if ( phi->variable->name == variable1->name) { found = true; break; }
				}
				if (found) continue;
				auto current_s2 = section2;
				while (current_s2 and not search_build_phis(current_s1, variable1, current_s2, section1, section2)) {
					current_s2 = current_s2->predecessors.size() ? current_s2->predecessors[0] : nullptr;
					if (current_s2 == this) break; // on a bouclé
				}
			}
			current_s1 = current_s1->predecessors.size() ? current_s1->predecessors[0] : nullptr;
		}
	}
	// Analyze instructions
	for (const auto& instruction : instructions) {
		instruction->pre_analyze(analyzer);
	}
	if (analyzer->current_section() == this) {
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
	for (auto& conversion : conversions) {

		// auto parent = std::get<0>(conversion)->section->variables.at(std::get<0>(conversion)->name);
		auto parent = std::get<0>(conversion)->parent;

		// std::cout << "Section analyze_end " << std::get<0>(conversion) << " " << std::get<0>(conversion)->type << " = (" << parent->type << ") " << parent << std::endl;
		std::get<0>(conversion)->type = parent->type;
	}
}

void Section::reanalyze_conversions(SemanticAnalyzer* analyzer) {
	// std::cout << "Section::analyze_end " << color << id << END_COLOR << std::endl;
	for (auto& conversion : conversions) {

		auto parent = std::get<2>(conversion)->variables.at(std::get<0>(conversion)->name);

		// std::cout << "Section reanalyze_conversions " << std::get<0>(conversion) << " " << std::get<0>(conversion)->type << " = (" << parent->type << ") " << parent << std::endl;
		std::get<0>(conversion)->type = parent->type;
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
		// std::cout << "phi " << phi->variable1 << " " << phi->value1.v << " " << phi->variable2 << " " << phi->value2.v << " " << phi->variable1->type << " " << phi->variable2->type << std::endl;
		if (phi->variable1->root == phi->variable2->root and phi->variable1->type == phi->variable2->type) {
			phi->variable->val = phi->variable1->val;
			phi->active = false;
		} else {
			// std::cout << "compile phi " << phi->variable << " " << phi->variable->type << " " << phi->value1.t << " " << phi->value2.t << std::endl;
			if (phi->value1.v && phi->value2.v) {
				// std::cout << "phi " << phi->variable << " type " << phi->variable->type << std::endl;
				auto phi_type = phi->variable->type->is_mpz_ptr() ? env.mpz : phi->variable->type;
				auto phi_node = c.insn_phi(phi_type, c.insn_convert(phi->value1, phi_type), phi->section1, c.insn_convert(phi->value2, phi_type), phi->section2);
				phi->variable->val = c.create_entry(phi->variable->name, phi_type);
				c.insn_store(phi->variable->val, phi_node);
			} else {
				phi->variable->val = phi->variable1->val;
				// std::cout << "half phi: " << phi->variable << " " << phi->variable->type << " " << phi->variable->val.t << std::endl;
				phi->active = false;
			}
		}
	}

	((Section*) this)->first_basic_block = c.builder.GetInsertBlock(); // Update basic block before generation
}

void Section::compile_end(Compiler& c) const {
	// std::cout << "Section<" << color << id << END_COLOR << ">::compile_end " << std::endl;

	// Conversions
	for (const auto& conversion : conversions) {
		// std::cout << "Convert variable " << std::get<0>(conversion) << " " << std::get<0>(conversion)->type << " = " << std::get<0>(conversion)->parent << " " << std::get<0>(conversion)->parent->type << std::endl;
		if (std::get<0>(conversion)->type != std::get<1>(conversion)->parent->type) {
			auto val = c.insn_convert(std::get<0>(conversion)->parent->get_value(c), std::get<1>(conversion)->type);
			std::get<0>(conversion)->create_entry(c);
			std::get<0>(conversion)->store_value(c, c.insn_move_inc(val));
			std::get<0>(conversion)->parent->delete_value(c);
		} else {
			// std::cout << "no conversion needed: " << std::get<0>(conversion)->type << " " << std::get<0>(conversion)->val.t << " => " << std::get<1>(conversion)->parent << " " << std::get<1>(conversion)->parent->val.t << std::endl;
			std::get<0>(conversion)->val = std::get<1>(conversion)->parent->val;
		}
		// std::cout << "converted value = " << val.t << std::endl;
	}

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
				// TODO : normalement pas besoin de faire une condition ici
				if (var1->type != phi->variable->type) {
					phi->value1 = c.insn_convert(c.insn_load(var1->val), phi->variable->type);
				} else {
					phi->value1 = c.insn_load(var1->val);
				}
			}
			if (phi->section2 == this) {
				const auto& var2 = phi->variable2;
				// std::cout << "Block export value2 " << var2 << " " << var2->type << " convert to " <<  phi->variable->type << std::endl;
				// TODO : normalement pas besoin de faire une condition ici
				if (var2->type != phi->variable->type) {
					phi->value2 = c.insn_convert(c.insn_load(var2->val), phi->variable->type);
				} else {
					phi->value2 = c.insn_load(var2->val);
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

Section* Section::clone() const {
	auto s = new Section(env, name);
	for (const auto& i : instructions) {
		s->instructions.push_back(i->clone());
	}
	return s;
}

}