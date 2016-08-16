#include "../../compiler/semantic/SemanticAnalyser.hpp"

#include "../../compiler/instruction/ExpressionInstruction.hpp"
#include "../../vm/Program.hpp"
#include "../../vm/Context.hpp"
#include "../../vm/standard/NullSTD.hpp"
#include "../../vm/standard/NumberSTD.hpp"
#include "../../vm/standard/BooleanSTD.hpp"
#include "../../vm/standard/StringSTD.hpp"
#include "../../vm/standard/ArraySTD.hpp"
#include "../../vm/standard/MapSTD.hpp"
#include "../../vm/standard/ObjectSTD.hpp"
#include "../../vm/standard/SystemSTD.hpp"
#include "../../vm/standard/FunctionSTD.hpp"
#include "../../vm/standard/ClassSTD.hpp"
#include "SemanticException.hpp"
#include "../instruction/VariableDeclaration.hpp"

using namespace std;

namespace ls {

SemanticAnalyser::SemanticAnalyser() {
	program = nullptr;
	in_program = false;
//	loops.push(0);
//	variables.push_back(vector<map<std::string, SemanticVar*>> {});
//	functions_stack.push(nullptr); // The first function is the main function of the program
//	parameters.push_back(map<std::string, SemanticVar*> {});
}

SemanticAnalyser::~SemanticAnalyser() {}

void SemanticVar::will_take(SemanticAnalyser* analyser, const std::vector<Type>& args_type) {
	if (value != nullptr) {
		value->will_take(analyser, args_type);
		type.will_take(args_type);
	}
}

void SemanticVar::will_take_element(SemanticAnalyser* analyser, const Type& type) {
	if (value != nullptr) {
		value->will_take_element(analyser, type);
		this->type.will_take_element(type);
	}
}

void SemanticVar::must_be_pointer(SemanticAnalyser* analyser) {
	if (value != nullptr) {
		value->must_be_pointer(analyser);
		this->type.nature = Nature::POINTER;
	}
}

extern LSValue* jit_add(LSValue* x, LSValue* y);
extern LSValue* jit_sub(LSValue* x, LSValue* y);
extern LSValue* jit_mul(LSValue* x, LSValue* y);
extern LSValue* jit_div(LSValue* x, LSValue* y);
extern LSValue* jit_pow(LSValue* x, LSValue* y);
extern LSValue* jit_mod(LSValue* x, LSValue* y);

void SemanticAnalyser::analyse(Program* program, Context* context, std::vector<Module*>& modules) {

	this->program = program;

	enter_function(program->main);

	// Add context variables
	for (auto var : context->vars) {
		add_var(new Token(var.first), Type(var.second->getRawType(), Nature::POINTER), nullptr, nullptr);
	}

	Type op_type = Type(RawType::FUNCTION, Nature::POINTER);
	op_type.setArgumentType(0, Type::POINTER);
	op_type.setArgumentType(1, Type::POINTER);
	op_type.setReturnType(Type::POINTER);
	program->system_vars.insert(pair<string, LSValue*>("+", new LSFunction((void*) &jit_add, 1, true)));
	add_var(new Token("+"), op_type, nullptr, nullptr);
	program->system_vars.insert(pair<string, LSValue*>("-", new LSFunction((void*) &jit_sub, 1, true)));
	add_var(new Token("-"), op_type, nullptr, nullptr);
	program->system_vars.insert(pair<string, LSValue*>("*", new LSFunction((void*) &jit_mul, 1, true)));
	add_var(new Token("*"), op_type, nullptr, nullptr);
	program->system_vars.insert(pair<string, LSValue*>("×", new LSFunction((void*) &jit_mul, 1, true)));
	add_var(new Token("×"), op_type, nullptr, nullptr);
	program->system_vars.insert(pair<string, LSValue*>("/", new LSFunction((void*) &jit_div, 1, true)));
	add_var(new Token("/"), op_type, nullptr, nullptr);
	program->system_vars.insert(pair<string, LSValue*>("÷", new LSFunction((void*) &jit_div, 1, true)));
	add_var(new Token("÷"), op_type, nullptr, nullptr);
	program->system_vars.insert(pair<string, LSValue*>("**", new LSFunction((void*) &jit_pow, 1, true)));
	add_var(new Token("**"), op_type, nullptr, nullptr);
	program->system_vars.insert(pair<string, LSValue*>("%", new LSFunction((void*) &jit_mod, 1, true)));
	add_var(new Token("%"), op_type, nullptr, nullptr);

	NullSTD().include(this, program);
	BooleanSTD().include(this, program);
	NumberSTD().include(this, program);
	StringSTD().include(this, program);
	ArraySTD().include(this, program);
	MapSTD().include(this, program);
	ObjectSTD().include(this, program);
	FunctionSTD().include(this, program);
	ClassSTD().include(this, program);
	SystemSTD().include(this, program);

	for (Module* module : modules) {
		module->include(this, program);
	}

	in_program = true;

	program->main->type.setReturnType(Type::UNKNOWN);
	program->main->body->analyse(this, Type::UNKNOWN);
	if (program->main->type.return_types.size() > 1) { // the body contains return instruction
		Type return_type = program->main->body->type == Type::VOID ? Type::UNKNOWN : program->main->body->type;
		for (size_t i = 1; i < program->main->type.return_types.size(); ++i) {
			return_type = Type::get_compatible_type(return_type, program->main->type.return_types[i]);
		}
		program->main->type.return_types.clear();
		program->main->type.setReturnType(return_type);
		program->main->body->analyse(this, return_type); // second pass
	} else {
		program->main->type.setReturnType(program->main->body->type);
	}

	program->functions = functions;
}

void SemanticAnalyser::enter_function(Function* f) {

	// Create function scope
	variables.push_back(vector<map<std::string, SemanticVar*>> {});
	// First function block
	variables.back().push_back(map<std::string, SemanticVar*> {});
	// Parameters
	parameters.push_back(map<std::string, SemanticVar*> {});

	loops.push(0);
	functions_stack.push(f);
}

void SemanticAnalyser::leave_function() {

	variables.pop_back();
	parameters.pop_back();
	functions_stack.pop();
	loops.pop();
}

void SemanticAnalyser::enter_block() {
	variables.back().push_back(map<std::string, SemanticVar*> {});
}

void SemanticAnalyser::leave_block() {
	variables.back().pop_back();
}

Function* SemanticAnalyser::current_function() const {
	if (functions_stack.empty()) {
		return nullptr;
	}
	return functions_stack.top();
}

void SemanticAnalyser::enter_loop() {
	loops.top()++;
}

void SemanticAnalyser::leave_loop() {
	loops.top()--;
}

bool SemanticAnalyser::in_loop(int deepness) const {
	return loops.top() >= deepness;
}

SemanticVar* SemanticAnalyser::add_parameter(Token* v, Type type) {

	SemanticVar* arg = new SemanticVar(v->content, VarScope::PARAMETER, type, parameters.back().size(), nullptr, nullptr, current_function());
	parameters.back().insert(pair<string, SemanticVar*>(v->content, arg));
	return arg;
}

SemanticVar* SemanticAnalyser::get_var(Token* v) {

	// Search in interval variables : global for the program
	try {
		return internal_vars.at(v->content);
	} catch (exception& e) {}

	// Search recursively in the functions
	int f = functions_stack.size() - 1;
	while (f >= 0) {
		// Search in the function parameters
		try {
			return parameters.at(f).at(v->content);
		} catch (exception& e) {}

		// Search in the local variables of the function
		int b = variables.at(f).size() - 1;
		while (b >= 0) {
			try {
				return variables.at(f).at(b).at(v->content);
			} catch (exception& e) {}
			b--;
		}
		f--;
	}
	add_error({SemanticException::Type::UNDEFINED_VARIABLE, v->line, v->content});
	return nullptr;
}

SemanticVar* SemanticAnalyser::get_var_direct(std::string name) {
	try {
		if (variables.size() > 0) {
			return variables.back().back().at(name);
		}
	} catch (exception& e) {}
	return nullptr;
}

SemanticVar* SemanticAnalyser::add_var(Token* v, Type type, Value* value, VariableDeclaration* vd) {

	// Internal variable, before execution
	if (!in_program) {
		internal_vars.insert(pair<string, SemanticVar*>(
			v->content,
			new SemanticVar(v->content, VarScope::INTERNAL, type, 0, value, vd, current_function())
		));
		return internal_vars.at(v->content);
	}

	if (variables.back().back().find(v->content) != variables.back().back().end()) {
		add_error({SemanticException::Type::VARIABLE_ALREADY_DEFINED, v->line, v->content});
	}
	variables.back().back().insert(pair<string, SemanticVar*>(
		v->content,
		new SemanticVar(v->content, VarScope::LOCAL, type, 0, value, vd, current_function())
	));
	return variables.back().back().at(v->content);
}

void SemanticAnalyser::add_function(Function* l) {
	functions.push_back(l);
}

map<string, SemanticVar*>& SemanticAnalyser::get_local_vars() {
	return variables.back().back();
}

void SemanticAnalyser::add_error(SemanticException ex) {
	errors.push_back(ex);
}

} // end of namespace ls
