#include "VariableDeclaration.hpp"
#include "../semantic/SemanticAnalyzer.hpp"
#include "../error/Error.hpp"
#include "../value/Function.hpp"
#include "../value/Nulll.hpp"
#include "../semantic/Variable.hpp"

namespace ls {

VariableDeclaration::VariableDeclaration(Environment& env) : Instruction(env) {
	global = false;
	constant = false;
}

void VariableDeclaration::print(std::ostream& os, int indent, PrintOptions options) const {

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
			expressions.at(i)->print(os, indent, options);
		}
		if (i < variables.size() - 1) {
			os << ", ";
		}
	}
}

Location VariableDeclaration::location() const {
	auto end = [&]() {
		if (expressions.back() != nullptr) {
			return expressions.back()->location().end;
		} else {
			return variables.back()->location.end;
		}
	}();
	return { keyword->location.file, keyword->location.start, end };
}

void VariableDeclaration::set_end_section(Section* end_section) {
	assert(expressions.size());
	expressions.back()->set_end_section(end_section);
}

void VariableDeclaration::analyze_global_functions(SemanticAnalyzer* analyzer) const {
	if (global) {
		auto& env = analyzer->env;
		for (unsigned i = 0; i < variables.size(); ++i) {
			auto& var = variables.at(i);
			if (function) {
				const auto& expr = expressions.at(i);
				auto v = analyzer->add_global_var(var, Type::fun(env.void_, {}), expr.get());
				((VariableDeclaration*) this)->global_vars.insert({ var->content, v });
				if (Function* f = dynamic_cast<Function*>(expr.get())) {
					f->name = var->content;
				}
				expr->pre_analyze(analyzer);
			} else {
				const auto& expr = expressions.at(i);
				auto v = analyzer->add_global_var(var, env.any, expr.get());
				if (expr) {
					expr->pre_analyze(analyzer);
				}
				((VariableDeclaration*) this)->global_vars.insert({ var->content, v });
			}
		}
	}
}

void VariableDeclaration::pre_analyze(SemanticAnalyzer* analyzer) {
	// vars.clear();
	if (global && function) return;
	auto& env = analyzer->env;
	for (unsigned i = 0; i < variables.size(); ++i) {
		auto& var = variables.at(i);
		if (vars.find(var->content) == vars.end()) {
			auto type = (dynamic_cast<Function*>(expressions[i].get())) ? Type::fun(env.void_, {}) : env.any; // Set type in pre analyze to avoid capture functions
			auto v = analyzer->add_var(var, type, expressions.at(i).get());
			if (v) vars.insert({ var->content, v });
		} else {
			analyzer->add_var(var, vars.at(var->content));
		}
		if (expressions[i] != nullptr) {
			if (Function* f = dynamic_cast<Function*>(expressions[i].get())) {
				f->name = var->content;
			}
			expressions[i]->pre_analyze(analyzer);
		}
	}
}

void VariableDeclaration::analyze(SemanticAnalyzer* analyzer, const Type*) {
	auto& env = analyzer->env;

	type = env.void_;
	throws = false;

	for (unsigned i = 0; i < variables.size(); ++i) {
		auto& var = variables.at(i);
		auto& variables = (global ? global_vars : vars);
		auto vi = variables.find(var->content);
		if (vi == variables.end()) continue;
		auto v = vi->second;
		if (expressions[i]) {
			expressions[i]->analyze(analyzer);
			v->value = expressions[i].get();
			throws |= expressions[i]->throws;
		}
		if (v->value and v->value->type->is_void()) {
			analyzer->add_error({Error::Type::CANT_ASSIGN_VOID, location(), var->location, {var->content}});
		} else {
			v->type = Variable::get_type_for_variable_from_expression(env, v->value);
			if (constant) v->type = v->type->add_constant();
		}
		vars.insert({var->content, v});
		// std::cout << "VD type " << v << " " << (void*) v << " " << v->type << std::endl;
	}
}

Completion VariableDeclaration::autocomplete(SemanticAnalyzer& analyzer, size_t position) const {
	if (expressions.size()) {
		return expressions.front()->autocomplete(analyzer, position);
	}
	return { analyzer.env };
}

Hover VariableDeclaration::hover(SemanticAnalyzer& analyzer, size_t position) const {
	for (const auto& variable : variables) {
		if (variable->location.contains(position)) {
			return { (global ? global_vars : vars).at(variable->content)->type, variable->location };
		}
	}
	for (const auto& expression : expressions) {
		if (expression->location().contains(position)) {
			return expression->hover(analyzer, position);
		}
	}
	return { type, location() };
}

#if COMPILER
Compiler::value VariableDeclaration::compile(Compiler& c) const {
	for (unsigned i = 0; i < variables.size(); ++i) {
		const auto& name = variables[i]->content;
		const auto& variable = (global ? global_vars : vars).at(name);
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
	return { c.env };
}
#endif

std::unique_ptr<Instruction> VariableDeclaration::clone(Block* parent) const {
	auto vd = std::make_unique<VariableDeclaration>(type->env);
	vd->keyword = keyword;
	vd->global = global;
	vd->constant = constant;
	vd->function = function;
	for (const auto& v : variables) {
		vd->variables.push_back(v);
	}
	for (const auto& v : expressions) {
		vd->expressions.push_back(v ? v->clone(parent) : nullptr);
	}
	return vd;
}

}
