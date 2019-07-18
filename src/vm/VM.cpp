#include <sstream>
#include <chrono>
#include "VM.hpp"
#include "../constants.h"
#include "../colors.h"
#include "../analyzer/lexical/LexicalAnalyzer.hpp"
#include "../analyzer/syntaxic/SyntaxicAnalyzer.hpp"
#include "Context.hpp"
#include "../analyzer/semantic/SemanticAnalyzer.hpp"
#include "../analyzer/error/Error.hpp"
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
#include "../analyzer/semantic/Callable.hpp"
#include "../analyzer/semantic/CallableVersion.hpp"
#include "../analyzer/semantic/Variable.hpp"

namespace ls {

const unsigned long int VM::DEFAULT_OPERATION_LIMIT = 20000000;
VM* VM::current_vm = nullptr;
OutputStream* VM::default_output = new OutputStream();

VM::VM(bool legacy) : compiler(this), legacy(legacy) {

	operation_limit = VM::DEFAULT_OPERATION_LIMIT;

	null_value = std::unique_ptr<LSNull>(LSNull::create());
	true_value = std::unique_ptr<LSBoolean>(LSBoolean::create(true));
	false_value = std::unique_ptr<LSBoolean>(LSBoolean::create(false));
	LSNull::set_null_value(null_value.get());
	LSBoolean::set_true_value(true_value.get());
	LSBoolean::set_false_value(false_value.get());

	// Include STD modules
	add_module(std::make_unique<ValueSTD>(this));
	add_module(std::make_unique<NullSTD>(this));
	add_module(std::make_unique<BooleanSTD>(this));
	add_module(std::make_unique<NumberSTD>(this));
	add_module(std::make_unique<StringSTD>(this));
	add_module(std::make_unique<ArraySTD>(this));
	add_module(std::make_unique<MapSTD>(this));
	add_module(std::make_unique<SetSTD>(this));
	add_module(std::make_unique<ObjectSTD>(this));
	add_module(std::make_unique<FunctionSTD>(this));
	add_module(std::make_unique<ClassSTD>(this));
	add_module(std::make_unique<SystemSTD>(this));
	add_module(std::make_unique<IntervalSTD>(this));
	add_module(std::make_unique<JsonSTD>(this));

	auto ptr_type = Type::fun(Type::tmp_any, {Type::any});
	add_internal_var("ptr", ptr_type, nullptr, nullptr, {
		new CallableVersion {"Value.ptr", ptr_type }
	});
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
}

void VM::add_module(std::unique_ptr<Module> m) {
	auto const_class = Type::const_class(m->name);
	add_internal_var(m->name, const_class, m->clazz.get(), m->lsclass.get());
	modules.push_back(std::move(m));
}

VM::Result VM::execute(const std::string code, Context* ctx, std::string file_name, bool debug, bool ops, bool assembly, bool pseudo_code, bool optimized_ir, bool execute_ir, bool execute_bitcode) {

	// Reset
	this->file_name = file_name;
	VM::current_vm = this;
	LSNull::set_null_value(null_value.get());
	LSBoolean::set_true_value(true_value.get());
	LSBoolean::set_false_value(false_value.get());
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
	auto result = program->compile(*this, ctx, assembly, pseudo_code, optimized_ir, execute_ir, execute_bitcode);

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
				std::cout << o.second << " (" << o.second->refs << " refs) " << (void*) o.second << std::endl;
			}
		#endif
		// LCOV_EXCL_STOP
	}
	if (VM::mpz_deleted != VM::mpz_created) {
		std::cout << C_RED << "/!\\ " << VM::mpz_deleted << " / " << VM::mpz_created << " (" << (VM::mpz_created - VM::mpz_deleted) << " mpz leaked)" << END_COLOR << std::endl; // LCOV_EXCL_LINE
	}
	return result;
}

void VM::add_internal_var(std::string name, const Type* type, Class* clazz, LSClass* lsclass, Call call) {
	// std::cout << "add_interval_var "<< name << " " << type << " " << value << std::endl;
	internal_vars.insert({ name, std::make_unique<Variable>(name, VarScope::INTERNAL, type, 0, nullptr, nullptr, nullptr, clazz, lsclass, call) });
}

void VM::add_internal_var(std::string name, const Type* type, Function* function) {
	// std::cout << "add_interval_var "<< name << " " << type << " " << value << std::endl;
	internal_vars.insert({ name, std::make_unique<Variable>(name, VarScope::INTERNAL, type, 0, function, nullptr, nullptr, nullptr) });
}

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
		} else if (internal_vars.find(module) != internal_vars.end()) {
			const auto& clazz = (Class*) internal_vars.at(module)->clazz;
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
		if (internal_vars.find(name) != internal_vars.end()) {
			return internal_vars.at(name)->lsclass;
		}
	}
	return nullptr;
}

}
