#include "If.hpp"
#include "../semantic/SemanticAnalyzer.hpp"
#include "Number.hpp"
#include "../instruction/Return.hpp"
#include "../instruction/Break.hpp"
#include "../instruction/Continue.hpp"
#include "../../vm/LSValue.hpp"
#include "../semantic/Variable.hpp"
#include "Phi.hpp"

namespace ls {

If::If(bool ternary) {
	type = Type::void_;
	this->ternary = ternary;
}

void If::print(std::ostream& os, int indent, bool debug, bool condensed) const {
	if (ternary) {
		os << "(";
		condition->print(os, indent, debug);
		os << " ? ";
		then->instructions[0]->print(os, indent, debug, condensed);
		os << " : ";
		elze->instructions[0]->print(os, indent, debug, condensed);
		os << ")";
	} else {
		os << "if ";
		condition->print(os, indent + 1, debug);
		os << " ";
		then->print(os, indent, debug);
		if (elze != nullptr) {
			os << " else ";
			elze->print(os, indent, debug);
		}
	}
	if (debug) {
		os << " " << type;
	}
	for (const auto& phi : phis) {
		os << std::endl << tabs(indent) << phi->variable << " = phi(" << phi->variable1 << " " << phi->variable1->type << ", " << phi->variable2 << " " << phi->variable2->type << ") " << phi->variable->type;
	}
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
	if (elze) {
		phis = Phi::build_phis(analyzer, then.get(), elze.get());
	} else {
		phis = Phi::build_phis(analyzer, analyzer->current_block(), then.get());
	}
}

void If::analyze(SemanticAnalyzer* analyzer) {
	// std::cout << "If " << is_void << std::endl;
	
	condition->analyze(analyzer);
	throws = condition->throws;

	then->is_void = is_void;
	then->analyze(analyzer);

	type = then->type;

	if (elze != nullptr) {
		elze->is_void = is_void;
		elze->analyze(analyzer);
		if (type->is_void() and not elze->type->is_void() and not then->returning) {
			type = Type::null;
		}
		type = type->operator + (elze->type);
	} else if (not type->is_void() or then->returning) {
		type = type->operator + (Type::null);
	}
	if (is_void) type = Type::void_;
	throws |= then->throws or (elze != nullptr and elze->throws);
	returning = then->returning and (elze != nullptr and elze->returning);
	may_return = then->may_return or (elze != nullptr and elze->may_return);
	return_type = return_type->operator + (then->return_type);
	if (elze != nullptr) return_type = return_type->operator + (elze->return_type);

	for (const auto& phi : phis) {
		phi->variable->type = phi->variable1->type->operator + (phi->variable2->type);
	}
}

Compiler::value If::compile(Compiler& c) const {
	
	for (const auto& phi : phis) {
		if (phi->variable1->block->branch != then->branch) {
			// std::cout << "Variable export last value for phi " << phi->variable1 << std::endl;
			phi->value1 = c.insn_convert(c.insn_load(phi->variable1->val), phi->variable->type);
		}
	}

	auto label_then = c.insn_init_label("then");
	auto label_else = c.insn_init_label("else");
	auto label_end = c.insn_init_label("end");
	Compiler::value then_v;
	Compiler::value else_v;
	bool compile_elze = elze != nullptr or not type->is_void();

	auto cond = condition->compile(c);
	auto cond_boolean = c.insn_to_bool(cond);
	condition->compile_end(c);
	c.insn_delete_temporary(cond);
	c.insn_if_new(cond_boolean, &label_then, compile_elze ? &label_else : &label_end);

	c.insn_label(&label_then);

	then_v = c.insn_convert(then->compile(c), type->fold());
	if (!then_v.v) then_v = c.insn_convert(c.new_null(), type->fold());
	then->compile_end(c);

	c.insn_branch(&label_end);
	label_then.block = c.builder.GetInsertBlock();

	if (compile_elze) {
		c.insn_label(&label_else);

		if (elze != nullptr) {
			else_v = c.insn_convert(elze->compile(c), type->fold());
			elze->compile_end(c);
		} else if (not type->is_void()) {
			else_v = c.insn_convert(c.new_null(), type->fold());
		}

		c.insn_branch(&label_end);
		label_else.block = c.builder.GetInsertBlock();
	}

	c.insn_label(&label_end);
	
	auto r = [&]() -> Compiler::value { if (type->is_void()) {
		return {};
	} else {
		return c.insn_phi(type, then_v, label_then, else_v, label_else);
	}}();

	for (const auto& phi : phis) {
		// std::cout << "phi " << phi->variable1 << " " << phi->value1.v << " " << phi->variable2 << " " << phi->value2.v << std::endl;
		if (phi->variable1->root == phi->variable2->root and phi->variable1->type == phi->variable2->type) {
			phi->variable->val = phi->variable1->val;
		} else {
			auto phi_node = c.insn_phi(phi->variable->type, c.insn_convert(phi->value1, phi->variable->type), phi->block1, c.insn_convert(phi->value2, phi->variable->type), phi->block2);
			phi->variable->val = c.create_entry(phi->variable->name, phi->variable->type);
			c.insn_store(phi->variable->val, phi_node);
		}
		if (phi->variable->phi and phi->variable->phi->variable2 == phi->variable) {
			// std::cout << vv->var->phi->variable1 << " " << vv->var->phi->variable2 << std::endl;
			// std::cout << vv->var->phi->value1.t << std::endl;
			// std::cout << "phi delete unused var " << phi->variable->phi->variable1 << std::endl;
			c.insn_delete_temporary(phi->variable->phi->value1);
			// c.insn_delete_variable(phi->variable->phi->variable1->val);
		}
	}

	c.current_block()->blocks.push_back(label_end.block);

	return r;
}

std::unique_ptr<Value> If::clone() const {
	auto iff = std::make_unique<If>();
	iff->condition = condition->clone();
	iff->then = unique_static_cast<Block>(then->clone());
	iff->elze = elze ? unique_static_cast<Block>(elze->clone()) : nullptr;
	iff->ternary = ternary;
	return iff;
}

}
