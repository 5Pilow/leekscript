#include "Block.hpp"
#include "../../vm/value/LSNull.hpp"
#include "../../vm/value/LSNumber.hpp"
#include "../instruction/Return.hpp"
#include "../instruction/Throw.hpp"
#include "../instruction/VariableDeclaration.hpp"
#include "Function.hpp"
#include "../semantic/FunctionVersion.hpp"
#include "../semantic/Variable.hpp"
#include "../instruction/ExpressionInstruction.hpp"
#include "../value/Phi.hpp"

namespace ls {

Block::Block(bool is_function_block) : is_function_block(is_function_block) {}

void Block::print(std::ostream& os, int indent, bool debug, bool condensed) const {
	if (!condensed) {
		os << "{";
		os << std::endl;
	}
	for (auto& instruction : instructions) {
		if (!condensed) os << tabs(indent + 1);
		instruction->print(os, indent + 1, debug, condensed);
		if (!condensed) os << std::endl;
	}
	if (!condensed) os << tabs(indent) << "}";
	if (debug) {
		os << " " << type;
		if (may_return) os << " ==>" << return_type;
	}
	if (variables.size()) {
		os << " vars: [";
		int i = 0;
		for (const auto& variable : variables) {
			if (i++ > 0) os << ", ";
			os << variable.second;
		}
		os << "]";
	}
	for (const auto& assignment : assignments) {
		os << std::endl << tabs(indent) << assignment.first << " " << assignment.first->type << " = " << assignment.second << " " << assignment.second->type;
	}
}

Location Block::location() const {
	assert(instructions.size());
	auto start = instructions.at(0)->location().start;
	auto end = instructions.back()->location().end;
	return {instructions.at(0)->location().file, start, end};
}

void Block::analyze_global_functions(SemanticAnalyzer* analyzer) {
	analyzer->enter_block(this);
	for (const auto& instruction : instructions) {
		if (auto vd = dynamic_cast<const VariableDeclaration*>(instruction.get())) {
			vd->analyze_global_functions(analyzer);
		}
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
	for (const auto& instruction : instructions) {
		instruction->pre_analyze(analyzer);
	}
	analyzer->leave_block();
	create_assignments(analyzer);
}

void Block::create_assignments(SemanticAnalyzer* analyzer) {
	if (not is_function_block and not is_loop_body) {
		for (const auto& variable : variables) {
			// std::cout << "Block update_var " << variable.second << " " << (void*) variable.second->block->branch << " " << (void*) analyzer->current_block()->branch << std::endl;
			auto root = variable.second->root ? variable.second->root : variable.second;
			if (variable.second->parent and variable.second->block->branch == analyzer->current_block()->branch	and root->block != this) {
				auto new_var = analyzer->update_var(variable.second->parent);
				variable.second->assignment = true;
				assignments.push_back({ new_var, variable.second });
			}
		}
	}
}

void Block::analyze(SemanticAnalyzer* analyzer) {
	// std::cout << "Block::analyze() " << is_void << std::endl;

	analyzer->enter_block(this);
	throws = false;

	type = Type::void_;

	for (unsigned i = 0; i < instructions.size(); ++i) {
		const auto& instruction = instructions.at(i);
		if (i < instructions.size() - 1 or is_void) { // Not the last instruction
			instruction->is_void = true;
		}
		instruction->analyze(analyzer);
		if (i == instructions.size() - 1 and not is_void) { // Last instruction
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

	analyzer->leave_block();

	// std::cout << "Block type " << type << std::endl;

	if (type->must_manage_memory()) {
		type = type->add_temporary();
	}

	if (type == Type::mpz) {
		type = Type::tmp_mpz;
	} else if (type == Type::tmp_mpz) {
		temporary_mpz = true;
	} else if (type == Type::tmp_mpz_ptr) {
		type = Type::tmp_mpz;
		temporary_mpz = true;
		mpz_pointer = true;
	} else if (type == Type::mpz_ptr or type == Type::const_mpz_ptr) {
		type = Type::tmp_mpz;
		mpz_pointer = true;
	}
	
	for (const auto& assignment : assignments) {
		assignment.first->type = assignment.second->type;
	}
}

Compiler::value Block::compile(Compiler& c) const {

	// std::cout << "Compile block " << type << std::endl;

	c.enter_block((Block*) this);
	((Block*) this)->blocks.push_back(c.builder.GetInsertBlock());

	for (unsigned i = 0; i < instructions.size(); ++i) {

		auto val = instructions[i]->compile(c);

		if (instructions[i]->returning) {
			// no need to compile after a return
			instructions[i]->compile_end(c);
			c.leave_block(false); // Variables already deleted by the return instruction
			if (is_function_block) {
				c.fun->compile_return(c, {});
			}
			return {};
		}
		if (i < instructions.size() - 1) {
			instructions[i]->compile_end(c);
			if (val.v != nullptr && !instructions[i]->type->is_void()) {
				c.insn_delete_temporary(val);
			}
		} else {
			auto return_value = [&]() {
				if (instructions[i]->is_void) {
					if (val.v) c.insn_delete_temporary(val);
					return Compiler::value();
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
			instructions[i]->compile_end(c);
			if (is_function_block and c.vm->context) {
				c.fun->parent->export_context(c);
			}
			// Load the value of variable needed for phi nodes after the block
			if (not is_function_block) {
				for (const auto& variable : variables) {
					if (variable.second->parent) {
						if (variable.second->phi) {
							// std::cout << "Variable export last value for phi " << variable.second << " " << variable.second->type << std::endl;
							if (variable.second->phi->variable1 == variable.second and variable.second->phi->variable->block->enabled) {
								// std::cout << "Block export value1 " << variable.second << " convert to " <<  variable.second->phi->variable->type << std::endl;
								variable.second->phi->value1 = c.insn_convert(c.insn_load(variable.second->val), variable.second->phi->variable->type);
							}
							if (variable.second->phi->variable2 == variable.second and variable.second->phi->variable->block->enabled) {
								// std::cout << "Block export value2 " << variable.second << " convert to " <<  variable.second->phi->variable->type << std::endl;
								variable.second->phi->value2 = c.insn_convert(c.insn_load(variable.second->val), variable.second->phi->variable->type);
							}
						}
					}
				}
			}
			for (const auto& assignment : assignments) {
				// std::cout << "Store variable " << assignment.first << " = " << assignment.second << std::endl;
				assignment.first->val = assignment.second->val;
			}
			c.leave_block();
			if (is_function_block) {
				c.fun->compile_return(c, return_value);
			}
			return return_value;
		}
	}
	c.leave_block();
	if (is_function_block) {
		c.fun->compile_return(c, {});
	}
	return {};
}

std::unique_ptr<Value> Block::clone() const {
	auto b = std::make_unique<Block>(is_function_block);
	for (const auto& i : instructions) {
		b->instructions.push_back(i->clone());
	}
	return b;
}

}
