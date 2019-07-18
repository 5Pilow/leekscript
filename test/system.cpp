#include "Test.hpp"

#include <chrono>
#include "../src/analyzer/error/Error.hpp"

long get_sec_time() {
	return std::chrono::duration_cast<std::chrono::seconds>(
		std::chrono::system_clock::now().time_since_epoch()
	).count();
}

long get_milli_time() {
	return std::chrono::duration_cast<std::chrono::milliseconds>(
		std::chrono::system_clock::now().time_since_epoch()
	).count();
}

long get_micro_time() {
	return std::chrono::duration_cast<std::chrono::microseconds>(
		std::chrono::system_clock::now().time_since_epoch()
	).count();
}

long get_nano_time() {
	return std::chrono::duration_cast<std::chrono::nanoseconds>(
		std::chrono::system_clock::now().time_since_epoch()
	).count();
}

void Test::test_system() {

	header("System");

	code("System.version").equals("2");

	code("System.operations").ops_limit(1000).equals("0");
	code("2 + 2 System.operations").ops_limit(1000).equals("1");

	long sec_time = get_sec_time();
	code("System.time").almost(sec_time, 1L);

	long milli_time = get_milli_time();
	code("System.milliTime").almost(milli_time, 100L);

	long micro_time = get_micro_time();
	code("System.microTime").almost(micro_time, 100000L);

	long nano_time = get_nano_time();
	code("System.nanoTime").almost(nano_time, 100000000L);

	code("let a = System.print(12)").error(ls::Error::Type::CANT_ASSIGN_VOID, {"a"});
	code("var a = 2 a = System.print(12)").error(ls::Error::Type::CANT_ASSIGN_VOID, {"a"});
	code("let p = System.print p('salut')").output("salut\n");
	code("let p = System.print p(12)").output("12\n");

	section("print()");
	code("System.print(true)").output("true\n");
	code("System.print(12)").output("12\n");
	code("System.print(123.456)").output("123.456\n");
	code("System.print(1234567123456)").output("1234567123456\n");
	code("System.print(12345671234561234567123456)").output("12345671234561234567123456\n");
	code("System.print(12m ** 34m)").output("4922235242952026704037113243122008064\n");
	code("let a = 52026704037113243m System.print(a)").output("52026704037113243\n");
	code("System.print('salut !')").output("salut !\n");
	code("System.print([1, 2, 3])").output("[1, 2, 3]\n");
	code("System.print(['a', 'b', 'c'])").output("['a', 'b', 'c']\n");
	code("let a = 123 System.print(123)").output("123\n");
	code("let a = 123 System.print(a)").output("123\n");
	code("let a = 'hello' System.print(a)").output("hello\n");
	code("print(12)").output("12\n");
	code("print('hello')").output("hello\n");
	code("print([1, 2, 3])").output("[1, 2, 3]\n");

	section("v1 debug");
	code_v1("debug('hello')").output("hello\n");
}
