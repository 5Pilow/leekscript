#ifndef SYSTEMSTD_HPP
#define SYSTEMSTD_HPP

#include "../Module.hpp"

namespace ls {

class SystemSTD : public Module {
public:
	SystemSTD(VM* vm);

	static Compiler::value version(Compiler& c);
	static long time();
	static long millitime();
	static long microtime();
	static long nanotime();
	static void print(LSValue* v);
	static void print_int(int v);
	static void print_mpz(__mpz_struct* v);
	static void print_mpz_tmp(__mpz_struct* v);
	static void print_long(long v);
	static void print_bool(bool v);
	static void print_float(double v);

	static void throw1(int type, char* file, char* function, size_t line);
	static void throw2(void** ex, char* file, char* function, size_t line);

	static void v1_debug(LSValue* v);
};

}

#endif
