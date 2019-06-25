#include "SemanticAnalyzer.hpp"
#include "../../compiler/instruction/ExpressionInstruction.hpp"
#include "../../vm/Program.hpp"
#include "../../vm/Context.hpp"
#include "../error/Error.hpp"
#include "../instruction/VariableDeclaration.hpp"
#include "../../vm/Module.hpp"
#include <functional>
#include "Variable.hpp"
#include "FunctionVersion.hpp"

namespace ls {

SemanticAnalyzer::SemanticAnalyzer() {
	program = nullptr;
}

void SemanticAnalyzer::analyze(Program* program, Context* context) {

	this->program = program;

	program->main->create_default_version(this);
	enter_function(program->main->default_version);
	enter_block(program->main->default_version->body.get());

	// Add context variables
	if (context) {
		for (auto var : context->vars) {
			// std::cout << "Add context var " << var.first << std::endl;
			add_var(new Token(TokenType::IDENT, program->main_file, 0, 0, 0, var.first), var.second.type, nullptr);
		}
	}
	leave_block();
	leave_function();

	program->analyze(this);
	program->functions = functions;
}

void SemanticAnalyzer::enter_function(FunctionVersion* f) {
	// std::cout << "enter function" << std::endl;
	blocks.push_back({});
	loops.push(0);
	functions_stack.push_back(f);
}

void SemanticAnalyzer::leave_function() {
	// std::cout << "leave function" << std::endl;
	blocks.pop_back();
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

FunctionVersion* SemanticAnalyzer::current_function() const {
	return functions_stack.back();
}
Block* SemanticAnalyzer::current_block() const {
	return blocks.back().back();
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

Variable* SemanticAnalyzer::get_var(Token* v) {

	// Search in interval variables : global for the program
	auto i = vm->internal_vars.find(v->content);
	if (i != vm->internal_vars.end()) {
		return i->second.get();
	}

	// Search operators
	if (auto op = program->get_operator(v->content)) {
		return op;
	}

	// Search recursively in the functions
	int f = functions_stack.size() - 1;
	while (f >= 0) {
		// Search in the function parameters
		const auto& arguments = functions_stack[f]->arguments;
		auto i = arguments.find(v->content);
		if (i != arguments.end()) {
			return i->second;
		}

		// Search in the local variables of the function
		const auto& fvars = blocks[f];
		int b = fvars.size() - 1;
		while (b >= 0) {
			const auto& vars = fvars[b]->variables;
			auto i = vars.find(v->content);
			if (i != vars.end()) {
				return i->second;
			}
			b--;
		}
		f--;
	}
	return nullptr;
}

Variable* SemanticAnalyzer::add_var(Token* v, const Type* type, Value* value) {
	if (vm->internal_vars.find(v->content) != vm->internal_vars.end()) {
		add_error({Error::Type::VARIABLE_ALREADY_DEFINED, v->location, v->location, {v->content}});
		return nullptr;
	}
	if (blocks.back().back()->variables.find(v->content) != blocks.back().back()->variables.end()) {
		add_error({Error::Type::VARIABLE_ALREADY_DEFINED, v->location, v->location, {v->content}});
		return nullptr;
	}
	blocks.back().back()->variables.insert(std::pair<std::string, Variable*>(
		v->content,
		new Variable(v->content, VarScope::LOCAL, type, 0, value, current_function(), nullptr)
	));
	return blocks.back().back()->variables.at(v->content);
}

Variable* SemanticAnalyzer::add_global_var(Token* v, const Type* type, Value* value) {
	// std::cout << "blocks " << blocks.size() << std::endl;
	auto& vars = (*blocks.begin()->begin())->variables;
	if (vars.find(v->content) != vars.end()) {
		add_error({Error::Type::VARIABLE_ALREADY_DEFINED, v->location, v->location, {v->content}});
		return nullptr;
	}
	vars.insert(std::pair<std::string, Variable*>(
		v->content,
		new Variable(v->content, VarScope::LOCAL, type, 0, value, (*functions.begin())->default_version, nullptr)
	));
	return vars.at(v->content);
}

void SemanticAnalyzer::add_function(Function* l) {
	functions.push_back(l);
}

Variable* SemanticAnalyzer::convert_var_to_any(Variable* var) {
	// std::cout << "SemanticAnalyser::convert_var_to_any(" << var->name << ")" << std::endl;
	if (var->type->is_polymorphic()) return var;
	auto new_var = new Variable(var->name, var->scope, Type::any, 0, nullptr, var->function, nullptr);
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
			auto& vars = blocks.at(f).at(b)->variables;
			if (vars.find(var->name) != vars.end()) {
				vars.at(var->name) = new_var;
			}
			b--;
		}
		f--;
	}
	return new_var;
}

void SemanticAnalyzer::add_error(Error ex) {
	ex.underline_code = program->underline_code(ex.location, ex.focus);
	errors.push_back(ex);
}

} // end of namespace ls
