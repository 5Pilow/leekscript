#include "VariableDeclaration.hpp"
#include "../../vm/LSValue.hpp"
#include "../../vm/value/LSNull.hpp"
#include "../semantic/SemanticAnalyzer.hpp"
#include "../error/Error.hpp"
#include "../value/Function.hpp"
#include "../value/Nulll.hpp"
#include "../semantic/Variable.hpp"

namespace ls {

VariableDeclaration::VariableDeclaration() {
	global = false;
	constant = false;
}

void VariableDeclaration::print(std::ostream& os, int indent, bool debug, bool condensed) const {

	os << (global ? "global " : (constant ? "let " : "var "));

	for (unsigned i = 0; i < variables.size(); ++i) {
		auto name = variables.at(i)->content;
		auto v = vars.find(name);
		if (v != vars.end()) {
			os << v->second;
		} else {
			os << name;
		}
		if (expressions[i] != nullptr) {
			os << " = ";
			expressions.at(i)->print(os, indent, debug);
		}
		if (i < variables.size() - 1) {
			os << ", ";
		}
	}
}

Location VariableDeclaration::location() const {
	auto end = variables.size() > expressions.size() ? variables.back()->location.end : expressions.back()->location().end;
	return {keyword->location.file, keyword->location.start, end};
}

void VariableDeclaration::analyze_global_functions(SemanticAnalyzer* analyzer) const {
	if (global && function) {
		auto var = variables.at(0);
		const auto& expr = expressions.at(0);
		auto v = analyzer->add_global_var(var, Type::fun(), expr.get());
		((VariableDeclaration*) this)->vars.insert({ var->content, v });
	}
}

void VariableDeclaration::pre_analyze(SemanticAnalyzer* analyzer) {
	if (global && function) return;
	for (unsigned i = 0; i < variables.size(); ++i) {
		auto& var = variables.at(i);
		auto type = (dynamic_cast<Function*>(expressions[i].get())) ? Type::fun() : Type::any; // Set type in pre analyze to avoid capture functions
		auto v = analyzer->add_var(var, type, expressions.at(i).get());
		if (v) vars.insert({ var->content, v });
		if (expressions[i] != nullptr) {
			expressions[i]->pre_analyze(analyzer);
		}
	}
}

void VariableDeclaration::analyze(SemanticAnalyzer* analyzer, const Type*) {

	type = Type::void_;
	throws = false;

	for (unsigned i = 0; i < variables.size(); ++i) {
		auto& var = variables.at(i);
		auto vi = vars.find(var->content);
		if (vi == vars.end()) continue;
		auto v = vi->second;
		if (expressions[i]) {
			if (Function* f = dynamic_cast<Function*>(expressions[i].get())) {
				f->name = var->content;
			}
			expressions[i]->analyze(analyzer);
			v->value = expressions[i].get();
			throws |= expressions[i]->throws;
		} else {
			v->value = new Nulll(nullptr);
		}
		if (v->value->type->is_void()) {
			analyzer->add_error({Error::Type::CANT_ASSIGN_VOID, location(), var->location, {var->content}});
		} else {
			v->type = Variable::get_type_for_variable_from_expression(v->value->type);
			if (constant) v->type = v->type->add_constant();
		}
		vars.insert({var->content, v});
	}
}

Compiler::value VariableDeclaration::compile(Compiler& c) const {
	for (unsigned i = 0; i < variables.size(); ++i) {
		const auto& name = variables[i]->content;
		const auto& variable = vars.at(name);
		if (expressions[i] != nullptr) {
			const auto& ex = expressions[i];
			if (dynamic_cast<Function*>(ex.get()) and not ex->type->is_closure()) {
				continue;
			}
			auto val = ex->compile(c);
			if (!val.t->reference) {
				val = c.insn_move_inc(val);
			}
			variable->create_entry(c);
			variable->store_value(c, val);
			ex->compile_end(c);
		} else {
			variable->create_entry(c);
			variable->store_value(c, c.new_null());
		}
	}
	return {};
}

std::unique_ptr<Instruction> VariableDeclaration::clone() const {
	auto vd = std::make_unique<VariableDeclaration>();
	vd->keyword = keyword;
	vd->global = global;
	vd->constant = constant;
	vd->function = function;
	for (const auto& v : variables) {
		vd->variables.push_back(v);
	}
	for (const auto& v : expressions) {
		vd->expressions.push_back(v ? v->clone() : nullptr);
	}
	return vd;
}

}
