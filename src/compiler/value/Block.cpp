#include "Block.hpp"

#include "../../vm/value/LSNull.hpp"
#include "../../vm/value/LSNumber.hpp"
#include "../instruction/Return.hpp"
#include "../../vm/VM.hpp"

using namespace std;

namespace ls {

Block::Block() {
	type = Type::NULLL;
}

Block::~Block() {
	for (Instruction* instruction : instructions) {
		delete instruction;
	}
}

void Block::print(ostream& os, int indent, bool debug) const {
	os << "{";
	os << endl;
	for (Instruction* instruction : instructions) {
		os << tabs(indent + 1);
		instruction->print(os, indent + 1, debug);
		os << endl;
	}
	os << tabs(indent) << "}";
	if (debug) {
		os << " " << type;
	}
}

unsigned Block::line() const {
	return 0;
}

void Block::analyse(SemanticAnalyser* analyser, const Type& req_type) {

	analyser->enter_block();

	type = Type::VOID;

	for (unsigned i = 0; i < instructions.size(); ++i) {
		if (i < instructions.size() - 1 || req_type.nature == Nature::VOID) {
			instructions[i]->analyse(analyser, Type::VOID);
		} else {
			instructions[i]->analyse(analyser, req_type);
			type = instructions[i]->type;
		}
		if (dynamic_cast<Return*>(instructions[i])) {
			type = Type::VOID; // This block has really no type
			analyser->leave_block();
			return; // no need to compile after a return
		}
	}

	analyser->leave_block();

	if (type.nature == Nature::VOID) { // empty block or last instruction type is VOID
		if (req_type.nature != Nature::UNKNOWN) {
			type.nature = req_type.nature;
		} else {
			type = Type::NULLL;
		}
	}
}

jit_value_t Block::compile(Compiler& c) const {

	c.enter_block();

	for (unsigned i = 0; i < instructions.size(); ++i) {
		jit_value_t val = instructions[i]->compile(c);
		if (dynamic_cast<Return*>(instructions[i])) {
			break; // no need to compile after a return
		}
		if (i == instructions.size() - 1 && instructions[i]->type.nature != Nature::VOID) {
			if (type.must_manage_memory()) {
				jit_value_t ret = VM::move_obj(c.F, val);
				c.leave_block(c.F);
				return ret;
			} else {
				c.leave_block(c.F);
				return val;
			}
		}
	}
	c.leave_block(c.F);

	if (type.nature == Nature::POINTER) {
		return VM::get_null(c.F);
	}
	if (type.nature == Nature::VALUE) {
		return jit_value_create_nint_constant(c.F, VM::get_jit_type(type), 0);
	}
	return nullptr;
}

}
