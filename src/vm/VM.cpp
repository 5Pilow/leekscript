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

namespace ls {

const unsigned long int VM::DEFAULT_OPERATION_LIMIT = 20000000;
VM* VM::current_vm = nullptr;
OutputStream* VM::default_output = new OutputStream();

VM::VM(bool legacy) : compiler(this), legacy(legacy) {

	operation_limit = VM::DEFAULT_OPERATION_LIMIT;
	stdLib = new StandardLibrary(legacy);
}

VM::~VM() {}

VM* VM::current() {
	return current_vm;
}

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
VM::Result VM::execute(const std::string code, Context* ctx, std::string file_name, bool format, bool debug, bool ops, bool assembly, bool pseudo_code, bool optimized_ir, bool execute_ir, bool execute_bitcode) {

	// Reset
	this->file_name = file_name;
	VM::current_vm = this;
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
	this->context = ctx;

	auto program = new Program(code, file_name);

	// Compile
	auto result = program->compile(*this, stdLib, ctx, format, debug, assembly, pseudo_code, optimized_ir, execute_ir, execute_bitcode);

	if (format or debug) {
		std::cout << "main() ";
		std::cout << result.program << std::endl; // LCOV_EXCL_LINE
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
	if (result.compilation_success) {
		std::string value = "";
		auto exe_start = std::chrono::high_resolution_clock::now();
		try {
			value = program->execute(*this);
			result.execution_success = true;
		} catch (vm::ExceptionObj& ex) {
			result.exception = ex;
		}
		auto exe_end = std::chrono::high_resolution_clock::now();

		auto execution_time = std::chrono::duration_cast<std::chrono::nanoseconds>(exe_end - exe_start).count();
		result.execution_time = (((double) execution_time / 1000) / 1000);
		result.value = value;
		result.type = program->type;
	}

	// Set results
	result.operations = VM::operations;

	// Cleaning
	delete program;
	for (const auto& f : function_created) {
		delete f;
	}
	function_created.clear();
	for (const auto& c : class_created) {
		delete c;
	}
	class_created.clear();
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
	return result;
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
		} else if (stdLib->classes.find(module) != stdLib->classes.end()) {
			const auto& clazz = stdLib->classes.at(module)->clazz;
			if (method.substr(0, 8) == "operator") {
				const auto& op = method.substr(8);
				const auto& implems = clazz->operators.at(op);
				return implems.at(version).addr;
			} else {
				if (clazz->methods.find(method) != clazz->methods.end()) {
					const auto& implems = clazz->methods.at(method);
					return implems.versions.at(version)->addr;
				} else if (clazz->static_fields.find(method) != clazz->static_fields.end()) {
					if (clazz->static_fields.at(method).native_fun) {
						return clazz->static_fields.at(method).native_fun;
					} else {
						return clazz->static_fields.at(method).addr;
					}
				} else if (clazz->fields.find(method) != clazz->fields.end()) {
					return clazz->fields.at(method).native_fun;
				}
			}
		}
	} else {
		if (stdLib->classes.find(name) != stdLib->classes.end()) {
			return stdLib->classes.at(name)->lsclass.get();
		}
	}
	return nullptr;
}
#endif

}
