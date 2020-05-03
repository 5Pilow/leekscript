#include "SystemSTD.hpp"
#include <chrono>
#include "../../constants.h"
#include "../../type/Type.hpp"
#include "../../environment/Environment.hpp"
#if COMPILER
#include "../../vm/LSValue.hpp"
#include "../../vm/VM.hpp"
#endif

namespace ls {

SystemSTD::SystemSTD(Environment& env) : Module(env, "System") {

	static_field("version", env.integer, ADDR(version));
	static_field("operations", env.integer, ADDR([&](ls::Compiler& c) {
		return c.insn_load(c.get_symbol("operations", env.integer->pointer()));
	}));
	static_field_fun("time", env.long_, ADDR((void*) time));
	static_field_fun("milliTime", env.long_, ADDR((void*) millitime));
	static_field_fun("microTime", env.long_, ADDR((void*) microtime));
	static_field_fun("nanoTime", env.long_, ADDR((void*) nanotime));

	method("print", {
		{env.void_, {env.const_any}, ADDR(print)},
		{env.void_, {env.mpz_ptr}, ADDR(print_mpz)},
		{env.void_, {env.tmp_mpz_ptr}, ADDR(print_mpz_tmp)},
		{env.void_, {env.const_long}, ADDR(print_long)},
		{env.void_, {env.const_real}, ADDR(print_real)},
		{env.void_, {env.const_integer}, ADDR(print_int)},
		{env.void_, {env.const_boolean}, ADDR(print_bool)},
	});

	method("throw", {
		{env.void_, {env.integer, env.i8_ptr, env.i8_ptr, env.long_}, ADDR((void*) throw1)},
		{env.void_, {env.long_, env.long_, env.i8_ptr, env.i8_ptr}, ADDR((void*) throw2)},
	}, PRIVATE | LEGACY);

	method("debug", {
		{env.void_, {env.any}, ADDR(print)}
	}, LEGACY_ONLY);

	method("internal_print", {
		{env.void_, {env.i8->pointer(), env.const_boolean}, ADDR((void*) internal_print_bool)},
		{env.void_, {env.i8->pointer(), env.const_integer}, ADDR((void*) internal_print_int)},
		{env.void_, {env.i8->pointer(), env.const_long}, ADDR((void*) internal_print_long)},
		{env.void_, {env.i8->pointer(), env.const_real}, ADDR((void*) internal_print_real)},
		{env.void_, {env.i8->pointer(), env.mpz_ptr}, ADDR((void*) internal_print_mpz)},
		{env.void_, {env.i8->pointer(), env.tmp_mpz_ptr}, ADDR((void*) internal_print_mpz_tmp)},
		{env.void_, {env.i8->pointer(), env.const_any}, ADDR((void*) internal_print)},
	}, PRIVATE | LEGACY);

	/**
	 * Legacy-only
	 */
	static_field("TYPE_NULL", env.integer, ADDR([&](ls::Compiler& c) { return c.new_integer(0); }), LEGACY_ONLY);
	static_field("TYPE_NUMBER", env.integer, ADDR([&](ls::Compiler& c) { return c.new_integer(1); }), LEGACY_ONLY);
	static_field("TYPE_BOOLEAN", env.integer, ADDR([&](ls::Compiler& c) { return c.new_integer(2); }), LEGACY_ONLY);
	static_field("TYPE_STRING", env.integer, ADDR([&](ls::Compiler& c) { return c.new_integer(3); }), LEGACY_ONLY);
	static_field("TYPE_ARRAY", env.integer, ADDR([&](ls::Compiler& c) { return c.new_integer(4); }), LEGACY_ONLY);
	static_field("TYPE_FUNCTION", env.integer, ADDR([&](ls::Compiler& c) { return c.new_integer(5); }), LEGACY_ONLY);

	method("getOperations", {
		{env.integer, {}, ADDR(nullptr)}
	}, LEGACY_ONLY);

	method("getInstructionCount", {
		{env.integer, {}, ADDR(nullptr)}
	}, LEGACY_ONLY);
}

#if COMPILER

long SystemSTD::time() {
	return std::chrono::duration_cast<std::chrono::seconds>(
		std::chrono::system_clock::now().time_since_epoch()
	).count();
}

long SystemSTD::millitime() {
	return std::chrono::duration_cast<std::chrono::milliseconds>(
		std::chrono::system_clock::now().time_since_epoch()
	).count();
}

long SystemSTD::microtime() {
	return std::chrono::duration_cast<std::chrono::microseconds>(
		std::chrono::system_clock::now().time_since_epoch()
	).count();
}

long SystemSTD::nanotime() {
	return std::chrono::duration_cast<std::chrono::nanoseconds>(
		std::chrono::system_clock::now().time_since_epoch()
	).count();
}

Compiler::value SystemSTD::version(Compiler& c) {
	return c.new_integer(LEEKSCRIPT_VERSION);
}

Compiler::value SystemSTD::print(Compiler& c, std::vector<Compiler::value> args, int) {
	return c.insn_call(c.env.void_, {c.get_vm(), args[0]}, "System.internal_print.6");
}
Compiler::value SystemSTD::print_int(Compiler& c, std::vector<Compiler::value> args, int) {
	return c.insn_call(c.env.void_, {c.get_vm(), args[0]}, "System.internal_print.1");
}
Compiler::value SystemSTD::print_mpz(Compiler& c, std::vector<Compiler::value> args, int) {
	return c.insn_call(c.env.void_, {c.get_vm(), args[0]}, "System.internal_print.4");
}
Compiler::value SystemSTD::print_mpz_tmp(Compiler& c, std::vector<Compiler::value> args, int) {
	return c.insn_call(c.env.void_, {c.get_vm(), args[0]}, "System.internal_print.5");
}
Compiler::value SystemSTD::print_long(Compiler& c, std::vector<Compiler::value> args, int) {
	return c.insn_call(c.env.void_, {c.get_vm(), args[0]}, "System.internal_print.2");
}
Compiler::value SystemSTD::print_bool(Compiler& c, std::vector<Compiler::value> args, int) {
	return c.insn_call(c.env.void_, {c.get_vm(), args[0]}, "System.internal_print.0");
}
Compiler::value SystemSTD::print_real(Compiler& c, std::vector<Compiler::value> args, int) {
	return c.insn_call(c.env.void_, {c.get_vm(), args[0]}, "System.internal_print.3");
}

void SystemSTD::internal_print(VM* vm, LSValue* value) {
	value->print(vm->output->stream());
	vm->output->end();
	LSValue::delete_temporary(value);
}

void SystemSTD::internal_print_int(VM* vm, int v) {
	vm->output->stream() << v;
	vm->output->end();
}

void SystemSTD::internal_print_mpz(VM* vm, __mpz_struct* v) {
	char buff[1000];
	mpz_get_str(buff, 10, v);
	vm->output->stream() << buff;
	vm->output->end();
}
void SystemSTD::internal_print_mpz_tmp(VM* vm, __mpz_struct* v) {
	char buff[1000];
	mpz_get_str(buff, 10, v);
	vm->output->stream() << buff;
	vm->output->end();
	mpz_clear(v);
	vm->mpz_deleted++;
}

void SystemSTD::internal_print_long(VM* vm, long v) {
	vm->output->stream() << v;
	vm->output->end();
}

void SystemSTD::internal_print_bool(VM* vm, bool v) {
	vm->output->stream() << std::boolalpha << v;
	vm->output->end();
}

void SystemSTD::internal_print_real(VM* vm, double v) {
	vm->output->stream() << v;
	vm->output->end();
}

void SystemSTD::throw1(int type, char* file, char* function, size_t line) {
	// std::cout << "SystemSTD::throw " << type << " " << function << " " << line << std::endl;
	vm::ExceptionObj ex { (vm::Exception) type };
	ex.frames.push_back({file, function, line});
	throw ex;
}

/**
 * Doc
 *  - https://refspecs.linuxfoundation.org/abi-eh-1.22.html#base-throw
 *  - https://llvm.org/docs/ExceptionHandling.html
 *  - https://monoinfinito.wordpress.com/series/exception-handling-in-c/
 */
void SystemSTD::throw2(void** ex, char* file, char* function, size_t line) {
	auto exception = (vm::ExceptionObj*) (ex);
	exception->frames.push_back({file, function, line});
}

#endif

}
