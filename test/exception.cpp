#include "Test.hpp"

void Test::test_exceptions() {

	header("Exceptions");
	code("throw 2").ops_limit(1000).exception(ls::vm::Exception::OPERATION_LIMIT_EXCEEDED);
	code("throw").exception(ls::vm::Exception::EXCEPTION);
	code("12").exception(ls::vm::Exception::NO_EXCEPTION);
	code("var a = 12m; throw").exception(ls::vm::Exception::EXCEPTION);
	code("var a = 12m + 5m; throw").exception(ls::vm::Exception::EXCEPTION);

	section("Exceptions - Avoid leaks");
	code("var x = 'hello' [][0]").exception(ls::vm::Exception::ARRAY_OUT_OF_BOUNDS);
	code("var x = 'hello' throw").exception(ls::vm::Exception::EXCEPTION);
	code("let f = -> { var x = 'hello' [][0] } f()").exception(ls::vm::Exception::ARRAY_OUT_OF_BOUNDS, {
		{"test", "f", 1}, {"test", "main", 1}
	});
	code("let f = -> { var x = 'hello' throw } f()").exception(ls::vm::Exception::EXCEPTION, {
		{"test", "f", 1}, {"test", "main", 1}
	});

	section("Catch-else operator");
	code("2 !? 5").equals("2");
	code("{ throw } !? 5").equals("5");
	code("[][0] !? 6").equals("6");
	code("(2 \\ 0) !? 7").equals("7");
	code("([][0] !? 12) + ([][0] !? 5)").equals("17");
	code("([0][1] !? 12) + ([0][1] !? 5)").equals("17");
	DISABLED_code("let f = -> { { throw } !? 5 } f()").equals("5");
	DISABLED_code("let f = -> { (2 \\ 0) !? 6 } f()").equals("6");
	DISABLED_code("let f = -> { var s = 'leak?' { [][0] } !? 7 } f()").equals("7");
	code("{ var s = 'leak2' [][0] } !? 8").equals("8");
	DISABLED_code("let f = -> { var s = 'leak1' { var s = 'leak2' [][0] } !? 8 } f()").equals("8");
	code("let a = [12][0] !? 'a' null - 5").exception(ls::vm::Exception::NO_SUCH_OPERATOR);

	section("Stacktraces");
	std::string f = "test/code/exception/exception.leek";
	file(f).exception(ls::vm::Exception::DIVISION_BY_ZERO, {
		{f, "main", 3},
	});
	std::string f1 = "test/code/exception/exception_1.leek";
	file(f1).exception(ls::vm::Exception::DIVISION_BY_ZERO, {
		{f1, "crash", 2}, {f1, "power", 7}, {f1, "will_crash", 12}, {f1, "do_something", 17}, {f1, "main", 20}
	});
	std::string f2 = "test/code/exception/exception_2.leek";
	file(f2).exception(ls::vm::Exception::NO_SUCH_OPERATOR, {
		{f2, "crash", 2}, {f2, "power", 7}, {f2, "will_crash", 12}, {f2, "do_something", 17}, {f2, "main", 20}
	});
	std::string f3 = "test/code/exception/exception_3.leek";
	file(f3).exception(ls::vm::Exception::EXCEPTION, {
		{f3, "crash", 2}, {f3, "power", 7}, {f3, "will_crash", 12}, {f3, "do_something", 17}, {f3, "main", 20}
	});
	std::string f4 = "test/code/exception/exception_4.leek";
	file(f4).exception(ls::vm::Exception::NUMBER_OVERFLOW, {
		{f4, "crash", 2}, {f4, "power", 7}, {f4, "will_crash", 12}, {f4, "do_something", 17}, {f4, "main", 20}
	});
	std::string f5 = "test/code/exception/exception_5.leek";
	file(f5).exception(ls::vm::Exception::ARRAY_OUT_OF_BOUNDS, {
		{f5, "crash", 3}, {f5, "power", 8}, {f5, "will_crash", 13}, {f5, "do_something", 18}, {f5, "main", 21}
	});
}
