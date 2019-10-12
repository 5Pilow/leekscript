#include "Block.hpp"
#include "../instruction/Return.hpp"
#include "../instruction/Throw.hpp"
#include "../instruction/VariableDeclaration.hpp"
#include "Function.hpp"
#include "../semantic/FunctionVersion.hpp"
#include "../semantic/Variable.hpp"
#include "../instruction/ExpressionInstruction.hpp"
#include "../value/Phi.hpp"
#include "../value/If.hpp"
#include "../../colors.h"

namespace ls {

Block::Block(Environment& env, bool is_function_block) : Value(env), is_function_block(is_function_block)
#if COMPILER
, return_value(env)
#endif
{
	sections.push_back(new Section { env, "block" });
	jumping = true;
}

void Block::print(std::ostream& os, int indent, PrintOptions options) const {
	if (!options.condensed) {
		os << "{";
		os << std::endl;
	}
	for (auto& section : sections) {
		section->print(os, indent, options);
	}
	if (!options.condensed) {
		if (!is_function_block) {
			os << sections.back()->color << "┃" << END_COLOR;
		}
		os << tabs(indent) << "}";
	}
	if (options.debug) {
		os << " " << type;
		if (may_return) os << " ==>" << return_type;
		// for (const auto& assignment : assignments) {
		// 	os << std::endl << tabs(indent) << assignment.first << " " << assignment.first->type << " = " << assignment.second << " " << assignment.second->type;
		// }
	}
}

Location Block::location() const {
	assert(sections.size());
	assert(sections.at(0)->instructions.size());
	auto start = sections.at(0)->instructions.at(0)->location().start;
	auto end = sections.at(0)->instructions.back()->location().end;
	return {sections.at(0)->instructions.at(0)->location().file, start, end};
}

void Block::add_instruction(Instruction* instruction) {
	add_instruction(std::unique_ptr<Instruction>(instruction));
}

void Block::add_instruction(std::unique_ptr<Instruction> instruction) {
	if (!instruction) return; // it's possible that the instruction is null (parse failed)
	jumping |= instruction->jumping;
	auto last = sections.back()->instructions.emplace_back(std::move(instruction)).get();
	if (last->jumping) {
		assert(last->end_section);
		sections.push_back(last->end_section);
	}
}

void Block::analyze_global_functions(SemanticAnalyzer* analyzer) {
	analyzer->enter_block(this);
	for (const auto& section : sections) {
		analyzer->enter_section(section);
		for (const auto& instruction : section->instructions) {
			if (auto vd = dynamic_cast<const VariableDeclaration*>(instruction.get())) {
				vd->analyze_global_functions(analyzer);
			}
		}
		analyzer->leave_section();
	}
	analyzer->leave_block();
}

void Block::setup_branch(SemanticAnalyzer* analyzer) {
	if (!branch) {
		branch = analyzer->current_function()->body.get() == this ? this : analyzer->current_block()->branch;
	}
}

void Block::pre_analyze(SemanticAnalyzer* analyzer) {
	setup_branch(analyzer);
	analyzer->enter_block(this);
	for (const auto& section : sections) {
		section->pre_analyze(analyzer);
	}
	analyzer->leave_block();
	create_assignments(analyzer);
}

void Block::create_assignments(SemanticAnalyzer* analyzer) {
	// if (not is_function_block and not is_loop_body) {
	// 	for (const auto& variable : variables) {
	// 		// std::cout << "Block update_var " << variable.second << " " << (void*) variable.second->block->branch << " " << (void*) analyzer->current_block()->branch << std::endl;
	// 		auto root = variable.second->root ? variable.second->root : variable.second;
	// 		if (variable.second->parent and variable.second->block->branch == analyzer->current_block()->branch	and root->block != this) {
	// 			auto new_var = analyzer->update_var(variable.second->parent);
	// 			variable.second->assignment = true;
	// 			assignments.push_back({ new_var, variable.second });
	// 		}
	// 	}
	// }
}

void Block::analyze(SemanticAnalyzer* analyzer) {
	const auto& env = analyzer->env;
	// std::cout << "Block::analyze() " << is_void << std::endl;

	analyzer->enter_block(this);
	throws = false;

	type = env.void_;

	// TODO FLAG is_void on whole block
	// std::cout << "sections : " << sections.size() << std::endl;

	for (unsigned s = 0; s < sections.size(); ++s) {
		const auto& section = sections[s];
		auto last_section = (s == sections.size() - 1) or (s == sections.size() - 2 and sections.back()->instructions.size() == 0);
		// std::cout << "instruction " << s << " " << last_section << std::endl;
		analyzer->enter_section(section);
		section->analyze(analyzer);
		for (unsigned i = 0; i < section->instructions.size(); ++i) {
			const auto& instruction = section->instructions.at(i);
			if (not last_section or i < section->instructions.size() - 1 or is_void) { // Not the last instruction
				instruction->is_void = true;
			}
			instruction->analyze(analyzer);
			if (last_section and i == section->instructions.size() - 1 and not is_void) { // Last instruction
				type = instruction->type;
			}
			if (instruction->may_return) may_return = true;
			if (instruction->throws) throws = true;
			return_type = return_type->operator + (instruction->return_type);
			if (instruction->returning) {
				returning = true;
				break; // no need to analyze after a return
			}
		}
		analyzer->leave_section();
	}

	analyzer->leave_block();

	// std::cout << "Block type " << type << std::endl;

	if (type->must_manage_memory()) {
		type = type->add_temporary();
	}

	if (type == env.mpz) {
		type = env.tmp_mpz;
	} else if (type == env.tmp_mpz) {
		temporary_mpz = true;
	} else if (type == env.tmp_mpz_ptr) {
		type = env.tmp_mpz;
		temporary_mpz = true;
		mpz_pointer = true;
	} else if (type == env.mpz_ptr or type == env.const_mpz_ptr) {
		type = env.tmp_mpz;
		mpz_pointer = true;
	}

	// for (const auto& assignment : assignments) {
	// 	assignment.first->type = assignment.second->type;
	// }
}

#if COMPILER
Compiler::value Block::compile(Compiler& c) const {

	// std::cout << "Compile block " << type << std::endl;

	c.enter_block((Block*) this);

	for (unsigned s = 0; s < sections.size(); ++s) {
		const auto& section = sections[s];
		auto last_section = (s == sections.size() - 1) or (s == sections.size() - 2 and sections.back()->instructions.size() == 0);
		c.enter_section(section);
		for (unsigned i = 0; i < section->instructions.size(); ++i) {

			auto val = section->instructions[i]->compile(c);

			if (section->instructions[i]->returning) {
				// no need to compile after a return
				section->instructions[i]->compile_end(c);
				return { c.env };
			}
			if (not last_section or i < section->instructions.size() - 1) { // not the last instruction
				section->instructions[i]->compile_end(c);
				if (val.v != nullptr && !section->instructions[i]->type->is_void()) {
					c.insn_delete_temporary(val);
				}
			} else {
				((Block*) this)->return_value = [&]() {
					if (section->instructions[i]->is_void) {
						if (val.v) c.insn_delete_temporary(val);
						return Compiler::value { c.env };
					} else if (not val.v) {
						return val;
					} else if (type->must_manage_memory() and val.v != nullptr) {
						return c.insn_move(val);
					} else if (mpz_pointer) {
						return c.insn_load(temporary_mpz ? val : c.insn_clone_mpz(val));
					} else if (type->is_mpz()) {
						return temporary_mpz ? val : c.insn_clone_mpz(val);
					} else {
						return val;
					}
				}();
				section->instructions[i]->compile_end(c);
				if (is_function_block and c.vm->context) {
					c.fun->parent->export_context(c);
				}
				// Load the value of variable needed for phi nodes after the block
				if (not is_function_block) {
					// for (const auto& variable : variables) {
					// 	if (variable.second->parent) {
					// 		if (variable.second->phi) {
					// 			// std::cout << "Variable export last value for phi " << variable.second << " " << variable.second->type << std::endl;
					// 			if (variable.second->phi->variable1 == variable.second and variable.second->phi->variable->block->enabled) {
					// 				// std::cout << "Block export value1 " << variable.second << " convert to " <<  variable.second->phi->variable->type << std::endl;
					// 				variable.second->phi->value1 = c.insn_convert(c.insn_load(variable.second->val), variable.second->phi->variable->type);
					// 			}
					// 			if (variable.second->phi->variable2 == variable.second and variable.second->phi->variable->block->enabled) {
					// 				// std::cout << "Block export value2 " << variable.second << " convert to " <<  variable.second->phi->variable->type << std::endl;
					// 				variable.second->phi->value2 = c.insn_convert(c.insn_load(variable.second->val), variable.second->phi->variable->type);
					// 			}
					// 		}
					// 	}
					// }
				}
				// for (const auto& assignment : assignments) {
				// 	// std::cout << "Store variable " << assignment.first << " = " << assignment.second << std::endl;
				// 	assignment.first->val = assignment.second->val;
				// }
				return return_value;
			}
		}
		if (s < sections.size() - 1 and c.current_section() == section) {
			c.leave_section();
		}
	}
	return { c.env };
}

void Block::compile_end(Compiler& c) const {
	if (c.current_section() == sections.back()) {
		c.leave_section();
	}
	c.leave_block();
	if (is_function_block) {
		c.fun->compile_return(c, return_value);
	}
}

#endif

std::unique_ptr<Value> Block::clone() const {
	auto b = std::make_unique<Block>(type->env, is_function_block);
	for (const auto& section : sections) {
		for (const auto& i : section->instructions) {
			b->add_instruction(i->clone());
		}
	}
	return b;
}

}
