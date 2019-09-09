#include "Return.hpp"
#include "../semantic/SemanticAnalyzer.hpp"
#include "../value/Function.hpp"
#include "../semantic/FunctionVersion.hpp"

namespace ls {

Return::Return(std::unique_ptr<Value> v) : expression(std::move(v)) {
	returning = true;
	may_return = true;
}

void Return::print(std::ostream& os, int indent, bool debug, bool condensed) const {
	os << "return ";
	if (expression != nullptr) {
		expression->print(os, indent, debug);
	}
}

void Return::pre_analyze(SemanticAnalyzer* analyzer) {
	if (expression != nullptr) {
		expression->pre_analyze(analyzer);
	}
}

void Return::analyze(SemanticAnalyzer* analyzer, const Type*) {
	if (expression != nullptr) {
		expression->analyze(analyzer);
		return_type = expression->type;
		if (return_type->is_mpz_ptr()) return_type = Type::tmp_mpz;
		throws = expression->throws;
	}
}

Location Return::location() const {
	return expression->location();
}

Compiler::value Return::compile(Compiler& c) const {
	if (expression != nullptr) {
		auto r = expression->compile(c);
		if (r.t == Type::mpz_ptr) {
			r = c.insn_load(c.insn_clone_mpz(r));
		} else if (r.t == Type::tmp_mpz_ptr) {
			r = c.insn_load(r);
		} else {
			r = c.insn_move(r);
		}
		c.fun->compile_return(c, r, true);
	} else {
		c.fun->compile_return(c, {}, true);
	}
	c.insert_new_generation_block();
	return {};
}

std::unique_ptr<Instruction> Return::clone() const {
	auto ex = expression ? expression->clone() : nullptr;
	return std::make_unique<Return>(std::move(ex));
}

}
