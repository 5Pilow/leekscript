#include "PostfixExpression.hpp"
#include "LeftValue.hpp"
#include "../../vm/LSValue.hpp"
#include "../semantic/SemanticAnalyzer.hpp"
#include "../error/Error.hpp"
#include "../../type/Type.hpp"

namespace ls {

PostfixExpression::PostfixExpression() {
	return_value = true;
}

void PostfixExpression::print(std::ostream& os, int indent, bool debug, bool condensed) const {
	expression->print(os, indent, debug);
	operatorr->print(os);
	if (debug) {
		os << " " << type;
	}
}

Location PostfixExpression::location() const {
	return expression->location(); // TODO add the op
}

void PostfixExpression::pre_analyze(SemanticAnalyzer* analyzer) {
	expression->pre_analyze(analyzer);
}

void PostfixExpression::analyze(SemanticAnalyzer* analyzer) {

	expression->analyze(analyzer);
	throws = expression->throws;

	if (expression->type->constant) {
		analyzer->add_error({Error::Type::CANT_MODIFY_CONSTANT_VALUE, location(), expression->location(), {expression->to_string()}});
	}
	if (!expression->isLeftValue()) {
		analyzer->add_error({Error::Type::VALUE_MUST_BE_A_LVALUE, location(), expression->location(), {expression->to_string()}});
	}
	type = expression->type;
	throws |= expression->type->fold()->is_polymorphic();
	if (type == Type::mpz) {
		type = Type::tmp_mpz;
	}
	this->return_value = return_value;
	if (is_void) {
		type = Type::void_;
	}
}

Compiler::value PostfixExpression::compile(Compiler& c) const {

	c.inc_ops(1);
	c.mark_offset(operatorr->token->location.start.line);

	switch (operatorr->type) {

		case TokenType::PLUS_PLUS: {
			if (expression->type->is_mpz_ptr()) {
				auto x = expression->compile_l(c);
				auto one = c.new_integer(1);
				if (is_void) {
					c.insn_call(Type::void_, {x, x, one}, "Number.mpz_add_ui");
					return {};
				} else {
					auto r = c.insn_clone_mpz(x);
					c.insn_call(Type::void_, {x, x, one}, "Number.mpz_add_ui");
					return r;
				}
			} else if (!expression->type->is_polymorphic()) {
				auto x_addr = expression->compile_l(c);
				auto x = c.insn_load(x_addr);
				auto sum = c.insn_add(x, c.new_integer(1));
				c.insn_store(x_addr, sum);
				return x;
			} else {
				auto e = expression->compile_l(c);
				if (is_void) {
					return c.insn_invoke(Type::any, {e}, "Value.pre_incl");
				} else {
					return c.insn_invoke(Type::any, {e}, "Value.incl");
				}
			}
			break;
		}
		case TokenType::MINUS_MINUS: {
			if (expression->type->is_mpz_ptr()) {
				auto x = expression->compile_l(c);
				auto one = c.new_integer(1);
				if (is_void) {
					c.insn_call(Type::void_, {x, x, one}, "Number.mpz_sub_ui");
					return {};
				} else {
					auto r = c.insn_clone_mpz(x);
					c.insn_call(Type::void_, {x, x, one}, "Number.mpz_sub_ui");
					return r;
				}
			} else if (expression->type->is_primitive()) {
				auto x_addr = expression->compile_l(c);
				auto x = c.insn_load(x_addr);
				auto sum = c.insn_sub(x, c.new_integer(1));
				c.insn_store(x_addr, sum);
				return x;
			} else {
				auto e = expression->compile_l(c);
				if (is_void) {
					return c.insn_invoke(Type::any, {e}, "Value.pre_decl");
				} else {
					return c.insn_invoke(Type::any, {e}, "Value.decl");
				}
			}
			break;
		}
		default: {}
	}
	return {nullptr, {}};
}

std::unique_ptr<Value> PostfixExpression::clone() const {
	auto pe = std::make_unique<PostfixExpression>();
	pe->expression = unique_static_cast<LeftValue>(expression->clone());
	pe->operatorr = operatorr;
	return pe;
}

}
