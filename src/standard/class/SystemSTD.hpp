#ifndef SYSTEMSTD_HPP
#define SYSTEMSTD_HPP

#include "../Module.hpp"

namespace ls {

class SystemSTD : public Module {
public:
	SystemSTD(Environment& env);

	static Compiler::value version(Compiler& c);
	static long time();
	static long millitime();
	static long microtime();
	static long nanotime();

	static Compiler::value print(Compiler& c, std::vector<Compiler::value> args, int);
	static Compiler::value print_int(Compiler& c, std::vector<Compiler::value> args, int);
	static Compiler::value print_mpz(Compiler& c, std::vector<Compiler::value> args, int);
	static Compiler::value print_mpz_tmp(Compiler& c, std::vector<Compiler::value> args, int);
	static Compiler::value print_long(Compiler& c, std::vector<Compiler::value> args, int);
	static Compiler::value print_bool(Compiler& c, std::vector<Compiler::value> args, int);
	static Compiler::value print_real(Compiler& c, std::vector<Compiler::value> args, int);

	static void internal_print(VM* vm, LSValue* v);
	static void internal_print_int(VM* vm, int v);
	static void internal_print_mpz(VM* vm, __mpz_struct* v);
	static void internal_print_mpz_tmp(VM* vm, __mpz_struct* v);
	static void internal_print_long(VM* vm, long v);
	static void internal_print_bool(VM* vm, bool v);
	static void internal_print_real(VM* vm, double v);

	static void throw1(int type, char* file, char* function, size_t line);
	static void throw2(void** ex, char* file, char* function, size_t line);

	static void v1_debug(LSValue* v);
};

}

#endif
