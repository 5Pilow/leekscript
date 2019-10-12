#include "SemanticAnalyzer.hpp"
#include "../instruction/ExpressionInstruction.hpp"
#include "../Program.hpp"
#include "../Context.hpp"
#include "../error/Error.hpp"
#include "../instruction/VariableDeclaration.hpp"
#include "../../standard/Module.hpp"
#include <functional>
#include "Variable.hpp"
#include "FunctionVersion.hpp"

namespace ls {

SemanticAnalyzer::SemanticAnalyzer(Environment& env) : env(env) {
	program = nullptr;
	for (const auto& clazz : env.std.classes) {
		auto const_class = env.const_class(clazz.first);
		globals.insert({ clazz.first, std::make_unique<Variable>(clazz.first, VarScope::INTERNAL, const_class, 0, nullptr, nullptr, nullptr, nullptr, clazz.second->clazz.get()) });
	}
}

void SemanticAnalyzer::analyze(Program* program) {

	this->program = program;

	program->main->create_default_version(this);
	enter_function(program->main->default_version);
	enter_block(program->main->default_version->body.get());

	// Add context variables
	if (program->context) {
		for (auto& var : program->context->vars) {
			// std::cout << "Add context var " << var.first << std::endl;
			var.second.variable = add_var(new Token(TokenType::IDENT, program->main_file, 0, 0, 0, var.first), var.second.type, nullptr);
			// std::cout << "variable added " << var.second.variable << " " << (void*) var.second.variable << std::endl;
		}
	}
	leave_block();
	leave_function();

	program->main->analyze(this);
	program->functions = functions;
}

void SemanticAnalyzer::enter_function(FunctionVersion* f) {
	// std::cout << "enter function" << std::endl;
	blocks.push_back({});
	sections.push_back({});
	loops.push(0);
	functions_stack.push_back(f);
}

void SemanticAnalyzer::leave_function() {
	// std::cout << "leave function" << std::endl;
	blocks.pop_back();
	sections.pop_back();
	functions_stack.pop_back();
	loops.pop();
}

void SemanticAnalyzer::enter_block(Block* block) {
	// std::cout << "enter block" << std::endl;
	blocks.back().push_back(block);
}

void SemanticAnalyzer::leave_block() {
	// std::cout << "leave block" << std::endl;
	blocks.back().pop_back();
}

void SemanticAnalyzer::enter_section(Section* section) {
	// std::cout << "enter section" << std::endl;
	sections.back().push_back(section);
}

void SemanticAnalyzer::leave_section() {
	// std::cout << "leave section" << std::endl;
	sections.back().pop_back();
}

FunctionVersion* SemanticAnalyzer::current_function() const {
	return functions_stack.back();
}
Block* SemanticAnalyzer::current_block() const {
	return blocks.back().back();
}
Section* SemanticAnalyzer::current_section() const {
	return sections.back().back();
}

void SemanticAnalyzer::enter_loop() {
	loops.top()++;
}

void SemanticAnalyzer::leave_loop() {
	loops.top()--;
}

bool SemanticAnalyzer::in_loop(int deepness) const {
	return loops.top() >= deepness;
}

Variable* SemanticAnalyzer::get_var(const std::string& v) {

	// Search in global variables
	auto i = globals.find(v);
	if (i != globals.end()) {
		return i->second.get();
	}

	// Search operators
	if (auto op = program->get_operator(v)) {
		return op;
	}

	// Search recursively in the functions
	int f = functions_stack.size() - 1;
	while (f >= 0) {
		// Search in the function parameters
		const auto& arguments = functions_stack[f]->arguments;
		auto i = arguments.find(v);
		if (i != arguments.end()) {
			return i->second;
		}
		// Search in the function captures
		// const auto& captures = functions_stack[f]->captures_map;
		// i = captures.find(v->content);
		// if (i != captures.end()) {
		// 	return i->second;
		// }

		// Search in the local variables of the function
		const auto& fvars = blocks[f];
		int b = fvars.size() - 1;
		while (b >= 0) {
			int s = fvars[b]->sections.size() - 1;
			while (s >= 0) {
				const auto& vars = fvars[b]->sections[s]->variables;
				// std::cout << "Block variables : ";
				// for (const auto& v : vars) std::cout << v.first << " " << v.second << ", ";
				// std::cout << std::endl;
				auto i = vars.find(v);
				if (i != vars.end()) {
					return i->second;
				}
				s--;
			}
			b--;
		}
		f--;
	}
	return nullptr;
}

Variable* SemanticAnalyzer::add_var(Token* v, const Type* type, Value* value) {
	if (globals.find(v->content) != globals.end()) {
		add_error({Error::Type::VARIABLE_ALREADY_DEFINED, v->location, v->location, {v->content}});
		return nullptr;
	}
	for (const auto& section : blocks.back().back()->sections) {
		if (section->variables.find(v->content) != section->variables.end()) {
			add_error({Error::Type::VARIABLE_ALREADY_DEFINED, v->location, v->location, {v->content}});
			return nullptr;
		}
	}
	return sections.back().back()->variables.insert(std::pair<std::string, Variable*>(
		v->content,
		new Variable(v->content, VarScope::LOCAL, type, 0, value, current_function(), current_block(), current_section(), nullptr)
	)).first->second;
}

Variable* SemanticAnalyzer::add_global_var(Token* v, const Type* type, Value* value) {
	// std::cout << "blocks " << blocks.size() << std::endl;
	for (const auto& section : blocks.begin()->front()->sections) {
		auto& vars = section->variables;
		if (vars.find(v->content) != vars.end()) {
			add_error({Error::Type::VARIABLE_ALREADY_DEFINED, v->location, v->location, {v->content}});
			return nullptr;
		}
	}
	return blocks.begin()->front()->sections.front()->variables.insert(std::pair<std::string, Variable*>(
		v->content,
		new Variable(v->content, VarScope::LOCAL, type, 0, value, current_function(), current_block(), current_section(), nullptr)
	)).first->second;
}

void SemanticAnalyzer::add_function(Function* l) {
	functions.push_back(l);
}

Variable* SemanticAnalyzer::convert_var_to_any(Variable* var) {
	// std::cout << "SemanticAnalyser::convert_var_to_any(" << var->name << ")" << std::endl;
	if (var->type->is_polymorphic()) return var;
	auto new_var = new Variable(var->name, var->scope, env.any, 0, nullptr, var->function, var->block, var->section, nullptr);
	// Search recursively in the functions

	int f = functions_stack.size() - 1;
	while (f >= 0) {
		// Search in the function parameters
		auto& params = functions_stack.at(f)->arguments;
		if (params.find(var->name) != params.end()) {
			params.at(var->name) = new_var;
		}
		// Search in the local variables of the function
		int b = blocks.at(f).size() - 1;
		while (b >= 0) {
			for (const auto& section : blocks.at(f).at(b)->sections) {
				auto& vars = section->variables;
				if (vars.find(var->name) != vars.end()) {
					vars.at(var->name) = new_var;
					break;
				}
			}
			b--;
		}
		f--;
	}
	return new_var;
}

Variable* SemanticAnalyzer::update_var(Variable* variable) {
	// std::cout << "update_var " << variable << " " << (int) variable->scope << std::endl;
	Variable* new_variable;
	if (current_block() == variable->block) {
		// std::cout << "same block" << std::endl;
		/* Same block */
		// var a = 12
		// a.1 = 5.5
		// a.2 = 'salut'
		auto root = variable->root ? variable->root : variable;
		new_variable = new Variable(root->name, variable->scope, env.any, root->index, nullptr, current_function(), current_block(), current_section(), nullptr);
		new_variable->parent = variable;
		new_variable->id = ++root->generator;
		new_variable->root = root;
	} else {
		// std::cout << "branch" << std::endl;
		/* Branch */
		// var a = 12
		// a.1 = 5.5
		// if (...) {
		//    a.1.1 = 'salut'
		// }
		auto root = variable->root ? variable->root : variable;
		new_variable = new Variable(variable->name, variable->scope, env.any, variable->index, nullptr, current_function(), current_block(), current_section(), nullptr);
		new_variable->id = ++variable->generator;
		new_variable->parent = variable;
		new_variable->root = root;
	}
	if (variable->scope == VarScope::PARAMETER) {
		// std::cout << "update argument " << new_variable->name << std::endl;
		current_function()->arguments[new_variable->name] = new_variable;
	} else {
		current_section()->variables[new_variable->name] = new_variable;
	}
	current_block()->mutations.push_back(new_variable);
	return new_variable;
}

void SemanticAnalyzer::add_error(Error ex) {
	ex.underline_code = program->underline_code(ex.location, ex.focus);
	errors.push_back(ex);
}

} // end of namespace ls
