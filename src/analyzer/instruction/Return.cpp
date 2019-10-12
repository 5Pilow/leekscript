#include "Return.hpp"
#include "../semantic/SemanticAnalyzer.hpp"
#include "../value/Function.hpp"
#include "../semantic/FunctionVersion.hpp"

namespace ls {

Return::Return(Environment& env, std::unique_ptr<Value> v) : Instruction(env), expression(std::move(v)) {
	returning = true;
	may_return = true;
	jumping = true;
	jump_to_existing_section = true;
	// end_section = (Section*) (void*) 121212; // fake end_section to indicate the return is leaving somewhere
}

void Return::print(std::ostream& os, int indent, PrintOptions options) const {
	os << "return ";
	if (expression != nullptr) {
		expression->print(os, indent, options);
	}
}

void Return::pre_analyze(SemanticAnalyzer* analyzer) {
	if (expression != nullptr) {
		expression->pre_analyze(analyzer);
	}
}

void Return::analyze(SemanticAnalyzer* analyzer, const Type*) {
	auto& env = analyzer->env;
	if (expression != nullptr) {
		expression->analyze(analyzer);
		return_type = expression->type;
		if (return_type->is_mpz_ptr()) return_type = env.tmp_mpz;
		throws = expression->throws;
	}
}

Location Return::location() const {
	return expression->location();
}

#if COMPILER
Compiler::value Return::compile(Compiler& c) const {
	auto& env = c.env;
	if (expression != nullptr) {
		auto r = expression->compile(c);
		if (r.t == env.mpz_ptr) {
			r = c.insn_load(c.insn_clone_mpz(r));
		} else if (r.t == env.tmp_mpz_ptr) {
			r = c.insn_load(r);
		} else {
			r = c.insn_move(r);
		}
		c.fun->compile_return(c, r, true);
	} else {
		c.fun->compile_return(c, { c.env }, true);
	}
	// c.insert_new_generation_block();
	return { c.env };
}
#endif

std::unique_ptr<Instruction> Return::clone(Block* parent) const {
	auto ex = expression ? expression->clone(parent) : nullptr;
	return std::make_unique<Return>(type->env, std::move(ex));
}

}
