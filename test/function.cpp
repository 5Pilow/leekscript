#include "Test.hpp"

void Test::test_functions() {

	/*
	 * Functions
	 */
	header("Functions");
	success("function foo(x, y) { x + y } foo(1, 2)", "3");

	sem_err("null()", ls::SemanticException::Type::CANNOT_CALL_VALUE, "null");
	sem_err("12()", ls::SemanticException::Type::CANNOT_CALL_VALUE, "12");
	sem_err("'hello'()", ls::SemanticException::Type::CANNOT_CALL_VALUE, "'hello'");
	sem_err("[1, 2, 3]()", ls::SemanticException::Type::CANNOT_CALL_VALUE, "array[1, 2, 3]");

	/*
	 * Lambdas
	 */
	header("Functions / Lambdas");
	success("let f = x -> x f(12)", "12");
	success("let f = x -> x ** 2 f(12)", "144");
	success("let f = x, y -> x + y f(5, 12)", "17");
	success("let f = -> 12 f()", "12");
	success("(x -> x)(12)", "12");
	success("(x, y -> x + y)(12, 5)", "17");
	success("( -> [])()", "array[]");
	success("( -> 12)()", "12");
	success("let f = x -> x f(5) + f(7)", "12");
	success("[-> 12][0]()", "12");
	success("[-> 12, 'toto'][0]()", "12");
	success("(x -> x + 12.12)(1.01)", "13.129999999999999005");
	success("(x -> x + 12)(1.01)", "13.009999999999999787");
	success("[x -> x ** 2][0](12)", "144");
	success("[[x -> x ** 2]][0][0](12)", "144");
	success("[[[x -> x ** 2]]][0][0][0](12)", "144");
	success("[[[[[[[x -> x ** 2]]]]]]][0][0][0][0][0][0][0](12)", "144");
	success("(-> -> 12)()()", "12");
	success("let f = -> -> 12 f()()", "12");
	success("let f = x -> -> 'salut' f()()", "'salut'");
	success("let f = x -> [x, x, x] f(44)", "array[44, 44, 44]");
	success("let f = function(x) { let r = x ** 2 return r + 1 } f(10)", "101");
	success("1; 2", "2");
	success("return 1; 2", "1");
	success("let f = function(x) { if (x < 10) {return true} return 12 } [f(5), f(20)]", "array[true, 12]");
	//	success("let a = 10 a ~ x -> x ^ 2", "100");

	/*
	 * Closures
	 */
	header("Closures");
	//	success("let a = 5 let f = -> a f()", "5");
	//	success("let f = x -> y -> x + y let g = f(5) g(12)", "17");
	//	success("let f = x -> y -> x + y f(5)(12)", "17");
	//	success("let f = x -> x (-> f(12))()", "12");
	//	success("let f = x -> x let g = x -> f(x) g(12)", "12");
	//	success("let g = x -> x ^ 2 let f = x, y -> g(x + y) f(6, 2)", "64");

	header("Recursive");
	//	success("let fact = x -> if x == 1 { 1 } else { fact(x - 1) * x } fact(10)", "3628800");

	/*
	 * Function operators
	 */
	header("Function operators");

	success("+(1, 2)", "3");
	success("+([1], 2)", "array[1, 2]");
	success("+('test', 2)", "'test2'");
//	success("-(9, 2)", "7");
	success("*(5, 8)", "40");
	success("*('test', 2)", "'testtest'");
	success("×(5, 8)", "40");
	success("×('test', 2)", "'testtest'");
	success("/(48, 12)", "4");
	success("/('banana', 'n')", "array['ba', 'a', 'a']");
	success("÷(48, 12)", "4");
	success("÷('banana', 'n')", "array['ba', 'a', 'a']");
	success("**(2, 11)", "2048");
	success("%(48, 5)", "3");
	success("let p = + p(1, 2)", "3");
	success("let p = + p('test', 2)", "'test2'");
	//success("let p = -; p(9, 2)", "7");
	success("let p = * p(5, 8)", "40");
	success("let p = × p(5, 8)", "40");
	success("let p = / p(48, 12)", "4");
	success("let p = ÷ p(48, 12)", "4");
	success("let p = % p(48, 5)", "3");
	success("let p = ** p(2, 11)", "2048");
	success("+", "<function>");
	success("+.class", "<class Function>");
	success("let p = +; p.class", "<class Function>");
}
