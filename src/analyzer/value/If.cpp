#include "If.hpp"
#include "../semantic/SemanticAnalyzer.hpp"
#include "Number.hpp"
#include "../instruction/Return.hpp"
#include "../instruction/Break.hpp"
#include "../instruction/Continue.hpp"
#include "../semantic/Variable.hpp"
#include "Phi.hpp"

namespace ls {

If::If(Environment& env, bool ternary) : Value(env) {
	this->ternary = ternary;
	jumping = true;
}

void If::print(std::ostream& os, int indent, PrintOptions options) const {
	// if (ternary) {
	// 	os << "(";
	// 	condition->print(os, indent, options);
	// 	os << " ? ";
	// 	then->sections[0]->instructions[0]->print(os, indent, options);
	// 	os << " : ";
	// 	elze->sections[0]->instructions[0]->print(os, indent, options);
	// 	os << ")";
	// } else {
		os << "if (";
		condition->print(os, indent + 1, options);
		os << ") ";
		then->print(os, indent, options);
		if (elze != nullptr) {
			os << " else ";
			elze->print(os, indent, options);
		}
	// }
	// if (options.debug) {
	// 	os << " " << type;
	// 	for (const auto& phi : phis) {
	// 		os << std::endl << tabs(indent) << phi->variable << " = phi(" << phi->variable1 << " " << phi->variable1->type << ", " << phi->variable2 << " " << phi->variable2->type << ") " << phi->variable->type;
	// 	}
	// }
}

Location If::location() const {
	return condition->location(); // TODO better
}

void If::pre_analyze(SemanticAnalyzer* analyzer) {
	condition->pre_analyze(analyzer);
	then->branch = then.get();
	then->pre_analyze(analyzer);
	if (elze != nullptr) {
		elze->branch = elze.get();
		elze->pre_analyze(analyzer);
	}
	// if (elze) {
	// 	phis = Phi::build_phis(analyzer, then.get(), elze.get());
	// } else {
	// 	phis = Phi::build_phis(analyzer, analyzer->current_block(), then.get());
	// }
}

void If::analyze(SemanticAnalyzer* analyzer) {
	// std::cout << "If " << is_void << std::endl;
	const auto& env = analyzer->env;

	condition->analyze(analyzer);
	throws = condition->throws;

	then->is_void = is_void;
	then->analyze(analyzer);

	type = then->type;

	if (elze != nullptr) {
		elze->is_void = is_void;
		elze->analyze(analyzer);
		// std::cout << "elze type " << elze->type << std::endl;
		if (type->is_void() and not elze->type->is_void() and not then->returning) {
			type = env.null;
		} else if (type != env.void_ and elze->type == env.void_) {
			type = type->operator + (env.null);
		} else {
			type = type->operator + (elze->type);
		}
	} else if (not type->is_void() or then->returning) {
		type = type->operator + (env.null);
	}
	if (is_void) type = env.void_;
	throws |= then->throws or (elze != nullptr and elze->throws);
	returning = then->returning and (elze != nullptr and elze->returning);
	may_return = then->may_return or (elze != nullptr and elze->may_return);
	return_type = then->return_type;
	if (elze != nullptr) return_type = return_type->operator + (elze->return_type);

	// for (const auto& phi : phis) {
	// 	phi->variable->type = phi->variable1->type->operator + (phi->variable2->type);
	// }
	// std::cout << "If type = " << type << std::endl;
}

#if COMPILER
Compiler::value If::compile(Compiler& c) const {

	// for (const auto& phi : phis) {
	// 	if (phi->variable1->block->branch != then->branch) {
	// 		// std::cout << "Variable export last value for phi " << phi->variable1 << std::endl;
	// 		phi->value1 = c.insn_convert(c.insn_load(phi->variable1->val), phi->variable->type);
	// 	}
	// }

	Compiler::value then_v { c.env };
	Compiler::value else_v { c.env };
	bool compile_elze = elze != nullptr or not type->is_void();

	auto cond = condition->compile(c);
	auto cond_boolean = c.insn_to_bool(cond);
	condition->compile_end(c);
	c.insn_delete_temporary(cond);

	c.leave_section_condition(cond_boolean);

	then_v = c.insn_convert(then->compile(c), type->fold());
	if (!then_v.v) then_v = c.insn_convert(c.new_null(), type->fold());
	then->compile_end(c);

	if (compile_elze) {
		if (elze) {
			else_v = c.insn_convert(elze->compile(c), type->fold());
			if (type != c.env.void_ and else_v.t == c.env.void_) {
				else_v = c.insn_convert(c.new_null(), type->fold());
			}
			elze->compile_end(c);
		} else if (not type->is_void()) {
			else_v = c.insn_convert(c.new_null(), type->fold());
		}
	}

	c.enter_section(end_section);

	auto r = [&]() -> Compiler::value { if (type->is_void()) {
		return { c.env };
	} else {
		return c.insn_phi(type, then_v, then->sections.back(), else_v, elze->sections.back());
	}}();

	// for (const auto& phi : phis) {
	// 	// std::cout << "phi " << phi->variable1 << " " << phi->value1.v << " " << phi->variable2 << " " << phi->value2.v << std::endl;
	// 	if (phi->variable1->root == phi->variable2->root and phi->variable1->type == phi->variable2->type) {
	// 		phi->variable->val = phi->variable1->val;
	// 	} else {
	// 		auto phi_node = c.insn_phi(phi->variable->type, c.insn_convert(phi->value1, phi->variable->type), phi->block1, c.insn_convert(phi->value2, phi->variable->type), phi->block2);
	// 		phi->variable->val = c.create_entry(phi->variable->name, phi->variable->type);
	// 		c.insn_store(phi->variable->val, phi_node);
	// 	}
	// 	if (phi->variable->phi and phi->variable->phi->variable2 == phi->variable) {
	// 		// std::cout << vv->var->phi->variable1 << " " << vv->var->phi->variable2 << std::endl;
	// 		// std::cout << vv->var->phi->value1.t << std::endl;
	// 		// std::cout << "phi delete unused var " << phi->variable->phi->variable1 << std::endl;
	// 		c.insn_delete_temporary(phi->variable->phi->value1);
	// 		// c.insn_delete_variable(phi->variable->phi->variable1->val);
	// 	}
	// }

	return r;
}
#endif

std::unique_ptr<Value> If::clone() const {
	auto iff = std::make_unique<If>(type->env);
	iff->condition = condition->clone();
	iff->then = unique_static_cast<Block>(then->clone());
	iff->elze = elze ? unique_static_cast<Block>(elze->clone()) : nullptr;
	iff->ternary = ternary;
	return iff;
}

}
