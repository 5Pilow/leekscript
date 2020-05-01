#include <sstream>
#include <chrono>
#include "VM.hpp"
#include "../constants.h"
#include "../colors.h"
#include "../analyzer/lexical/LexicalAnalyzer.hpp"
#include "../analyzer/syntaxic/SyntaxicAnalyzer.hpp"
#include "../analyzer/Context.hpp"
#include "../analyzer/semantic/SemanticAnalyzer.hpp"
#include "../analyzer/error/Error.hpp"
#include "value/LSNumber.hpp"
#include "value/LSArray.hpp"
#include "../analyzer/Program.hpp"
#include "value/LSObject.hpp"
#include "value/LSFunction.hpp"
#include "../standard/class/ValueSTD.hpp"
#include "../standard/class/NullSTD.hpp"
#include "../standard/class/NumberSTD.hpp"
#include "../standard/class/BooleanSTD.hpp"
#include "../standard/class/StringSTD.hpp"
#include "../standard/class/ArraySTD.hpp"
#include "../standard/class/MapSTD.hpp"
#include "../standard/class/SetSTD.hpp"
#include "../standard/class/ObjectSTD.hpp"
#include "../standard/class/SystemSTD.hpp"
#include "../standard/class/FunctionSTD.hpp"
#include "../standard/class/ClassSTD.hpp"
#include "../standard/class/IntervalSTD.hpp"
#include "../standard/class/JsonSTD.hpp"
#include "../analyzer/semantic/Callable.hpp"
#include "../analyzer/semantic/CallableVersion.hpp"
#include "../analyzer/semantic/Variable.hpp"
#include "../environment/Environment.hpp"

namespace ls {

const unsigned long int VM::DEFAULT_OPERATION_LIMIT = 20000000;
OutputStream* VM::default_output = new OutputStream();

VM::VM(Environment& env, StandardLibrary& std) : env(env), std(std) {
	operation_limit = VM::DEFAULT_OPERATION_LIMIT;
}

VM::~VM() {}

void VM::static_init() {
	// Global initialization
	llvm::InitializeNativeTarget();
	llvm::InitializeNativeTargetAsmPrinter();
	llvm::InitializeNativeTargetAsmParser();

	LSNull::set_null_value(LSNull::create());
	LSBoolean::set_true_value(LSBoolean::create(true));
	LSBoolean::set_false_value(LSBoolean::create(false));
}

#if COMPILER
void VM::execute(Program& program, bool format, bool debug, bool ops, bool assembly, bool pseudo_code, bool optimized_ir, bool execute_ir, bool execute_bitcode) {

	// Reset
	this->file_name = file_name;
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
	this->context = program.context;

	if (pseudo_code) {
		if (debug) std::cout << std::endl;
		std::cout << program.result.pseudo_code;
	}
	if (assembly) {
		if (debug) std::cout << std::endl;
		std::cout << program.result.assembly;
	}

	// Execute
	if (program.result.compilation_success) {
		std::string value = "";
		auto exe_start = std::chrono::high_resolution_clock::now();
		try {
			value = program.execute(*this);
			program.result.execution_success = true;
		} catch (vm::ExceptionObj& ex) {
			program.result.exception = ex;
		}
		auto exe_end = std::chrono::high_resolution_clock::now();

		auto execution_time = std::chrono::duration_cast<std::chrono::nanoseconds>(exe_end - exe_start).count();
		program.result.execution_time = (((double) execution_time / 1000) / 1000);
		program.result.value = value;
		program.result.type = program.type;
	}

	// Set results
	program.result.operations = VM::operations;

	// Cleaning
	for (const auto& f : function_created) {
		delete f;
	}
	function_created.clear();
	for (const auto& c : class_created) {
		delete c;
	}
	class_created.clear();
	VM::enable_operations = true;
	env.clear_placeholder_types();

	// Results
	program.result.objects_created = LSValue::obj_count;
	program.result.objects_deleted = LSValue::obj_deleted;
	program.result.mpz_objects_created = VM::mpz_created;
	program.result.mpz_objects_deleted = VM::mpz_deleted;

	if (ls::LSValue::obj_deleted != ls::LSValue::obj_count) {
		// LCOV_EXCL_START
		#if DEBUG_LEAKS
			std::cout << C_RED << "/!\\ " << LSValue::obj_deleted << " / " << LSValue::obj_count << " (" << (LSValue::obj_count - LSValue::obj_deleted) << " leaked)" << END_COLOR << std::endl;
			int n = 20;
			for (auto o : LSValue::objs()) {
				std::cout << o.second << " (" << o.second->refs << " refs) " << (void*) o.second << std::endl;
				if (n-- < 0) {
					std::cout << "[...] and more" << std::endl;
					break;
				}
			}
		#endif
		// LCOV_EXCL_STOP
	}
	if (VM::mpz_deleted != VM::mpz_created) {
		std::cout << C_RED << "/!\\ " << VM::mpz_deleted << " / " << VM::mpz_created << " (" << (VM::mpz_created - VM::mpz_deleted) << " mpz leaked)" << END_COLOR << std::endl; // LCOV_EXCL_LINE
	}
}
#endif

#if COMPILER
void* VM::resolve_symbol(std::string name) {
	// std::cout << "VM::resolve_symbol " << name << std::endl;
	const auto& p = name.find(".");
	if (p != std::string::npos) {
		const auto& module = name.substr(0, p);
		// std::cout << "module = " << module << std::endl;
		auto method = name.substr(p + 1);
		const auto& h = method.find(".");
		int version = 0;
		if (h != std::string::npos) {
			version = std::stoi(method.substr(h + 1));
			method = method.substr(0, h);
		}
		// std::cout << "method = " << method << std::endl;
		// std::cout << "version = " << version << std::endl;
		if (module == "ctx") {
			return &context->vars.at(method).value;
		} else if (std.classes.find(module) != std.classes.end()) {
			const auto& clazz = std.classes.at(module)->clazz;
			if (method.substr(0, 8) == "operator") {
				const auto& op = method.substr(8);
				if (clazz->operators.find(op) != clazz->operators.end()) {
					const auto& implems = clazz->operators.at(op);
					return implems.versions.at(version).addr;
				}
				std::cout << C_YELLOW << "Operator '" << name << "' not found!" << END_COLOR << std::endl;
			} else {
				if (clazz->methods.find(method) != clazz->methods.end()) {
					const auto& implems = clazz->methods.at(method);
					return implems.versions.at(version).addr;
				} else if (clazz->static_fields.find(method) != clazz->static_fields.end()) {
					if (clazz->static_fields.at(method).native_fun) {
						return clazz->static_fields.at(method).native_fun;
					} else {
						return clazz->static_fields.at(method).addr;
					}
				} else if (clazz->fields.find(method) != clazz->fields.end()) {
					return clazz->fields.at(method).native_fun;
				}
				std::cout << C_YELLOW << "Method '" << name << "' not found!" << END_COLOR << std::endl;
			}
		}
	} else {
		if (std.classes.find(name) != std.classes.end()) {
			return std.classes.at(name)->lsclass;
		}
	}
	return nullptr;
}
#endif

void VM::add_operations(int amount) {
	if (not enable_operations) return;
	operations += amount;
	if (operations > operation_limit) {
		throw new vm::Exception(vm::Exception::OPERATION_LIMIT_EXCEEDED);
	}
}

}
