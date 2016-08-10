#include "Test.hpp"

void Test::test_operators() {

	header("Operator =");

	//success("let a a = 2", "2");
	//success("let a a = 2 a", "2");
	sem_err("2 = 2", ls::SemanticException::Type::VALUE_MUST_BE_A_LVALUE, "<v>");
	sem_err("'hello' = 2", ls::SemanticException::Type::VALUE_MUST_BE_A_LVALUE, "<v>");
	sem_err("[] = 2", ls::SemanticException::Type::VALUE_MUST_BE_A_LVALUE, "<v>");
	sem_err("true = []", ls::SemanticException::Type::VALUE_MUST_BE_A_LVALUE, "<v>");
	sem_err("null = x -> x", ls::SemanticException::Type::VALUE_MUST_BE_A_LVALUE, "<v>");
	sem_err("{} = 2.class", ls::SemanticException::Type::VALUE_MUST_BE_A_LVALUE, "<v>");

	header("Operator xor");

	success("true xor true", "false");
	success("true xor false", "true");
	success("false xor true", "true");
	success("false xor false", "false");

	success("12 xor 5", "false");
	success("12 xor 0", "true");
	success("false xor 99", "true");

	success("'salut' xor 5", "false");
	success("'salut' xor 'yo'", "false");
	success("'salut' xor false", "true");
	success("'salut' xor 0", "true");
	success("'salut' xor ''", "true");

	success("[] xor ''", "false");
	success("[] xor 777", "true");
	success("[1, 2, 3] xor [4, 5, 6]", "false");
	success("[] xor [4, 5, 6]", "true");

	/*
	 * Swap
	 */
	header("Swap");
	success("let a = 2 let b = 5 a <=> b [a, b]", "array[5, 2]");
//	success("let a = [1, 2, 3, 4] a[0] <=> a[3] a", "array[4, 2, 3, 1]");
	success("let a = 12 let b = 5 let s = a <=> b s", "5");

	/*
	a ~~ b => (a ~~ b)
	a ~ ~b => a ~ (~b)
	~~a => ~(~a)
	a; ~~b
	*/
	/*
	var dist = [...]
	dist ~*= 5
	var newdist = dist ~* 5

	res ?? 12 // res ? res : 12
	*/


	/*
	 * Compare
	 */

	header("Comparison");
	success("null < null", "false");
	success("null < true", "true");
	success("null < 1", "true");
	success("null < 'a'", "true");
	success("null < []", "true");
	success("null < {}", "true");

	success("true < null", "false");
	success("true < true", "false");
	//success("true < 1", "true");
	success("true < 'a'", "true");
	success("true < []", "true");
	success("true < {}", "true");

	success("1 < null", "false");
	success("1 < true", "false");
	success("1 < 1", "false");
	success("1 < 'a'", "true");
	success("1 < []", "true");
	success("1 < {}", "true");

	success("'a' < null", "false");
	success("'a' < true", "false");
	success("'a' < 1", "false");
	success("'a' < 'a'", "false");
	success("'a' < []", "true");
	success("'a' < {}", "true");

	success("[] < null", "false");
	success("[] < true", "false");
	success("[] < 1", "false");
	success("[] < 'a'", "false");
	success("[] < []", "false");
	success("[] < {}", "true");

	success("{} < null", "false");
	success("{} < true", "false");
	success("{} < 1", "false");
	success("{} < 'a'", "false");
	success("{} < []", "false");
	success("{} < {}", "false");

	success("null >= null", "true");
	success("null >= false", "false");
	success("null >= 1", "false");
	success("null >= 'a'", "false");
	success("null >= []", "false");
	success("null >= {}", "false");

	success("false >= null", "true");
	success("false >= false", "true");
	success("false >= 1", "false");
	success("false >= 'a'", "false");
	success("false >= []", "false");
	success("false >= {}", "false");

	success("1 >= null", "true");
	success("1 >= false", "true");
	success("1 >= 1", "true");
	success("1 >= 'a'", "false");
	success("1 >= []", "false");
	success("1 >= {}", "false");

	success("'a' >= null", "true");
	success("'a' >= false", "true");
	success("'a' >= 1", "true");
	success("'a' >= 'a'", "true");
	success("'a' >= []", "false");
	success("'a' >= {}", "false");

	success("[] >= null", "true");
	success("[] >= false", "true");
	success("[] >= 1", "true");
	success("[] >= 'a'", "true");
	success("[] >= []", "true");
	success("[] >= {}", "false");

	success("{} >= null", "true");
	success("{} >= false", "true");
	success("{} >= 1", "true");
	success("{} >= 'a'", "true");
	success("{} >= []", "true");
	success("{} >= {}", "true");
}
