#include <sstream>
#include <chrono>
#include "VM.hpp"
#include "../constants.h"
#include "../colors.h"
#include "../compiler/lexical/LexicalAnalyser.hpp"
#include "../compiler/syntaxic/SyntaxicAnalyser.hpp"
#include "Context.hpp"
#include "../compiler/semantic/SemanticAnalyser.hpp"
#include "../compiler/semantic/SemanticError.hpp"
#include "value/LSNumber.hpp"
#include "value/LSArray.hpp"
#include "Program.hpp"
#include "value/LSObject.hpp"
#include "value/LSFunction.hpp"
#include "standard/ValueSTD.hpp"
#include "standard/NullSTD.hpp"
#include "standard/NumberSTD.hpp"
#include "standard/BooleanSTD.hpp"
#include "standard/StringSTD.hpp"
#include "standard/ArraySTD.hpp"
#include "standard/MapSTD.hpp"
#include "standard/SetSTD.hpp"
#include "standard/ObjectSTD.hpp"
#include "standard/SystemSTD.hpp"
#include "standard/FunctionSTD.hpp"
#include "standard/ClassSTD.hpp"
#include "standard/IntervalSTD.hpp"
#include "standard/JsonSTD.hpp"
#include "legacy/Functions.hpp"
#include "../compiler/semantic/Callable.hpp"
#include "../compiler/value/Expression.hpp"
#include "../compiler/instruction/ExpressionInstruction.hpp"
#include "../compiler/value/VariableValue.hpp"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Function.h"
#include "llvm/IRReader/IRReader.h"
#include "llvm/Support/SourceMgr.h"
#include "llvm/Support/raw_ostream.h"

