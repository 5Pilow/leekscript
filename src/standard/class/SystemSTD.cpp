#include "SystemSTD.hpp"
#include <chrono>
#include "../../vm/LSValue.hpp"
#include "../../constants.h"
#include "../../vm/VM.hpp"
#include "../../type/Type.hpp"

namespace ls {

SystemSTD::SystemSTD(StandardLibrary* stdLib) : Module(stdLib, "System") {

	static_field("version", Type::integer, ADDR(version));
	static_field("operations", Type::integer, ADDR([&](ls::Compiler& c) {
		return c.insn_load(c.get_symbol("operations", Type::integer->pointer()));
	}));
	static_field_fun("time", Type::long_, ADDR((void*) time));
	static_field_fun("milliTime", Type::long_, ADDR((void*) millitime));
	static_field_fun("microTime", Type::long_, ADDR((void*) microtime));
	static_field_fun("nanoTime", Type::long_, ADDR((void*) nanotime));

	method("print", {
		{Type::void_, {Type::const_any}, ADDR(print)},
		{Type::void_, {Type::mpz_ptr}, ADDR(print_mpz)},
		{Type::void_, {Type::tmp_mpz_ptr}, ADDR(print_mpz_tmp)},
		{Type::void_, {Type::const_long}, ADDR(print_long)},
		{Type::void_, {Type::const_real}, ADDR(print_real)},
		{Type::void_, {Type::const_integer}, ADDR(print_int)},
		{Type::void_, {Type::const_boolean}, ADDR(print_bool)},
	});

	method("throw", {
		{Type::void_, {Type::integer, Type::i8_ptr, Type::i8_ptr, Type::long_}, ADDR((void*) throw1)},
		{Type::void_, {Type::long_, Type::long_, Type::i8_ptr, Type::i8_ptr}, ADDR((void*) throw2)},
	});

	method("debug", {
		{Type::void_, {Type::any}, ADDR(print)}
	});

	method("internal_print", {
		{Type::void_, {Type::i8->pointer(), Type::const_boolean}, ADDR((void*) internal_print_bool)},
		{Type::void_, {Type::i8->pointer(), Type::const_integer}, ADDR((void*) internal_print_int)},
		{Type::void_, {Type::i8->pointer(), Type::const_long}, ADDR((void*) internal_print_long)},
		{Type::void_, {Type::i8->pointer(), Type::const_real}, ADDR((void*) internal_print_real)},
		{Type::void_, {Type::i8->pointer(), Type::mpz_ptr}, ADDR((void*) internal_print_mpz)},
		{Type::void_, {Type::i8->pointer(), Type::tmp_mpz_ptr}, ADDR((void*) internal_print_mpz_tmp)},
		{Type::void_, {Type::i8->pointer(), Type::const_any}, ADDR((void*) internal_print)},
	});
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
	return c.insn_call(Type::void_, {c.get_vm(), args[0]}, "System.internal_print.6");
}
Compiler::value SystemSTD::print_int(Compiler& c, std::vector<Compiler::value> args, int) {
	return c.insn_call(Type::void_, {c.get_vm(), args[0]}, "System.internal_print.1");
}
Compiler::value SystemSTD::print_mpz(Compiler& c, std::vector<Compiler::value> args, int) {
	return c.insn_call(Type::void_, {c.get_vm(), args[0]}, "System.internal_print.4");
}
Compiler::value SystemSTD::print_mpz_tmp(Compiler& c, std::vector<Compiler::value> args, int) {
	return c.insn_call(Type::void_, {c.get_vm(), args[0]}, "System.internal_print.5");
}
Compiler::value SystemSTD::print_long(Compiler& c, std::vector<Compiler::value> args, int) {
	return c.insn_call(Type::void_, {c.get_vm(), args[0]}, "System.internal_print.2");
}
Compiler::value SystemSTD::print_bool(Compiler& c, std::vector<Compiler::value> args, int) {
	return c.insn_call(Type::void_, {c.get_vm(), args[0]}, "System.internal_print.0");
}
Compiler::value SystemSTD::print_real(Compiler& c, std::vector<Compiler::value> args, int) {
	return c.insn_call(Type::void_, {c.get_vm(), args[0]}, "System.internal_print.3");
}

void SystemSTD::internal_print(VM* vm, LSValue* value) {
	std::cout << "print int " << vm << std::endl;
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
	auto ex = vm::ExceptionObj((vm::Exception) type);
	ex.frames.push_back({file, function, line});
	throw ex;
}

void fake_ex_destru_fun(void*) {}
void SystemSTD::throw2(void** ex, char* file, char* function, size_t line) {
	auto exception = (vm::ExceptionObj*) (ex + 4);
	exception->frames.push_back({file, function, line});
	__cxa_throw(exception, (void*) &typeid(vm::ExceptionObj), &fake_ex_destru_fun);
}

#endif

}
