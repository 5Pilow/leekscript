#include "SystemSTD.hpp"
#include <chrono>
#include "../../vm/LSValue.hpp"
#include "../../constants.h"
#include "../../vm/VM.hpp"
#include "../../type/Type.hpp"

namespace ls {

SystemSTD::SystemSTD(VM* vm) : Module(vm, "System") {

	static_field("version", Type::integer, ADDR(version));
	static_field("operations", Type::integer, ADDR([&](ls::Compiler& c) {
		return c.insn_load(c.get_symbol("operations", Type::integer->pointer()));
	}));
	static_field_fun("time", Type::long_, ADDR((void*) time));
	static_field_fun("milliTime", Type::long_, ADDR((void*) millitime));
	static_field_fun("microTime", Type::long_, ADDR((void*) microtime));
	static_field_fun("nanoTime", Type::long_, ADDR((void*) nanotime));

	method("print", {
		{Type::void_, {Type::const_any}, ADDR((void*) print)},
		{Type::void_, {Type::mpz_ptr}, ADDR((void*) print_mpz)},
		{Type::void_, {Type::tmp_mpz_ptr}, ADDR((void*) print_mpz_tmp)},
		{Type::void_, {Type::const_long}, ADDR((void*) print_long)},
		{Type::void_, {Type::const_real}, ADDR((void*) print_float)},
		{Type::void_, {Type::const_integer}, ADDR((void*) print_int)},
		{Type::void_, {Type::const_boolean}, ADDR((void*) print_bool)},
	});

	method("throw", {
		{Type::void_, {Type::integer, Type::i8_ptr, Type::i8_ptr, Type::long_}, ADDR((void*) throw1)},
		{Type::void_, {Type::long_, Type::long_, Type::i8_ptr, Type::i8_ptr}, ADDR((void*) throw2)},
	});

	method("debug", {
		{Type::void_, {Type::any}, ADDR((void*) print)}
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

void SystemSTD::print(LSValue* value) {
	value->print(VM::current()->output->stream());
	VM::current()->output->end();
	LSValue::delete_temporary(value);
}

void SystemSTD::print_int(int v) {
	VM::current()->output->stream() << v;
	VM::current()->output->end();
}

void SystemSTD::print_mpz(__mpz_struct* v) {
	char buff[1000];
	mpz_get_str(buff, 10, v);
	VM::current()->output->stream() << buff;
	VM::current()->output->end();
}
void SystemSTD::print_mpz_tmp(__mpz_struct* v) {
	char buff[1000];
	mpz_get_str(buff, 10, v);
	VM::current()->output->stream() << buff;
	VM::current()->output->end();
	mpz_clear(v);
	VM::current()->mpz_deleted++;
}

void SystemSTD::print_long(long v) {
	VM::current()->output->stream() << v;
	VM::current()->output->end();
}

void SystemSTD::print_bool(bool v) {
	VM::current()->output->stream() << std::boolalpha << v;
	VM::current()->output->end();
}

void SystemSTD::print_float(double v) {
	VM::current()->output->stream() << v;
	VM::current()->output->end();
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
