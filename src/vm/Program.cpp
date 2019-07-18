#include <chrono>
#include "Program.hpp"
#include "Context.hpp"
#include "value/LSNull.hpp"
#include "value/LSArray.hpp"
#include "../analyzer/lexical/LexicalAnalyzer.hpp"
#include "../analyzer/syntaxic/SyntaxicAnalyzer.hpp"
#include "Context.hpp"
#include "../analyzer/semantic/SemanticAnalyzer.hpp"
#include "../analyzer/error/Error.hpp"
#include "../util/Util.hpp"
#include "../constants.h"
#include "../colors.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Function.h"
#include "llvm/IRReader/IRReader.h"
#include "llvm/Support/SourceMgr.h"
#include "llvm/Support/raw_ostream.h"
#include "../analyzer/value/Expression.hpp"
#include "../analyzer/instruction/ExpressionInstruction.hpp"
#include "../analyzer/value/VariableValue.hpp"
#include "llvm/Bitcode/BitcodeWriter.h"
#include "llvm/Bitcode/BitcodeReader.h"
#include "../analyzer/semantic/Variable.hpp"
#include "../analyzer/semantic/FunctionVersion.hpp"

namespace ls {

Program::Program(const std::string& code, const std::string& file_name) {
	this->code = code;
	this->file_name = file_name;
	closure = nullptr;
}

Program::~Program() {
	if (handle_created) {
		vm->compiler.removeModule(module_handle);
	}
}

#if COMPILER
VM::Result Program::compile_leekscript(VM& vm, Context* ctx, bool bitcode, bool pseudo_code, bool optimized_ir) {

	auto parse_start = std::chrono::high_resolution_clock::now();

	auto resolver = new Resolver();
	VM::Result result;
	main_file = new File(file_name, code, new FileContext());
	SyntaxicAnalyzer syn { resolver };
	auto block = syn.analyze(main_file);

	if (main_file->errors.size() > 0) {
		result.compilation_success = false;
		result.errors = main_file->errors;
		return result;
	}

	auto token = new Token(TokenType::FUNCTION, main_file, 0, 0, 0, "function");
	this->main = std::make_unique<Function>(std::move(token));
	this->main->body = block;
	this->main->is_main_function = true;

	// Semantical analysis
	SemanticAnalyzer sem;
	sem.vm = &vm;
	sem.analyze(this, ctx);

	std::ostringstream oss;
	print(oss, true);
	result.program = oss.str();

	if (sem.errors.size()) {
		result.compilation_success = false;
		result.errors = sem.errors;
		return result;
	}

	// print(std::cout, true); std::cout << std::endl;

	// Compilation
	vm.internals.clear();
	vm.compiler.program = this;
	vm.compiler.init();

	module = new llvm::Module(file_name, vm.compiler.getContext());
	module->setDataLayout(vm.compiler.DL);

	main->compile(vm.compiler);

	auto parse_end = std::chrono::high_resolution_clock::now();
	auto parse_time = std::chrono::duration_cast<std::chrono::nanoseconds>(parse_end - parse_start).count();
	result.parse_time = (((double) parse_time / 1000) / 1000);

	if (pseudo_code) {
		std::error_code EC2;
		llvm::raw_fd_ostream ir(file_name + ".ll", EC2, llvm::sys::fs::F_None);
		module->print(ir, nullptr);
		ir.flush();
	}

	auto compilation_start = std::chrono::high_resolution_clock::now();

	module_handle = vm.compiler.addModule(std::unique_ptr<llvm::Module>(module), true, bitcode, optimized_ir);
	handle_created = true;
	auto ExprSymbol = vm.compiler.findSymbol("main");
	assert(ExprSymbol && "Function not found");
	closure = (void*) cantFail(ExprSymbol.getAddress());
	// std::cout << "program type " << main->type->return_type() << std::endl;
	type = main->type->return_type()->fold();
	// std::cout << "program type " << type << std::endl;

	auto compilation_end = std::chrono::high_resolution_clock::now();
	auto compilation_time = std::chrono::duration_cast<std::chrono::nanoseconds>(compilation_end - compilation_start).count();
	result.compilation_time = (((double) compilation_time / 1000) / 1000);

	result.compilation_success = true;

	return result;
}

VM::Result Program::compile_ir_file(VM& vm) {
	VM::Result result;
	llvm::SMDiagnostic Err;
	auto Mod = llvm::parseIRFile(file_name, Err, vm.compiler.getContext());
	if (!Mod) {
		Err.print("main", llvm::errs());
		result.compilation_success = false;
		result.program = "<error>";
		return result;
	}
	auto llvm_type = Mod->getFunction("main")->getReturnType();
	vm.compiler.addModule(std::move(Mod), true);
	auto symbol = vm.compiler.findSymbol("main");
	closure = (void*) cantFail(symbol.getAddress());

	if (llvm_type->isPointerTy() and llvm_type->getPointerElementType()->isFunctionTy()) {
		type = Type::fun();
	} else if (llvm_type->isPointerTy()) {
		type = Type::any;
	} else if (llvm_type->isStructTy()) {
		type = Type::mpz;
	} else if (llvm_type->isFloatingPointTy()) {
		type = Type::real;
	} else {
		type = Type::integer;
	}

	result.compilation_success = true;
	std::ostringstream oss;
	oss << llvm_type;
	result.program = type->to_string() + " " + oss.str();
	return result;
}

VM::Result Program::compile_bitcode_file(VM& vm) {
	VM::Result result;
	auto EMod = llvm::parseBitcodeFile(llvm::MemoryBufferRef { *llvm::MemoryBuffer::getFile(file_name).get() }, vm.compiler.getContext());
	if (!EMod) {
		llvm::errs() << EMod.takeError() << '\n';
		result.compilation_success = false;
		result.program = "<error>";
		return result;
	}
	auto Mod = std::move(EMod.get());
	auto llvm_type = Mod->getFunction("main")->getReturnType();
	vm.compiler.addModule(std::move(Mod), false); // Already optimized
	auto symbol = vm.compiler.findSymbol("main");
	closure = (void*) cantFail(symbol.getAddress());

	type = llvm_type->isPointerTy() ? Type::any : (llvm_type->isStructTy() ? Type::mpz : Type::integer);

	result.compilation_success = true;
	std::ostringstream oss;
	oss << llvm_type;
	result.program = type->to_string() + " " + oss.str();
	return result;
}

VM::Result Program::compile(VM& vm, Context* ctx, bool export_bitcode, bool pseudo_code, bool optimized_ir, bool ir, bool bitcode) {
	this->vm = &vm;

	if (ir) {
		return compile_ir_file(vm);
	} else if (bitcode) {
		return compile_bitcode_file(vm);
	} else {
		return compile_leekscript(vm, ctx, export_bitcode, pseudo_code, optimized_ir);
	}
}
#endif

Variable* Program::get_operator(const std::string& name) {
	// std::cout << "Program::get_operator(" << name << ")" << std::endl;

	auto op = operators.find(name);
	if (op != operators.end()) {
		return op->second;
	}

	std::vector<std::string> ops = {"+", "-", "*", "×", "/", "÷", "**", "%", "\\", "~", ">", "<", ">=", "<="};
	std::vector<TokenType> token_types = {TokenType::PLUS, TokenType::MINUS, TokenType::TIMES, TokenType::TIMES, TokenType::DIVIDE, TokenType::DIVIDE, TokenType::POWER, TokenType::MODULO, TokenType::INT_DIV, TokenType::TILDE, TokenType::GREATER, TokenType::LOWER, TokenType::GREATER_EQUALS, TokenType::LOWER_EQUALS};
	
	auto o = std::find(ops.begin(), ops.end(), name);
	if (o == ops.end()) return nullptr;

	auto token = new Token(TokenType::FUNCTION, main_file, 0, 0, 0, "function");
	auto f = new Function(token);
	f->addArgument(new Token(TokenType::IDENT, main_file, 0, 1, 0, "x"), nullptr);
	f->addArgument(new Token(TokenType::IDENT, main_file,2, 1, 2, "y"), nullptr);
	f->body = new Block(true);
	auto ex = std::make_unique<Expression>();
	ex->v1 = std::make_unique<VariableValue>(new Token(TokenType::IDENT, main_file, 0, 1, 0, "x"));
	ex->v2 = std::make_unique<VariableValue>(new Token(TokenType::IDENT, main_file, 2, 1, 2, "y"));
	ex->op = std::make_shared<Operator>(new Token(token_types.at(std::distance(ops.begin(), o)), main_file, 1, 1, 1, name));
	f->body->instructions.emplace_back(new ExpressionInstruction(std::move(ex)));
	auto type = Type::fun(Type::any, {Type::any, Type::any});

	auto var = new Variable(name, VarScope::INTERNAL, type, 0, f, nullptr, nullptr, nullptr);
	operators.insert({name, var});
	return var;
}

void Program::analyze(SemanticAnalyzer* analyzer) {
	main->name = "main";
	main->analyze(analyzer);
}

std::string Program::execute(VM& vm) {

	assert(!type->reference && "Program return type shouldn't be a reference!");

	if (type->is_void()) {
		auto fun = (void (*)()) closure;
		fun();
		return "(void)";
	}
	if (type->is_bool()) {
		auto fun = (bool (*)()) closure;
		bool res = fun();
		return res ? "true" : "false";
	}
	if (type->is_integer()) {
		auto fun = (int (*)()) closure;
		int res = fun();
		return std::to_string(res);
	}
	if (type->is_mpz()) {
		auto fun = (__mpz_struct (*)()) closure;
		auto ret = fun();
		char buff[1000000];
		mpz_get_str(buff, 10, &ret);
		mpz_clear(&ret);
		vm.mpz_deleted++;
		return std::string(buff);
	}
	if (type->is_real()) {
		auto fun = (double (*)()) closure;
		double res = fun();
		return LSNumber::print(res);
	}
	if (type->is_long()) {
		auto fun = (long (*)()) closure;
		long res = fun();
		return std::to_string(res);
	}
	if (type->is_function_pointer()) {
		auto fun = (long (*)()) closure;
		fun();
		return "<function>";
	}
	auto fun = (LSValue* (*)()) closure;
	auto ptr = fun();
	std::ostringstream oss;
	ptr->dump(oss, 5);
	LSValue::delete_ref(ptr);
	return oss.str();
}

void Program::print(std::ostream& os, bool debug) const {
	if (main) {
		main->default_version->body->print(os, 0, debug, false);
	} else {
		os << "(ll file)";
	}
}

std::ostream& operator << (std::ostream& os, const Program* program) {
	program->print(os, false);
	return os;
}

std::string Program::underline_code(Location location, Location focus) const {
	// std::cout << "underline " << location.start.column << " " << location.end.column << " " << focus.start.column << " " << focus.end.column << std::endl;
	auto padding = 10ul;
	auto start = padding > location.start.raw ? 0ul : location.start.raw - padding;
	auto end = std::min(code.size(), location.end.raw + padding);
	auto padding_left = std::min(padding, location.start.raw - start);
	auto padding_right = std::min(padding, end - location.end.raw);
	auto ellipsis_left = start > 0;
	auto ellipsis_right = end < code.size();

	auto extract = code.substr(start, end - start);
	auto underlined = extract.substr(padding_left, end - start - padding_left - padding_right);
	auto before = extract.substr(0, padding_left);
	auto after = extract.substr(extract.size() - padding_right, padding_right);

	size_t p = before.rfind('\n');
	if (p != std::string::npos) {
		before = before.substr(p + 1, before.size() - p);
		ellipsis_left = false;
	}
	p = after.find('\n');
	if (p != std::string::npos) {
		after = after.substr(0, p);
		ellipsis_right = false;
	}

	auto focus_start = focus.start.raw - location.start.raw;
	auto focus_size = focus.end.raw - focus.start.raw;
	underlined = underlined.substr(0, focus_start)
		+ C_RED + underlined.substr(focus_start, focus_size) + END_COLOR
		+ UNDERLINE + underlined.substr(focus_size + focus_start);

	if (before.size() and before.front() != ' ')
		before = ' ' + before;
	if (after.size() and after.back() != ' ')
		after = after + ' ';

	return (ellipsis_left ? (C_GREY "[...]" END_COLOR) : "")
		+ before + UNDERLINE + underlined + END_STYLE + after
		+ (ellipsis_right ? (C_GREY "[...]" END_COLOR) : "");
}

}
