#include "Match.hpp"
#include "../../vm/LSValue.hpp"
#include "../semantic/SemanticAnalyser.hpp"

using namespace std;

namespace ls {

Match::Match() {
	value = nullptr;
}

Match::~Match() {
	delete value;
	for (auto& ps : pattern_list) {
		for (Pattern p : ps) {
			delete p.begin;
			delete p.end;
		}
	}
	for (auto x : returns) {
		delete x;
	}
}

void Match::print(std::ostream& os, int indent, bool debug) const {
	os << "match ";
	value->print(os, indent, debug);
	os << " {";
	for (size_t i = 0; i < pattern_list.size(); ++i) {

		os << endl << tabs(indent + 1);

		const vector<Pattern>& list = pattern_list[i];
		for (size_t j = 0; j < list.size(); ++j) {
			if (j > 0) {
				os << "|";
			}
			list[j].print(os, indent + 1, debug);
		}
		os << " : ";
		returns[i]->print(os, indent + 1, debug);
	}
	os << endl << tabs(indent) << "}";
	if (debug) {
		os << " " << type;
	}
}

unsigned Match::line() const {
	return 0;
}

void Match::analyse(SemanticAnalyser* analyser, const Type& req_type) {

	bool any_pointer = false;

	value->analyse(analyser, Type::UNKNOWN);
	if (value->type.nature == Nature::POINTER) any_pointer = true;

	for (auto& ps : pattern_list) {
		for (Pattern& p : ps) {
			if (any_pointer) break;

			if (p.begin) {
				p.begin->analyse(analyser, Type::UNKNOWN);
				if (p.begin->type.nature == Nature::POINTER) any_pointer = true;
			}
			if (p.end) {
				p.end->analyse(analyser, Type::UNKNOWN);
				if (p.end->type.nature == Nature::POINTER) any_pointer = true;
			}
		}
	}

	if (any_pointer) {
		value->analyse(analyser, Type::POINTER);
		for (auto& ps : pattern_list) {
			for (Pattern& p : ps) {
				if (p.begin) {
					p.begin->analyse(analyser, Type::POINTER);
				}
				if (p.end) {
					p.end->analyse(analyser, Type::POINTER);
				}
			}
		}
	}

	/* Default case not needed
	 *  match x {
	 *    1: { do this }
	 *    2: { do that }
	 *  }
	 *
	 * Default case needed
	 *  let y = match x {
	 *    1: { return this }
	 *    2: { return that }
	 *    ..: { return default }
	 *  }
	 */

	if (req_type.nature != Nature::VOID) {
		// in that case we need a default case

		bool default_exists = false;
		for (const auto& ps : pattern_list) {
			for (const Pattern& p : ps) {
				default_exists = default_exists || p.is_default();
			}
		}

		if (!default_exists) {
			analyser->add_error({SemanticException::Type::MUST_RETURN_A_VALUE, 0});
		}
	}
	// Return type is always pointer because in the default case, null is return
	type = Type::POINTER;
	for (Value* r : returns) {
		r->analyse(analyser, Type::POINTER);
	}
}

jit_value_t Match::compile(Compiler& c) const {

	jit_value_t v = value->compile(c);

	jit_value_t res = jit_value_create(c.F, JIT_POINTER);
	jit_label_t label_next = jit_label_undefined;
	jit_label_t label_end = jit_label_undefined;

	for (size_t i = 0; i < pattern_list.size(); ++i) {
		if (i > 0) {
			jit_insn_label(c.F, &label_next);
			label_next = jit_label_undefined;
		}

		if (pattern_list[i].size() == 1) {
			jit_value_t cond = pattern_list[i][0].match(c, v);
			if (cond) {
				jit_insn_branch_if_not(c.F, cond, &label_next);
			}
		} else {
			jit_label_t label_match = jit_label_undefined;

			for (const Pattern& pattern : pattern_list[i]) {
				jit_value_t cond = pattern.match(c, v);
				if (cond) jit_insn_branch_if(c.F, cond, &label_match);
				else jit_insn_branch(c.F, &label_match); // mettre .. dans la liste est idiot donc il n'y a pas besoin que ce soit optimisé
			}

			jit_insn_branch(c.F, &label_next);
			jit_insn_label(c.F, &label_match);
		}

		jit_value_t ret = returns[i]->compile(c);
		jit_insn_store(c.F, res, ret);
		jit_insn_branch(c.F, &label_end);
	}
	jit_insn_label(c.F, &label_next);

	//jit_insn_store(c.F, res, VM::create_null(c.F));
	/* Dont need this no longer
	 *
	 * default case always here if required
	 */

	jit_insn_label(c.F, &label_end);
	return res;
}

Match::Pattern::Pattern(Value* value)
	: interval(false), begin(value), end(nullptr) {}

Match::Pattern::Pattern(Value* begin, Value* end)
	: interval(true), begin(begin), end(end) {}

Match::Pattern::~Pattern() {}

void Match::Pattern::print(ostream &os, int indent, bool debug) const {
	if (interval) {
		if (begin) begin->print(os, indent, debug);
		os << "..";
		if (end) end->print(os, indent, debug);
	} else {
		begin->print(os, indent, debug);
	}
}

bool jit_equals_(LSValue* x, LSValue* y) {
	return x->operator == (y);
}
bool jit_less_(LSValue* x, LSValue* y) {
	return y->operator < (x);
}
bool jit_greater_equal_(LSValue* x, LSValue* y) {
	return y->operator >= (x);
}

jit_value_t Match::Pattern::match(Compiler &c, jit_value_t v) const {

	jit_type_t args_types[2] = {JIT_POINTER, JIT_POINTER};
	jit_type_t sig = jit_type_create_signature(jit_abi_cdecl, jit_type_sys_bool, args_types, 2, 0);

	if (interval) {
		jit_value_t ge = nullptr;
		if (begin) {
			jit_value_t b = begin->compile(c);
			if (begin->type.nature == Nature::VALUE) {
				ge = jit_insn_ge(c.F, v, b);
			} else {
				jit_value_t args[2] = { v, b };
				ge = jit_insn_call_native(c.F, "", (void*) jit_greater_equal_, sig, args, 2, JIT_CALL_NOTHROW);
			}
		}

		jit_value_t lt = nullptr;
		if (end) {
			jit_value_t e = end->compile(c);
			if (end->type.nature == Nature::VALUE) {
				lt = jit_insn_lt(c.F, v, e);
			} else {
				jit_value_t args[2] = { v, e };
				lt = jit_insn_call_native(c.F, "", (void*) jit_less_, sig, args, 2, JIT_CALL_NOTHROW);
			}
		}

		if (ge) {
			if (lt) {
				return jit_insn_and(c.F, ge, lt);
			} else {
				return ge;
			}
		} else {
			if (lt) {
				return lt;
			} else {
				return nullptr; // équivalent à default
			}
		}

	} else {
		jit_value_t cond;
		jit_value_t p = begin->compile(c);

		if (begin->type.nature == Nature::VALUE) {
			cond = jit_insn_eq(c.F, v, p);
		} else {
			jit_value_t args[2] = { v, p };
			cond = jit_insn_call_native(c.F, "", (void*) jit_equals_, sig, args, 2, JIT_CALL_NOTHROW);
		}
		return cond;
	}
}

}
