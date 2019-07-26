#include <chrono>
#include "Program.hpp"
#include "Context.hpp"
#include "../vm/value/LSNull.hpp"
#include "../vm/value/LSArray.hpp"
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
#include "../environment/Environment.hpp"
#include "../vm/VM.hpp"

namespace ls {

Program::Program(Environment& env, const std::string& code, const std::string& file_name)
	: env(env), code(code), file_name(file_name) {}

Program::~Program() {
	#if COMPILER
	if (handle_created) {
		compiler->removeModule(module_handle);
	}
	#endif
}

void Program::analyze(SyntaxicAnalyzer& syn, SemanticAnalyzer& sem, bool format, bool debug) {

	auto parse_start = std::chrono::high_resolution_clock::now();

	main_file = new File(file_name, code, new FileContext());
	auto block = syn.analyze(main_file);

	if (main_file->errors.size() > 0) {
		result.errors = main_file->errors;
		return;
	}

	auto token = new Token(TokenType::FUNCTION, main_file, 0, 0, 0, "function");
	main = std::make_unique<Function>(env, std::move(token));
	main->body = block;
	main->is_main_function = true;
	main->name = "main";

	sem.analyze(this, nullptr);

	auto parse_end = std::chrono::high_resolution_clock::now();
	auto parse_time = std::chrono::duration_cast<std::chrono::nanoseconds>(parse_end - parse_start).count();
	result.parse_time = (((double) parse_time / 1000) / 1000);

	type = main->type->return_type();

	if (sem.errors.size()) {
		result.compilation_success = false;
		result.errors = sem.errors;
		return;
	}

	if (format or debug) {
		std::cout << "main() ";
		print(std::cout, debug);
		std::cout << std::endl;
	}
	result.analyzed = true;
}

#if COMPILER
void Program::compile_leekscript(Compiler& c, bool format, bool debug, bool bitcode, bool pseudo_code, bool optimized_ir) {

	if (not result.analyzed) {
		return;
	}

	compiler = &c;

	c.vm->internals.clear();
	c.program = this;
	c.init();

	module = new llvm::Module(file_name, c.getContext());
	module->setDataLayout(c.DL);

	main->compile(c);

	if (pseudo_code) {
		std::error_code EC2;
		llvm::raw_fd_ostream ir(file_name + ".ll", EC2, llvm::sys::fs::F_None);
		module->print(ir, nullptr);
		ir.flush();
	}

	auto compilation_start = std::chrono::high_resolution_clock::now();

	module_handle = c.addModule(std::unique_ptr<llvm::Module>(module), true, bitcode, optimized_ir);
	handle_created = true;
	auto ExprSymbol = c.findSymbol("main");
	assert(ExprSymbol && "Function not found");
	closure = (void*) cantFail(ExprSymbol.getAddress());
	// std::cout << "program type " << main->type->return_type() << std::endl;
	type = main->type->return_type()->fold();
	// std::cout << "program type " << type << std::endl;

	auto compilation_end = std::chrono::high_resolution_clock::now();
	auto compilation_time = std::chrono::duration_cast<std::chrono::nanoseconds>(compilation_end - compilation_start).count();
	result.compilation_time = (((double) compilation_time / 1000) / 1000);

	result.compilation_success = true;
}

void Program::compile_ir_file(Compiler& c) {
	llvm::SMDiagnostic Err;
	auto Mod = llvm::parseIRFile(file_name, Err, c.getContext());
	if (!Mod) {
		Err.print("main", llvm::errs());
		result.compilation_success = false;
		result.program = "<error>";
		return;
	}
	auto llvm_type = Mod->getFunction("main")->getReturnType();
	c.addModule(std::move(Mod), true);
	auto symbol = c.findSymbol("main");
	closure = (void*) cantFail(symbol.getAddress());

	if (llvm_type->isPointerTy() and llvm_type->getPointerElementType()->isFunctionTy()) {
		type = Type::fun(env.void_, {});
	} else if (llvm_type->isPointerTy()) {
		type = env.any;
	} else if (llvm_type->isStructTy()) {
		type = env.mpz;
	} else if (llvm_type->isFloatingPointTy()) {
		type = env.real;
	} else {
		type = env.integer;
	}

	result.compilation_success = true;
	std::ostringstream oss;
	oss << llvm_type;
	result.program = type->to_string() + " " + oss.str();
}

void Program::compile_bitcode_file(Compiler& c) {
	auto EMod = llvm::parseBitcodeFile(llvm::MemoryBufferRef { *llvm::MemoryBuffer::getFile(file_name).get() }, c.getContext());
	if (!EMod) {
		llvm::errs() << EMod.takeError() << '\n';
		result.compilation_success = false;
		result.program = "<error>";
		return;
	}
	auto Mod = std::move(EMod.get());
	auto llvm_type = Mod->getFunction("main")->getReturnType();
	c.addModule(std::move(Mod), false); // Already optimized
	auto symbol = c.findSymbol("main");
	closure = (void*) cantFail(symbol.getAddress());

	type = llvm_type->isPointerTy() ? env.any : (llvm_type->isStructTy() ? env.mpz : env.integer);

	result.compilation_success = true;
	std::ostringstream oss;
	oss << llvm_type;
	result.program = type->to_string() + " " + oss.str();
}

void Program::compile(Compiler& c, bool format, bool debug, bool export_bitcode, bool pseudo_code, bool optimized_ir, bool ir, bool bitcode) {
	if (ir) {
		compile_ir_file(c);
	} else if (bitcode) {
		compile_bitcode_file(c);
	} else {
		compile_leekscript(c, format, debug, export_bitcode, pseudo_code, optimized_ir);
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
	auto f = new Function(env, token);
	f->addArgument(new Token(TokenType::IDENT, main_file, 0, 1, 0, "x"), nullptr);
	f->addArgument(new Token(TokenType::IDENT, main_file,2, 1, 2, "y"), nullptr);
	f->body = new Block(env, true);
	auto ex = std::make_unique<Expression>(env);
	ex->v1 = std::make_unique<VariableValue>(env, new Token(TokenType::IDENT, main_file, 0, 1, 0, "x"));
	ex->v2 = std::make_unique<VariableValue>(env, new Token(TokenType::IDENT, main_file, 2, 1, 2, "y"));
	ex->op = std::make_shared<Operator>(new Token(token_types.at(std::distance(ops.begin(), o)), main_file, 1, 1, 1, name));
	f->body->instructions.emplace_back(new ExpressionInstruction(env, std::move(ex)));
	auto type = Type::fun(env.any, { env.any, env.any });

	auto var = new Variable(name, VarScope::INTERNAL, type, 0, f, nullptr, nullptr, nullptr);
	operators.insert({name, var});
	return var;
}

#if COMPILER
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
#endif

void Program::print(std::ostream& os, bool debug) const {
	if (main) {
		main->default_version->body->print(os, 0, { debug, false, false });
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
