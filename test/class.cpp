#include "Test.hpp"

void Test::test_classes() {

	header("Classes");
	section("type ID");
	code("Number.typeID()").equals("11");
	code("[Number, ''][0].typeID()").equals("11");
	code("Number = 12").error(ls::Error::Type::CANT_MODIFY_CONSTANT_VALUE, {"Number"});

	section(".class attribute");
	code("true.class").equals("<class Boolean>");
	code("false.class").equals("<class Boolean>");
	code("0.class").equals("<class Number>");
	code("12.class").equals("<class Number>");
	code("(-5).class").equals("<class Number>");
	code("12.5234.class").equals("<class Number>");
	code("''.class").equals("<class String>");
	code("'salut'.class").equals("<class String>");
	code("[].class").equals("<class Array>");
	code("[1..12].class").equals("<class Interval>");
	code("[:].class").equals("<class Map>");
	code("<>.class").equals("<class Set>");
	code("[1, 2, 3].class").equals("<class Array>");
	code("{}.class").equals("<class Object>");
	code("{a: 12, b: 'yo'}.class").equals("<class Object>");
	code("(-> 12).class").equals("<class Function>");
	code("(x, y -> x + y).class").equals("<class Function>");
	code("12.class.class").equals("<class Class>");
	code("[true, 12, 'foo', [], [1, 2], [1.5, 2.5], [1..12], [1:12], <>, <1.5>, <'a'>, {}, x -> x, Number] ~~ x -> x.class").equals("[<class Boolean>, <class Number>, <class String>, <class Array>, <class Array>, <class Array>, <class Interval>, <class Map>, <class Set>, <class Set>, <class Set>, <class Object>, <class Function>, <class Class>]");

	section("Class.name");
	code("Boolean.name").equals("'Boolean'");
	code("Number.name").equals("'Number'");
	code("String.name").equals("'String'");
	code("Array.name").equals("'Array'");
	code("Interval.name").equals("'Interval'");
	code("Set.name").equals("'Set'");
	code("Map.name").equals("'Map'");
	code("Object.name").equals("'Object'");
	code("Function.name").equals("'Function'");
	code("Class.name").equals("'Class'");
	code("[Array, ''][0].name").equals("'Array'");

	section("Class.attr");
	code("Number.toto").error(ls::Error::NO_SUCH_ATTRIBUTE, {"toto", "Number"});

	section("Class.operator ==");
	code("Number == 12").equals("false");

	section("Class.operator <");
	code("Number < 12").equals("false");
	code("12 < Number").equals("true");
	code("[Number, 2][0] < 12").equals("false");

	section("is operator");
	code("12 is Number").equals("true");
	code("'yo' is Number").equals("false");
	code("'yo' is String").equals("true");
	code("[1, 2, 3] is Array").equals("true");
	code("[1..12] is Interval").equals("true");
	code("{a: 12} is Object").equals("true");
	code("(-> 12) is Function").equals("true");
	code("true is Boolean").equals("true");
	code("Number is Class").equals("true");

	section("Class creation");
	code("class A {} A").equals("<class A>");
	code("class A {} let a = A() a").equals("A {}");
	code("class A {} let a = new A a").equals("A {}");
	code("class A {} let a = new A() a").equals("A {}");
	code("class A { let b = 2 } let a = new A a").equals("A {b: 2}");
	code("class A { let b = 2 } let a = new A a.class").equals("<class A>");
	code("class A { let b = 2 } let a = new A a.b").equals("2");
	code("class A { let b = 'a' } let t = [A] let a = new t[0] a").equals("A {b: 'a'}");
	code("class A { let b = [1] } let t = [A] let c = t[0] let a = new c a").equals("A {b: [1]}");
	code("class A {} let a = new A let b = new a.class b.class").equals("<class A>");

	section("Methods access");
	code("['hello', 2][0].toto").exception(ls::vm::Exception::NO_SUCH_ATTRIBUTE);
	code("[].map").equals("<function>");
	code("[].azertyuiop").error(ls::Error::NO_SUCH_ATTRIBUTE, {"azertyuiop", "Array"});
	code("[[], ''][0].azertyuiop").exception(ls::vm::Exception::NO_SUCH_ATTRIBUTE);
	code("let my_map = [].map; my_map([1, 2, 3], x -> x ** 2)").equals("[1, 4, 9]");
	code("[].map == [].map").equals("true");
	code("{}.map == {}.map").equals("true");
	code("[].map == {}.map").equals("false");
	code("let a = [].map; a == [].map").equals("true");

	section("Class.operator ==");
	code("Number == 12.class").equals("true");
	code("Number == 'salut'.class").equals("false");

	section("Class.operator <");
	code("Number < String").equals("true");
	code("Number < Array").equals("false");

	section("Class::isTrue()");
	code("Number ? 12 : 5").equals("12");
	code("!(!String)").equals("true");

	section("Class::clone()");
	code("var a = Number a").equals("<class Number>");
}
