#include "ClassDeclaration.hpp"
#include "../semantic/Variable.hpp"

namespace ls {

ClassDeclaration::ClassDeclaration(Environment& env, Token* token) : Instruction(env), token(token) {
	name = token->content;
	var = nullptr;
}

void ClassDeclaration::print(std::ostream& os, int indent, PrintOptions options) const {
	os << "class " << name << " {" << std::endl;
	for (const auto& vd : fields) {
		os << tabs(indent + 1);
		vd->print(os, indent + 1, options);
		os << std::endl;
	}
	os << tabs(indent) << "}";
	if (options.debug) os << " " << type;
}

Location ClassDeclaration::location() const {
	return token->location;
}

void ClassDeclaration::pre_analyze(SemanticAnalyzer* analyzer) {
	auto& env = analyzer->env;
	var = analyzer->add_var(token, env.clazz(), nullptr);
	for (const auto& vd : fields) {
		vd->pre_analyze(analyzer);
	}
}

void ClassDeclaration::analyze(SemanticAnalyzer* analyzer, const Type*) {
	auto& env = analyzer->env;
	// TODO declare in pre_analyze
	for (const auto& vd : fields) {
		vd->analyze(analyzer, env.any);
	}
}

#if COMPILER
Compiler::value ClassDeclaration::compile(Compiler& c) const {
	auto& env = c.env;
	auto clazz = c.new_class(name);
	for (const auto& vd : fields) {
		for (size_t i = 0; i < vd->variables.size(); ++i) {
			// std::cout << "Compile class field '" << vd->variables.at(i)->content << "' type " << vd->expressions.at(i)->type << std::endl;
			auto default_value = vd->expressions.at(i)->compile(c);
			default_value = c.insn_to_any(default_value);
			auto field_name = c.new_const_string(vd->variables.at(i)->content);
			c.insn_call(env.void_, {clazz, field_name, default_value}, "Class.add_field");
		}
	}
	var->create_entry(c);
	var->store_value(c, clazz);
	return clazz;
}
#endif

std::unique_ptr<Instruction> ClassDeclaration::clone(Block* parent) const {
	auto cd = std::make_unique<ClassDeclaration>(type->env, token);
	cd->name = name;
	for (const auto& f : fields) {
		cd->fields.push_back(unique_static_cast<VariableDeclaration>(f->clone(parent)));
	}
	return cd;
}

}
