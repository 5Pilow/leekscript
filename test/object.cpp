#include "Test.hpp"
#include "../src/type/Type.hpp"

void Test::test_objects() {
	auto& env = getEnv();

	/*
	 * General
	 */
	header("Objects");
	code("Object()").equals("{}");
	code("new Object").equals("{}");
	code("new Object()").equals("{}");
	code("{}").equals("{}");
	code("{a: 12}").equals("{a: 12}");
	code("{a: 12, b: 5}").equals("{a: 12, b: 5}");
	code("{a: {}, b: []}").equals("{a: {}, b: []}");
	code("let a = {} a").equals("{}");
	code("let a = {b: 12, c: 5} a").equals("{b: 12, c: 5}");

	section("Objects with functions");
	code("var f = obj -> obj.a f({a: 'foo'})").equals("'foo'");
	code("var f = obj -> obj.a [f({a: 'foo'}), f({a: 'bar'})]").equals("['foo', 'bar']");
	code("var f = obj -> obj.a [f(12), f({a: 'bar'})]").error(ls::Error::NO_SUCH_ATTRIBUTE, {"a", "Number"});

	section("No commas");
	code("{a: 12 b: 5}").equals("{a: 12, b: 5}");
	code("{a: 12 - 2 yo: -6}").equals("{a: 10, yo: -6}");
	code("{a: 12 b: 'yo' c: true d: [1 2 3]}").equals("{a: 12, b: 'yo', c: true, d: [1, 2, 3]}");

	/*
	 * Operators
	 */
	section("Object.operator !");
	code("!{}").equals("true");
	code("!{a: 32}").equals("false");

	section("Object.operator | |");
	code("let a = {a: 32, b: 'toto', c: false}; |a|").equals("3");

	section("Object.operator in ()");
	code("12 in {x: 5, y: 12}").equals("true");
	code("12 in {x: 5, y: 'yo'}").equals("false");

	section("Object.operator . ()");
	code("let a = {b: 12, c: 5} a.b").equals("12");
	code("let a = {v: 5} a.v = 12").equals("12");
	code("let a = {v: 5} a.v = 12 a").equals("{v: 12}");
	code("let a = {v: 5} a.v = 'salut'").equals("'salut'");
	code("let a = {v: 5} a.v = 'salut' a").equals("{v: 'salut'}");
	code("let a = {b: 12} a.b += 10").equals("22");
	code("let a = {b: 12} a.b -= 10").equals("2");
	code("let a = {b: 12} a.b *= 10").equals("120");
	code("let a = {b: 12} a.b /= 10").almost(1.2);
	code("let a = {b: 12} a.b %= 10").equals("2");
	code("let o = {} o.new_val = 12 o").equals("{new_val: 12}");
	code("let o = {a: 'a'} o.b = 'b' o").equals("{a: 'a', b: 'b'}");
	DISABLED_code("Object.readonly.v = 5").exception(ls::vm::Exception::CANT_MODIFY_READONLY_OBJECT);
	code("let o = [{}, ''][0] o.values").equals("<function>");
	code("let pq = [{p: 22, v: 55}] pq[0].p").equals("22");
	code("let pq = [{p: 22, v: 55}] let o = pq[0] o.v").equals("55");

	section("Object.operator ==");
	code("class A {} {} == new A").equals("false");
	code("class A {} class B {} new A == new B").equals("false");
	code("class A {} new A == new A").equals("true");
	code("{a: 2} == {}").equals("false");
	code("{a: 2} == {a: 1}").equals("false");
	code("{a: 2} == {b: 2}").equals("false");
	code("{a: 2} == {a: 2}").equals("true");

	section("Object.operator <");
	code("{} < {}").equals("false");
	code("{a: 2} < {a: 3}").equals("true");
	code("{a: 2} < {a: 1}").equals("false");
	code("{a: 'b'} < {a: 'c'}").equals("true");
	code("{a: 'b'} < {a: 'a'}").equals("false");
	code("{b: 2} < {c: 2}").equals("true");
	code("{b: 2} < {a: 2}").equals("false");
	code("{a: 1} < {a: 1, b: 2}").equals("true");
	code("{a: 1, b: 2} < {a: 1}").equals("false");
	code("{a: 0, b: 2} < {a: 1}").equals("true");
	code("class A {} class B {} new A < new B").equals("true");
	code("class A {} class B {} new B < new A").equals("false");
	code("class A {} {} < new A").equals("true");
	code("class A {} new A < {}").equals("false");

	/*
	 * Iteration
	 */
	code("var s = '' for v in {a: 5, b: 'hello'} { s += v } s").error(ls::Error::Type::VALUE_NOT_ITERABLE, {"{a: 5, b: 'hello'}", env.tmp_object->to_string()}); // TODO .equals("'5hello'");

	/*
	 * Methods
	 */
	section("Object.keys()");
	code("{}.keys()").equals("[]");
	code("{a: 5, b: 'toto', c: true, d: -> 5}.keys()").equals("['a', 'b', 'c', 'd']");
	code("'x' in {x: 5, y: 'yo'}.keys()").equals("true");
	code("'x' in {a: 5, y: 'yo'}.keys()").equals("false");

	section("Object.values()");
	code("{}.values()").equals("[]");
	code("{a: 1}.values()").equals("[1]");
	code("{a: 1, b: 1}.values()").equals("[1, 1]");
	code("{a: 5, b: 'toto', c: true, d: -> 5}.values()").equals("[5, 'toto', true, <function>]");

	section("Object.isTrue()");
	code("if ({x: 12}) { 5 } else { 12 }").equals("5");
	code("if ({}) { 5 } else { 12 }").equals("12");

	section("Object.clone()");
	code("let a = {v: 12} [a]").equals("[{v: 12}]");

	section("Object.map()");
	code("{}.map(x -> x + 1)").equals("{}");
	code("{x: 12, y: 5}.map(x -> x + 1)").equals("{x: 13, y: 6}");
	code("{x: 'a', y: 'b'}.map(x -> x + ' !')").equals("{x: 'a !', y: 'b !'}");
}
