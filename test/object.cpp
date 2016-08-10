#include "Test.hpp"

void Test::test_objects() {

	header("Objects");

	success("Object()", "{}");
	success("new Object", "{}");
	success("new Object()", "{}");
	success("{}", "{}");
	success("{a: 12}", "{a: 12}");
	success("{a: 12, b: 5}", "{a: 12, b: 5}");
	success("{a: {}, b: []}", "{a: {}, b: array[]}");

	success("let a = {} a", "{}");
	success("let a = {b: 12, c: 5} a", "{b: 12, c: 5}");
	success("let a = {b: 12, c: 5} a.b", "12");

	success("let a = {b: 12} a.b += 10", "22");
	success("let a = {b: 12} a.b -= 10", "2");
	success("let a = {b: 12} a.b *= 10", "120");
	success_almost("let a = {b: 12} a.b /= 10", 1.2);
	success("let a = {b: 12} a.b %= 10", "2");

	success("let a = {a: 32, b: 'toto', c: false}; |a|", "3");

	success("{}.keys()", "array[]");
	success("{a: 5, b: 'toto', c: true, d: -> 5}.keys()", "array['a', 'b', 'c', 'd']");

	success("{}.values()", "array[]");
	success("{a: 5, b: 'toto', c: true, d: -> 5}.values()", "array[5, 'toto', true, <function>]");

	success("var f = obj -> obj.a f({a: 'foo'})", "'foo'");
//	success("var f = obj -> obj.a [f({a: 'foo'}), f({a: 'bar'})]", "array['foo', 'bar']");
//	success("var f = obj -> obj.a [f(12), f({a: 'bar'})]", "array[null, 'bar']");

	success("{a: 12 b: 5}", "{a: 12, b: 5}");
	success("{a: 12 - 2 yo: -6}", "{a: 10, yo: -6}");
	success("{a: 12 b: 'yo' c: true d: [1 2 3]}", "{a: 12, b: 'yo', c: true, d: array[1, 2, 3]}");

	success("12 in {x: 5, y: 12}", "true");
	success("12 in {x: 5, y: 'yo'}", "false");

	success("'x' in {x: 5, y: 'yo'}.keys()", "true");
	success("'x' in {a: 5, y: 'yo'}.keys()", "false");
}