namespace ls {

const unsigned long int VM::DEFAULT_OPERATION_LIMIT = 20000000;
VM* VM::current_vm = nullptr;
OutputStream* VM::default_output = new OutputStream();

LSValue* ptr_fun(LSValue* v) {
	return v->move();
}

VM::VM(bool v1) : compiler(this) {

	operation_limit = VM::DEFAULT_OPERATION_LIMIT;

	null_value = LSNull::create();
	true_value = LSBoolean::create(true);
	false_value = LSBoolean::create(false);
	LSNull::set_null_value(this->null_value);
	LSBoolean::set_true_value(this->true_value);
	LSBoolean::set_false_value(this->false_value);

	// Include STD modules
	add_module(new ValueSTD());
	add_module(new NullSTD());
	add_module(new BooleanSTD());
	add_module(new NumberSTD());
	add_module(new StringSTD());
	add_module(new ArraySTD());
	add_module(new MapSTD());
	add_module(new SetSTD());
	add_module(new ObjectSTD());
	add_module(new FunctionSTD());
	add_module(new ClassSTD());
	add_module(new SystemSTD());
	add_module(new IntervalSTD());
	add_module(new JsonSTD());

	// Add function operators
	std::vector<std::string> ops = {"+", "-", "*", "×", "/", "÷", "**", "%", "\\", "~", ">", "<", ">=", "<="};
	std::vector<std::function<Compiler::value(Compiler&, std::vector<Compiler::value>)>> ops_funs = {
		[](Compiler& c, std::vector<Compiler::value> args) { return c.insn_add(args[0], args[1]); },
		[](Compiler& c, std::vector<Compiler::value> args) { return c.insn_sub(args[0], args[1]); },
		[](Compiler& c, std::vector<Compiler::value> args) { return c.insn_mul(args[0], args[1]); },
		[](Compiler& c, std::vector<Compiler::value> args) { return c.insn_mul(args[0], args[1]); },
		[](Compiler& c, std::vector<Compiler::value> args) { return c.insn_div(args[0], args[1]); },
		[](Compiler& c, std::vector<Compiler::value> args) { return c.insn_div(args[0], args[1]); },
		[](Compiler& c, std::vector<Compiler::value> args) { return c.insn_pow(args[0], args[1]); },
		[](Compiler& c, std::vector<Compiler::value> args) { return c.insn_mod(args[0], args[1]); },
		[](Compiler& c, std::vector<Compiler::value> args) { return c.insn_int_div(args[0], args[1]); },
		[](Compiler& c, std::vector<Compiler::value> args) { return c.insn_mod(args[0], args[1]); },
		[](Compiler& c, std::vector<Compiler::value> args) { return c.insn_gt(args[0], args[1]); },
		[](Compiler& c, std::vector<Compiler::value> args) { return c.insn_lt(args[0], args[1]); },
		[](Compiler& c, std::vector<Compiler::value> args) { return c.insn_ge(args[0], args[1]); },
		[](Compiler& c, std::vector<Compiler::value> args) { return c.insn_le(args[0], args[1]); },
	};
	std::vector<TokenType> token_types = {TokenType::PLUS, TokenType::MINUS, TokenType::TIMES, TokenType::TIMES, TokenType::DIVIDE, TokenType::DIVIDE, TokenType::POWER, TokenType::MODULO, TokenType::INT_DIV, TokenType::TILDE, TokenType::GREATER, TokenType::LOWER, TokenType::GREATER_EQUALS, TokenType::LOWER_EQUALS};
	
	auto value_class = internal_vars["Value"]->lsvalue;

	for (unsigned o = 0; o < ops.size(); ++o) {
		auto name = ops[o];
		auto f = new Function();
		f->addArgument(new Token(TokenType::IDENT, 0, 1, 0, "x"), nullptr);
		f->addArgument(new Token(TokenType::IDENT, 2, 1, 2, "y"), nullptr);
		f->body = new Block();
		auto ex = new Expression();
		ex->v1 = new VariableValue(std::make_shared<Token>(TokenType::IDENT, 0, 1, 0, "x"));
		ex->v2 = new VariableValue(std::make_shared<Token>(TokenType::IDENT, 2, 1, 2, "y"));
		ex->op = std::make_shared<Operator>(new Token(token_types[o], 1, 1, 1, name));
		f->body->instructions.push_back( new ExpressionInstruction(ex));
		auto type = Type::fun(Type::any(), {Type::any(), Type::any()});
		type.native = true;
		add_internal_var(name, type, f);
	}

	auto ptr_type = Type::fun(Type::any(), {Type::any()});
	auto fun = new LSFunction((void*) ptr_fun);
	fun->args = {value_class};
	fun->return_type = value_class;
	add_internal_var("ptr", ptr_type, fun);

	// Add v1 functions
	if (v1) {
		legacy::Functions::create(this);
	}
}

VM::~VM() {
	for (auto& module : modules) {
		delete module;
	}
	for (auto& fun : system_vars) {
		delete fun;
	}
	delete null_value;
	delete true_value;
	delete false_value;
}

VM* VM::current() {
	return current_vm;
}

void VM::static_init() {
	// Global initialization
	llvm::InitializeNativeTarget();
	llvm::InitializeNativeTargetAsmPrinter();
	llvm::InitializeNativeTargetAsmParser();
}

void VM::add_module(Module* m) {
	modules.push_back(m);
	Type const_class = Type::clazz(m->name);
	const_class.constant = true;
	add_internal_var(m->name, const_class, m->clazz);
}

VM::Result VM::execute(const std::string code, std::string ctx, std::string file_name, bool debug, bool ops, bool assembly, bool pseudo_code, bool log_instructions) {

	// Reset
	this->file_name = file_name;
	VM::current_vm = this;
	LSNull::set_null_value(this->null_value);
	LSBoolean::set_true_value(this->true_value);
	LSBoolean::set_false_value(this->false_value);
	LSValue::obj_count = 0;
	LSValue::obj_deleted = 0;
	VM::mpz_created = 0;
	VM::mpz_deleted = 0;
	VM::operations = 0;
	VM::enable_operations = ops;
	Type::placeholder_counter = 0;
	#if DEBUG_LEAKS
		LSValue::objs().clear();
	#endif

	auto program = new Program(code, file_name);

	// Compile
	auto compilation_start = std::chrono::high_resolution_clock::now();
	VM::Result result = program->compile(*this, ctx, assembly, pseudo_code, log_instructions);
	auto compilation_end = std::chrono::high_resolution_clock::now();

	if (log_instructions) {
		std::cout << result.instructions_log;
	}

	if (debug) {
		std::cout << "main() " << result.program << std::endl; // LCOV_EXCL_LINE
	}
	if (pseudo_code) {
		if (debug) std::cout << std::endl;
		std::cout << result.pseudo_code;
	}
	if (assembly) {
		if (debug) std::cout << std::endl;
		std::cout << result.assembly;
	}

	// Execute
	std::string value = "";
	if (result.compilation_success) {

		auto exe_start = std::chrono::high_resolution_clock::now();
		try {
			value = program->execute(*this);
			result.execution_success = true;
		} catch (vm::ExceptionObj& ex) {
			result.exception = ex;
		}
		auto exe_end = std::chrono::high_resolution_clock::now();

		result.execution_time = std::chrono::duration_cast<std::chrono::nanoseconds>(exe_end - exe_start).count();
		result.execution_time_ms = (((double) result.execution_time / 1000) / 1000);
		result.value = value;
		result.type = program->main->body->type;
	}

	// Set results
	result.context = ctx;
	result.compilation_time = std::chrono::duration_cast<std::chrono::nanoseconds>(compilation_end - compilation_start).count();
	result.compilation_time_ms = (((double) result.compilation_time / 1000) / 1000);
	result.operations = VM::operations;

	// Cleaning
	delete program;
	VM::enable_operations = true;
	Type::clear_placeholder_types();

	// Results
	result.objects_created = LSValue::obj_count;
	result.objects_deleted = LSValue::obj_deleted;
	result.mpz_objects_created = VM::mpz_created;
	result.mpz_objects_deleted = VM::mpz_deleted;

	if (ls::LSValue::obj_deleted != ls::LSValue::obj_count) {
		// LCOV_EXCL_START
		std::cout << C_RED << "/!\\ " << LSValue::obj_deleted << " / " << LSValue::obj_count << " (" << (LSValue::obj_count - LSValue::obj_deleted) << " leaked)" << END_COLOR << std::endl;
		#if DEBUG_LEAKS
			for (auto o : LSValue::objs()) {
				std::cout << o.second << " (" << o.second->refs << " refs)" << std::endl;
			}
		#endif
		// LCOV_EXCL_STOP
	}
	if (VM::mpz_deleted != VM::mpz_created) {
		std::cout << C_RED << "/!\\ " << VM::mpz_deleted << " / " << VM::mpz_created << " (" << (VM::mpz_created - VM::mpz_deleted) << " mpz leaked)" << END_COLOR << std::endl; // LCOV_EXCL_LINE
	}
	return result;
}

void VM::execute_ir(const std::string filename) {

	VM::current_vm = this;
	LSNull::set_null_value(this->null_value);
	LSBoolean::set_true_value(this->true_value);
	LSBoolean::set_false_value(this->false_value);

	llvm::SMDiagnostic Err;
	auto Mod = llvm::parseIRFile(filename, Err, compiler.getContext());
	compiler.addModule(std::move(Mod));
	auto symbol = compiler.findSymbol("main");
	void* f = (void*) cantFail(symbol.getAddress());
	auto fun = (int (*)()) f;

	auto exe_start = std::chrono::high_resolution_clock::now();
	int res = fun();
	auto exe_end = std::chrono::high_resolution_clock::now();

	auto execution_time = std::chrono::duration_cast<std::chrono::nanoseconds>(exe_end - exe_start).count();
	auto execution_time_ms = (((double) execution_time / 1000) / 1000);

	std::cout << res << std::endl;
	std::cout << execution_time_ms << " ms" << std::endl;
}

void VM::add_internal_var(std::string name, Type type, LSValue* value, Callable* callable) {
	// std::cout << "add_interval_var "<< name << " " << type << " " << value << std::endl;
	if (auto f = dynamic_cast<LSFunction*>(value)) {
		if (callable == nullptr) {
			callable = new Callable(name);
			callable->add_version({ name, type, f->function });
		}
	}
	internal_vars.insert({ name, std::make_shared<SemanticVar>(name, VarScope::INTERNAL, type, 0, nullptr, nullptr, nullptr, value, callable) });
	system_vars.push_back(value);
}

void VM::add_internal_var(std::string name, Type type, Function* function) {
	// std::cout << "add_interval_var "<< name << " " << type << " " << value << std::endl;
	internal_vars.insert({ name, std::make_shared<SemanticVar>(name, VarScope::INTERNAL, type, 0, function, nullptr, function, nullptr) });
}

void* VM::resolve_symbol(std::string name) {
	std::cout << "VM::resolve_symbol " << name << std::endl;
	auto p = name.find(".");
	if (p != std::string::npos) {
		auto module = name.substr(0, p);
		// std::cout << "module = " << module << std::endl;
		auto method = name.substr(p + 1);
		auto h = method.find(".");
		int version = 0;
		if (h != std::string::npos) {
			version = std::stoi(method.substr(h + 1));
			method = method.substr(0, h);
		}
		// std::cout << "method = " << method << std::endl;
		// std::cout << "version = " << version << std::endl;
		auto clazz = (LSClass*) internal_vars.at(module)->lsvalue;
		auto implems = clazz->methods.at(method);
		return implems.at(version).addr;
	}
	return nullptr;
}

}
