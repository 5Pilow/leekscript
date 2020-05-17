#include "PostfixExpression.hpp"
#include "LeftValue.hpp"
#include "../semantic/SemanticAnalyzer.hpp"
#include "../error/Error.hpp"
#include "../../type/Type.hpp"

namespace ls {

PostfixExpression::PostfixExpression(Environment& env) : Value(env) {
	return_value = true;
}

void PostfixExpression::print(std::ostream& os, int indent, PrintOptions options) const {
	expression->print(os, indent, options);
	operatorr->print(os);
	if (options.debug) {
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
	const auto& env = analyzer->env;

	expression->analyze(analyzer);
	throws = expression->throws;

	if (expression->type->constant) {
		analyzer->add_error({Error::Type::CANT_MODIFY_CONSTANT_VALUE, ErrorLevel::ERROR, location(), expression->location(), {expression->to_string()}});
	}
	if (!expression->isLeftValue()) {
		analyzer->add_error({Error::Type::VALUE_MUST_BE_A_LVALUE, ErrorLevel::ERROR, location(), expression->location(), {expression->to_string()}});
	}
	type = expression->type;
	throws |= expression->type->fold()->is_polymorphic();
	if (type == env.mpz) {
		type = env.tmp_mpz;
	}
	if (is_void) {
		type = env.void_;
	}
}

#if COMPILER
Compiler::value PostfixExpression::compile(Compiler& c) const {
	const auto& env = c.env;

	c.inc_ops(1);
	c.mark_offset(operatorr->token->location.start.line);

	switch (operatorr->type) {

		case TokenType::PLUS_PLUS: {
			if (expression->type->is_mpz_ptr()) {
				auto x = ((LeftValue*) expression.get())->compile_l(c);
				auto one = c.new_integer(1);
				if (is_void) {
					c.insn_call(env.void_, {x, x, one}, "Number.mpz_add_ui");
					return { c.env };
				} else {
					auto r = c.insn_clone_mpz(x);
					c.insn_call(env.void_, {x, x, one}, "Number.mpz_add_ui");
					return r;
				}
			} else if (!expression->type->is_polymorphic()) {
				auto x_addr = ((LeftValue*) expression.get())->compile_l(c);
				auto x = c.insn_load(x_addr);
				auto sum = c.insn_add(x, c.new_integer(1));
				c.insn_store(x_addr, sum);
				return x;
			} else {
				auto e = ((LeftValue*) expression.get())->compile_l(c);
				if (is_void) {
					return c.insn_invoke(env.any, {e}, "Value.pre_incl");
				} else {
					return c.insn_invoke(env.any, {e}, "Value.incl");
				}
			}
			break;
		}
		case TokenType::MINUS_MINUS: {
			if (expression->type->is_mpz_ptr()) {
				auto x = ((LeftValue*) expression.get())->compile_l(c);
				auto one = c.new_integer(1);
				if (is_void) {
					c.insn_call(env.void_, {x, x, one}, "Number.mpz_sub_ui");
					return { c.env };
				} else {
					auto r = c.insn_clone_mpz(x);
					c.insn_call(env.void_, {x, x, one}, "Number.mpz_sub_ui");
					return r;
				}
			} else if (expression->type->is_primitive()) {
				auto x_addr = ((LeftValue*) expression.get())->compile_l(c);
				auto x = c.insn_load(x_addr);
				auto sum = c.insn_sub(x, c.new_integer(1));
				c.insn_store(x_addr, sum);
				return x;
			} else {
				auto e = ((LeftValue*) expression.get())->compile_l(c);
				if (is_void) {
					return c.insn_invoke(env.any, {e}, "Value.pre_decl");
				} else {
					return c.insn_invoke(env.any, {e}, "Value.decl");
				}
			}
			break;
		}
		default: {}
	}
	return {nullptr, {}};
}
#endif

std::unique_ptr<Value> PostfixExpression::clone(Block* parent) const {
	auto pe = std::make_unique<PostfixExpression>(type->env);
	pe->expression = unique_static_cast<Value>(expression->clone(parent));
	pe->operatorr = operatorr;
	return pe;
}

}
