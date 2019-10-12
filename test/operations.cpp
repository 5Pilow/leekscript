#include "Test.hpp"

void Test::test_operations() {

	header("Operations");
	code("2").operations(0);
	code("2 + 2").operations(1);
	code("2 * 3 + 4 * 5").operations(3);
	code("3 < 5").operations(1);
	code("[]").operations(1);
	code("[1, 2, 3]").operations(4);
	code("[1, 2, 3, 4, 5, 6, 7, 8, 9]").operations(10);
	code("[1][0]").operations(4);
	code("x -> x").operations(0);
	code("(x -> x + 1)(12)").operations(3);

	section("GMP operations");
	code("12345m ** 12").operations(169);
	code("12345m ** 120").operations(1681);

	section("Operation limit exceeded");
	code("while true {}").ops_limit(1000).exception(ls::vm::Exception::OPERATION_LIMIT_EXCEEDED);
	code("for ;; {}").ops_limit(1000).exception(ls::vm::Exception::OPERATION_LIMIT_EXCEEDED);
}
