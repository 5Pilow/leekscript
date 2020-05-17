#include "Test.hpp"

void Test::test_functions() {

	header("Functions");
	code("x -> x").equals("<function>");
	code("function foo(x, y) { x + y } foo(1, 2)").equals("3");
	code("function f() { return 12 } [f(), 'str']").equals("[12, 'str']");
	code("function f() { 12 } [f(), 'str']").equals("[12, 'str']");
	code("function bar(u, v) return u * v - u / v end bar(12, 5)").equals("57.6");
	code("function bar(u, v) u * v - u / v end bar(12, 5)").equals("57.6");
	code("function test() { return }").equals("(void)");
	code("function test() { return ; }").equals("(void)");
	code("function test() { return ; } test()").equals("(void)");

	section("Global functions");
	code("let v = global_fun() function global_fun() { return 1234567 } v").equals("1234567");
	code("let v = global_fun() function global_fun() { return 'hello' } v").equals("'hello'");
	code("let v = global_fun(5) function global_fun(x) { return x + 12 } v").equals("17");

	section("Can't call a value");
	code("null()").error(ls::Error::Type::CANNOT_CALL_VALUE, {"null"});
	code("12()").error(ls::Error::Type::CANNOT_CALL_VALUE, {"12"});
	code("'hello'()").error(ls::Error::Type::CANNOT_CALL_VALUE, {"'hello'"});
	code("[1, 2, 3]()").error(ls::Error::Type::CANNOT_CALL_VALUE, {"[1, 2, 3]"});

	section("Simple returns");
	code("1; 2").equals("2");
	code("return 1").equals("1");
	code("return 'a'").equals("'a'");
	code("let x = 'yolo' return '1'; 2").equals("'1'");
	code("let x = '1' return x; 2").equals("'1'");
	code("{ return 12; 'salut' }").equals("12");
	code("{ { return 12; 'salut' } 'salut' }").equals("12");
	code("var a = 2 var b = 'salut' return a + b").equals("'2salut'");

	section("Functions / Lambdas");
	code("let f = x -> x f(12)").equals("12");
	code("let f = x -> x ** 2 f(12)").equals("144");
	code("let f = x, y -> x + y f(5, 12)").equals("17");
	code("let f = -> 12 f()").equals("12");
	code("(x -> x)(12)").equals("12");
	code("(x, y -> x + y)(12, 5)").equals("17");
	code("( -> [])()").equals("[]");
	code("( -> 12)()").equals("12");
	code("let f = x -> x f(5) + f(7)").equals("12");
	code("[-> 12][0]()").equals("12");
	code("[-> 12, 'toto'][0]()").equals("12");
	code("(x -> x + 12.12)(1.01)").almost(13.13);
	code("(x -> x + 12)(1.01)").almost(13.01);
	code("[x -> x ** 2][0](12)").equals("144");
	code("[[x -> x ** 2]][0][0](12)").equals("144");
	code("[[[x -> x ** 2]]][0][0][0](12)").equals("144");
	code("[[[[[[[x -> x ** 2]]]]]]][0][0][0][0][0][0][0](12)").equals("144");
	code("(-> -> 12)()()").equals("12");
	code("(-> -> -> 13)()()()").equals("13");
	code("(-> -> -> -> 14)()()()()").equals("14");
	code("(-> -> -> -> -> 15)()()()()()").equals("15");
	code("let f = -> -> 12 f()()").equals("12");
	code("let f = x -> -> 'salut' f(5)()").equals("'salut'");
	code("let f = x -> [x, x, x] f(44)").equals("[44, 44, 44]");
	code("let f = function(x) { let r = x ** 2 return r + 1 } f(10)").equals("101");
	code("let f = function(x) { if (x < 10) {return true} return 12 } [f(5), f(20)]").equals("[true, 12]");
	code("let f = function(x) { if (x < 10) {return true} else {return 12} } [f(5), f(20)]").equals("[true, 12]");
	DISABLED_code("let f = x -> { let y = { if x == 0 { return 'error' } 1/x } '' + y } [f(-2), f(0), f(2)]").equals("['-0.5', 'error', '0.5']");
	code("let f = i -> { [1 2 3][i] } f(1)").equals("2");
	code("let f = i -> { [1 2 3][i] } 42").equals("42");
	code("let f = a, i -> a[i] f([1 2 3], 1)").equals("2");
	code("[x -> x][0]").equals("<function>");
	code("let f = x = 2 -> x + 1 f").equals("<function>");
	code("let f = b -> if b { 2 } else { 3 } f(false)").equals("3");
	code("let f = b => {b = !b if b { 2 } else { 3 }} f(false)").equals("2");
	code("(x -> y -> x + 1)(1)(2)").equals("2");
	code("let f = x, y -> { x += '+' y += '.' } var a = 'A', b = 'B' f(a, b) [a, b]").equals("['A+', 'B.']");
	code("let f = -> 12m f()").equals("12");
	code("let f = x => x f(12m)").equals("12");

	section("Function call without commas");
	code("let f = x, y -> x + y f(12 7)").equals("19");
	code("String.replace('banana' 'a' '_')").equals("'b_n_n_'");

	section("Arguments");
	code("let f = x => { x++ x *= 2 return x } f(12)").equals("26");

	section("Closures");
	code("let a = 5 let f = -> a f()").equals("5");
	code("let a = 12 let f = -> -> a f()()").equals("12");
	code("let a = 12 let f = -> -> -> a f()()()").equals("12");
	code("let a = 12 let f = -> -> -> -> -> a f()()()()()").equals("12");
	DISABLED_code("let a = 12 let f = -> -> {let b = 5; -> -> -> a + b} f()()()()()").equals("17");
	DISABLED_code("let a = 'a' let f = -> -> {let b = 'b'; -> -> -> a + b} f()()()()()").equals("'ab'");
	DISABLED_code("let f = x -> y -> x + y let g = f(5) g(12)").equals("17");
	code("let a = 12 let f = x -> y -> x + y + a f(5)(2)").equals("19");
	DISABLED_code("let f = x -> y -> x + y let g = f('a') g('b')").equals("'ab'");
	code("let f = x -> y -> x + y f(5)(12)").equals("17");
	code("let f = x -> y -> x + y f('a')('b')").equals("'ab'");
	code("let f = x -> x (-> f(12))()").equals("12");
	code("let f = x -> x let g = x -> f(x) g(12)").equals("12");
	code("let g = x -> x ** 2 let f = x, y -> g(x + y) f(6, 2)").equals("64");
	code("let a = 5 let f = x -> x < a [1, 2, 3, 4, 5, 6].filter(f)").equals("[1, 2, 3, 4]");
	code("var g = x => { var y = 2; return x + y } g(10)").equals("12");
	code("let a = 12, b = 13, c = 14 let f = x -> x + a + b + c f(5)").equals("44");
	DISABLED_code("let f = x -> y -> x + y let g1 = f(5) let g2 = f('a') [g1(12) g1('b') g2(12) g2('b')]").equals("[]");
	code("var a = 2 a++ let f = x => x + a print(f(10)) a += 5 print(f(10))").output("13\n18\n");
	DISABLED_code("function g(a) { a++ let f = x => x + a a += 5 f(10) } g(10)").equals("26");
	DISABLED_code("let f = -> 12 let g = -> f g()()").equals("12");

	section("Recursive");
	code("let fact = x -> if x == 1 { 1 } else { fact(x - 1) * x } fact(8)").equals("40320");
	code("let fact = x -> if x == 1 { 1m } else { fact(x - 1) * x } fact(30m)").equals("265252859812191058636308480000000");
	code("let fact = x -> if x == 1 { 1m } else { fact(x - 1) * x } fact(30)").equals("265252859812191058636308480000000");
	code("let fact = x -> if x > 1 { fact(x - 1) * x } else { 1 } fact(10)").equals("3628800");
	code("let fib = n -> if n <= 1 { n } else { fib(n - 1) + fib(n - 2) } fib(25)").equals("75025");
	code("let fact = x -> if x > 1 x * fact(x - 1) else x fact(5)").equals("120");
	code("let test = x -> if x > 0 { test(x - 1) } else { 77 } test(4)").equals("77");
	code("let fact = (x, a) -> { if x == 0 then return a end return fact(x - 1, x * a) } fact(10, 1)").equals("3628800");
	code("let fact = (x, a) -> { if x == 0m then return a end return fact(x - 1, x * a) } fact(10m, 1m)").equals("3628800");
	code("function test() { let fact = x -> if x == 1 { 1 } else { fact(x - 1) * x } fact(8) } test()").equals("40320");

	section("Functions in array");
	code("var a = [12, x -> x + 7] a[1](12)").equals("19");
	code("var a = [12, x -> x + '!'] a[1](12)").equals("'12!'");
	code("let hl = [1, 'text', x -> x + 1] hl[2](hl[1]) + hl[2](hl[0])").equals("'text12'");
	code("var a = [] a.push(x -> 12)").equals("[<function>]");
	DISABLED_code("var a = [] a.push(x -> 12) a[0]").equals("<function>");

	section("Multiple versions of a function");
	code("let f = x -> x f(5) f('a')").equals("'a'");
	code("let f = x -> x f('a') f(5)").equals("5");
	code("let f = x -> x [f(5), f('a')]").equals("[5, 'a']");
	code("let f = x -> x [f(5), f('a'), f(5.5), f(2l)]").equals("[5, 'a', 5.5, 2]");

	/*
	 * Operators
	 */
	section("Function.operator !");
	code("!(x -> x)").equals("false");

	section("Function.operator ~");
	code("~(x -> x)").error(ls::Error::NO_SUCH_OPERATOR, {"~", "x => x"});

	section("Function.operator ++x");
	code("++(x -> x)").error(ls::Error::CANT_MODIFY_CONSTANT_VALUE, { "x => x" });

	section("Function.operator --x");
	code("--(x -> x)").error(ls::Error::CANT_MODIFY_CONSTANT_VALUE, { "x => x" });

	section("Function.operator x++");
	code("(x -> x)++").error(ls::Error::CANT_MODIFY_CONSTANT_VALUE, { "x => x" });

	section("Function.operator x--");
	code("(x -> x)--").error(ls::Error::CANT_MODIFY_CONSTANT_VALUE, { "x => x" });

	section("Operator ~ ");
	code("let a = 10 a ~ x -> x ** 2").equals("100");
	code("let a = 10.5 a ~ x -> x * 5").equals("52.5");
	code("3 ~ x -> x ** x").equals("27");
	code("1993 ~ Number.isPrime").equals("true");
	code("5l ~ x -> x ** x").equals("3125");
	code("true ~ x -> !x").equals("false");
	code("[1, 2, 3] ~ x -> x + 4").equals("[1, 2, 3, 4]");
	code("['a', 'b', 'c'] ~ x -> x.size()").equals("3");
	code("['a', 'b', 'c'] ~ Array.size").equals("3");
	code("(~)(5, x => x + 1000)").equals("1005");
	code("(~)(5l, x => x * 1000)").equals("5000");
	code("(~)([], x => x + 'a')").equals("['a']");
	code("let f = x => x.cos(); (~)(Number.pi, f)").equals("-1");

	section("Operator []");
	code("let f = x -> x f[2] = 5").error(ls::Error::Type::VALUE_MUST_BE_A_CONTAINER, {"f"});

	section("Function operators");
	code("(+)(1, 2)").equals("3");
	code("(+)([1], 2)").equals("[1, 2]");
	code("(+)('test', 2)").equals("'test2'");
	code("(-)(9, 2)").equals("7");
	code("*(5, 8)").equals("40");
	code("*('test', 2)").equals("'testtest'");
	code("×(5, 8)").equals("40");
	code("×('test', 2)").equals("'testtest'");
	code("**(2, 11)").equals("2048");
	code("/(48, 12)").equals("4");
	code("/('banana', 'n')").equals("['ba', 'a', 'a']");
	code("÷(48, 12)").equals("4");
	code("÷('banana', 'n')").equals("['ba', 'a', 'a']");
	code("**(2, 11)").equals("2048");
	code("%(48, 5)").equals("3");
	code("\\(72, 7)").equals("10");
	code("(\\)(72, 7)").equals("10");
	code("['', **(2, 11)]").equals("['', 2048]");
	code("(<)(1, 2)").equals("true");
	code("(>)(1, 2)").equals("false");
	code("(<)('a', 'b')").equals("true");
	code("(>)('a', 'b')").equals("false");
	code("(<=)(1, 1)").equals("true");
	code("(<=)(1, 2)").equals("true");
	code("(>=)(5, 5)").equals("true");
	code("(>=)(5, 6)").equals("false");
	code("let p = +; p(1, 2)").equals("3");
	code("let p = +; p('test', 2)").equals("'test2'");
	code("let p = -; p(9, 2)").equals("7");
	code("let p = * p(5, 8)").equals("40");
	code("let p = × p(5, 8)").equals("40");
	code("let p = / p(48, 12)").equals("4");
	code("let p = ÷ p(48, 12)").equals("4");
	code("let p = % p(48, 5)").equals("3");
	code("let p = ** p(2, 11)").equals("2048");
	code("let p = \\ p(72, 7)").equals("10");
	code("let p = <; p('a', 'b')").equals("true");
	code("let p = >; p('a', 'b')").equals("false");
	code("let p = <=; p('a', 'b')").equals("true");
	code("let p = >=; p('a', 'b')").equals("false");
	code("+").equals("<function>");
	code("+.class").equals("<class Function>");
	code("let p = +; p.class").equals("<class Function>");

	section("Function.isTrue()");
	code("if [x -> x, 12][0] { 'ok' } else { null }").equals("'ok'");

	section("Function.operator ==");
	code("let a = x -> x; a == a").equals("true");
	code("let a = x -> x; a == 2").equals("false");

	section("Function.operator <");
	code("let a = x -> x; a < a").equals("false");
	code("[x -> x, ''][0] < 5").equals("false");

	section("STD method");
	code("String.size").equals("<function>");
	code("Number.cos").equals("<function>");

	section("Check argument count");
	code("(x -> x)()").error(ls::Error::Type::WRONG_ARGUMENT_COUNT, {"x => x", "1", "0"});
	code("let f = x, y -> x + y f(5)").error(ls::Error::Type::WRONG_ARGUMENT_COUNT, {"f", "2", "1"});
	code("let add = +; add(5, 12, 13)").error(ls::Error::Type::WRONG_ARGUMENT_COUNT, {"add", "2", "3"});
	code("Number.abs(12, 12)").error(ls::Error::Type::WRONG_ARGUMENT_COUNT, {"Number.abs", "1", "2"});
	code("let siftUp = (c, pq) -> pq; let pqInsert = (p, v, pq) -> siftUp(0, pq); pqInsert(1, 2)").error(ls::Error::Type::WRONG_ARGUMENT_COUNT, {"pqInsert", "3", "2"});

	section("Void functions");
	code("(x -> System.print(x))(43)").equals("(void)");

	section("Default arguments");
	code("(x = 10) -> x").equals("<function>");
	code("x = 10 -> x").equals("<function>");
	code("(x = 10 -> x)").equals("<function>");
	code("let f = x = 10 -> x f()").equals("10");
	code("let f = (x = 10) -> x f()").equals("10");
	code("let f = (x = -10) -> x f()").equals("-10");
	code("let f = (x = 10) -> x f(5)").equals("5");
	code("let f = (x = 12.12) -> x f()").equals("12.12");
	code("let f = (x = 12.12) -> x f(5)").equals("5");
	code("let f = (x = 1212l) -> x f()").equals("1212");
	code("let f = (x = 1212l) -> x f(5)").equals("5");
	code("function f(x = 10) { x } f()").equals("10");
	code("let add = (x, y = 1) -> x + y add(5)").equals("6");
	code("let add = (x, y = 1) -> x + y add(5, 3)").equals("8");
	code("let add = (x, y = 1) -> x + y add()").error(ls::Error::WRONG_ARGUMENT_COUNT, {"add", "2", "1"});
	code("let f = (x = 'a', y = 'b') -> x + y f()").equals("'ab'");
	code("let f = (x = 'a', y = 'b') -> x + y f(100, 12)").equals("112");
	code("let f = (x = 'a', y = 'b') -> x + y f(100)").equals("'100b'");
	code("let f = (x = 'a', y = 'b') -> x + y f(100)").equals("'100b'");
	code("let add = (x, y = 1) -> x + y var a = 12 add(a)").equals("13");
	code("let add = (x, y = 1) -> x + y var a = 12 add(a, 10)").equals("22");

	section("Default arguments : advanced");
	code("let f = (x = [1, 2, 3]) -> x.size() f()").equals("3");
	code("let f = (x = [1, 2, 3]) -> x.size() f([])").equals("0");
	code("let f = (x = [1, 2, 3]) -> x.size() f([1, 2, 3, 4, 5])").equals("5");
	code("let f = (x = [1, 2, 3]) -> x.size() f('bonjour')").equals("7");

	section("Default arguments : expert");
	DISABLED_code("let f = (x = (y = 'abcd') -> y.size()) -> x f()").equals("<function>");
	code("let f = (x = (y = 'abcd') -> y.size()) -> x f([])").equals("[]");
	code("let f = (x = (y = 'abcd') -> y.size()) -> x f(2)").equals("2");
	code("let f = (x = 'AA') -> (y = 'BB') -> x + y f()()").equals("'AABB'");
	code("let f = (x = 'AA') -> (y = 'BB') -> x + y f()(4)").equals("'AA4'");
	code("let f = (x = 'AA') -> (y = 'BB') -> x + y f(5)()").equals("'5BB'");
	code("let f = (x = 'AA') -> (y = 'BB') -> x + y f(5)(4)").equals("9");

	section("Not compiled functions");
	code("var dumb = function(x) { }").equals("(void)");
	code("var dumb = function(x) { } var dumber = function(y) { dumb(y) }").equals("(void)");
	code("var dumb = function(x) { } var dumber = function(y) { let z = y dumb(z) }").equals("(void)");

	section("Weird");
	code("1 / 5; (-> 0)()").works();
}
