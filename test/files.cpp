#include "Test.hpp"

void Test::test_files() {

	/** Complex codes */
	header("Files");
	section("General");
	file("test/code/primes.leek").equals("78498");
	file("test/code/primes_gmp.leek").equals("9591");
	// TODO mutable arguments
	DISABLED_file("test/code/gcd.leek").equals("151");
	file("test/code/strings.leek").almost(52.0, 12.0);
	file("test/code/reachable_cells.leek").equals("383");// TODO issue #243
	DISABLED_file("test/code/break_and_continue.leek").equals("2504"); // TODO issue #243
	DISABLED_file("test/code/french.leek").ops_limit(10000).equals("'cent-soixante-huit millions quatre-cent-quatre-vingt-neuf-mille-neuf-cent-quatre-vingt-dix-neuf'");
	DISABLED_file("test/code/french.min.leek").equals("'neuf-cent-quatre-vingt-sept milliards six-cent-cinquante-quatre millions trois-cent-vingt-et-un-mille-douze'");
	file("test/code/quine.leek").quine();
	file_v1("test/code/quine_zwik.leek").quine();
	file("test/code/dynamic_operators").works();
	file("test/code/euler1.leek").equals("2333316668");
	file("test/code/text_analysis.leek").equals("[3, 47, 338]");
	file("test/code/divisors.leek").equals("[1, 3, 9, 13, 17, 39]");
	file("test/code/two_functions.leek").equals("[{p: 2, v: 5}, [{p: 3, v: 6}]]");
	file("test/code/product_n.leek").equals("5040");
	file("test/code/product_n_return.leek").equals("265252859812191058636308480000000");
	file("test/code/product_n_arrays.leek").equals("[5040]");
	file("test/code/product_coproduct.leek").equals("171122452428141311372468338881272839092270544893520369393648040923257279754140647424000000000000000");
	file("test/code/fold_left.leek").equals("[{w: 1}, {w: 3}, {w: 4}, {w: 2}, {w: 7}, {w: 5}, {w: 8}, {w: 9}, {w: 6}]");
	file("test/code/fold_left_2.leek").equals("{p: 6, v: {p: 9, v: {p: 8, v: {p: 5, v: {p: 7, v: { ... }}}}}}");
	file("test/code/fold_right.leek").equals("[{w: 6}, {w: 9}, {w: 8}, {w: 5}, {w: 7}, {w: 2}, {w: 4}, {w: 3}, {w: 1}]");
	file("test/code/fold_right_2.leek").equals("{p: {p: {p: {p: {p: { ... }, v: 7}, v: 2}, v: 4}, v: 3}, v: 1}");
	file("test/code/assignments.leek").equals("15");
	file("test/code/recursive_2_vars.leek").equals("1021");
	file("test/code/global_functions_1.leek").equals("false");
	file("test/code/global_functions_2.leek").equals("[false, true]");
	DISABLED_file("test/code/recursive_2_functions.leek").equals("10");
	// TODO leaks
	DISABLED_file("test/code/recursive_2_versions.leek").equals("9.5");
	DISABLED_file("test/code/swap.leek").equals("[{p: 1}, {p: 3}, {p: 4}, {p: 12}, {p: 5}]");
	file("test/code/classes_simple.leek").equals("['Ferrari', 'Maserati', 'Lamborghini']");
	file("test/code/classes_multiple.leek").equals("[4, 40, 80]");
	DISABLED_file("test/code/match.leek").output("Yeah!\n");
	file("test/code/fibonacci.leek").equals("832040");
	file("test/code/fibonacci_long.leek").equals("1346269");
	// TODO mutable arguments
	DISABLED_file("test/code/pow5.leek").equals("6938893903907228377647697925567626953125");
	file("test/code/tarai.leek").equals("16");
	file("test/code/return_in_function.leek").equals("2");

	section("Issues (fixed)");
	file("test/code/issue/207_basic.leek").equals("[1, 2, 3]");
	DISABLED_file("test/code/issue/207.leek").equals("[{p: 1, v: 1}, {p: 2, v: 2}, {p: 4, v: 4}, {p: 3, v: 3}, {p: 7, v: 7}, {p: 5, v: 5}, {p: 8, v: 8}, {p: 9, v: 9}, {p: 6, v: 6}]");
	file("test/code/issue/121.leek").equals("[123456789087654321345678976543, 2345678654324567897654324567]");

	section("include()");
	code("include('test/code/include/basic.leek')").equals("'basic'");
	code("include('./test/code/include/basic.leek')").equals("'basic'");
	code("include('./test/../test/code/../code/./include/basic.leek')").equals("'basic'");
	code("include('test/code/include/squared.leek') squared(12)").equals("144");
	code("include('test/code/include/car.class.leek') let ferrari = new Car() ferrari.price").equals("300000");
	code("include('test/code/include/hypot.leek') hypot(3, 4)").equals("5");
	code("include('test/code/include/folder/fact.leek')").equals("3628800");
	code("include('test/code/include/exception.leek') except()").exception(ls::vm::Exception::DIVISION_BY_ZERO, {
		{"folder/crash.leek", "crash", 5},
		{"test/code/include/exception.leek", "except", 4},
		{"test", "main", 1}
	});
}
