#include <string>
#include <iostream>
#include <sstream>
#include "Test.hpp"
#include "../src/analyzer/Context.hpp"
#include "../src/analyzer/lexical/LexicalAnalyzer.hpp"
#include "../src/analyzer/syntaxic/SyntaxicAnalyzer.hpp"
#include "../src/analyzer/semantic/SemanticAnalyzer.hpp"
#include "../src/analyzer/error/Error.hpp"
#include "../src/analyzer/Program.hpp"
#include "../src/vm/value/LSNumber.hpp"
#include "../src/vm/value/LSObject.hpp"
#include "../src/standard/StandardLibrary.hpp"

void Test::test_general() {
	auto& env = getEnv(false);

	header("General");
	// Print a syntaxical error
	auto error = new ls::Error(ls::Error::Type::NO_SUCH_ATTRIBUTE, {nullptr, {0, 0, 0}, {0, 0, 0}}, {nullptr, {0, 0, 0}, {0, 0, 0}}, {"key", "obj_fake"});
	std::cout << error->message() << std::endl;
	delete error;
	// Print a program
	auto program = new ls::Program { env, "2 + 2", "test" };
	env.analyze(*program);
	env.compile(*program);
	std::cout << program << std::endl;
	delete program;
	// LSObject getField
	ls::LSObject o;
	o.addField("test", ls::LSNumber::get(12));
	std::cout << o.getField("test") << std::endl;

	header("Basic codes");
	code("").equals("(void)");
	code(" ").equals("(void)"); // classic space
	code(" ").equals("(void)"); // unbreakable space
	code("	").equals("(void)"); // tab
	code("null").equals("null");
	code("()").error(ls::Error::Type::EXPECTED_VALUE, {")"});
	code("12").equals("12");
	code("1212m").equals("1212");
	code("true").equals("true");
	code("false").equals("false");
	code("'toto'").equals("'toto'");
	code("[]").equals("[]");
	code("{}").equals("{}");
	code("{a: 12}").equals("{a: 12}");
	code("{;}").equals("(void)");
	code("return 12").equals("12");
	code("return").equals("(void)");
	code("'a' 'b' 'c'").equals("'c'");

	section("Parenthesis");
	code("(5)").equals("5");
	code("(((((5)))))").equals("5");
	code("(5) * (4 + 2) - 7").equals("23");
	code("15 / 3").equals("5");
	code("(5) * (4 + 2) - 7 / (4 - (((5))))").equals("37");

	section("Line breaks");
	code("5 \n + 5").equals("10");
	code("5 \n - 5").equals("-5");

	header("Variables");
	code("let a").equals("(void)");
	code("let a = 2").equals("(void)");
	code("let a = 2 a").equals("2");
	code("let a = 1, b = 2, c = 3").equals("(void)");
	code("let a = 1, b = 2, c").equals("(void)");
	code("let a = 1, b, c = 3").equals("(void)");
	code("let a, b, c = 3").equals("(void)");
	code("let a, b, c").equals("(void)");
	code("let a a").equals("null");
	code("var a a = 12 a").equals("12");
	code("var a = 5 a = 13 a").equals("13");
	code("var a = 1 let b = (a = 12) b").equals("12");
	code("let s = 'hello'").equals("(void)");
	code("let s = 'hello' s").equals("'hello'");
	code("let état = 12 état").equals("12");
	code("let 韭 = 'leek' 韭").equals("'leek'");
	code("let ♫☯🐖👽 = 5 let 🐨 = 2 ♫☯🐖👽 ** 🐨").equals("25");
	code("var a = 2 [a = 10]").equals("[10]");
	code("var a = 2 ['a', a = 10]").equals("['a', 10]");

	section("Global variables");
	code("global a = 2").equals("(void)");
	code("global a = 2, b = 'a'").equals("(void)");

	section("Variable already defined");
	code("a").error(ls::Error::Type::UNDEFINED_VARIABLE, {"a"});
	code("let a = 2 let a = 5").error(ls::Error::Type::VARIABLE_ALREADY_DEFINED, {"a"});
	code("let Number = 2").error(ls::Error::Type::VARIABLE_ALREADY_DEFINED, {"Number"});

	section("Sub-blocks");
	code("let a = 12 a").equals("12");
	code("let a = 12 { let a = 5 } a").equals("12");
	code("let a = 12 var b = 0 { let a = 5 b = a } b").equals("5");
	code("{let a = 5} a").error(ls::Error::Type::UNDEFINED_VARIABLE, {"a"});
	code("{ var x = [] x.push(4) x } x").error(ls::Error::Type::UNDEFINED_VARIABLE, {"x"});

	section("Syntaxic errors");
	code("{").error(ls::Error::Type::BLOCK_NOT_CLOSED, {});
	code("2 + ()").error(ls::Error::Type::EXPECTED_VALUE, {")"});
	code("let 2 = 5").error(ls::Error::Type::UNEXPECTED_TOKEN, {"2"});
	code("(654321").error(ls::Error::Type::UNEXPECTED_TOKEN, {""});
	code("fun(1234").error(ls::Error::Type::UNEXPECTED_TOKEN, {""});

	section("Syntaxic edge cases");
	code("let toto = 2; toto'salut'").equals("'salut'"); // string just after a ident
	code("let toto = 2; toto\"salut\"").equals("'salut'"); // string just after a ident
	code("12345'salut'").equals("'salut'"); // string just after a number
	code("12345\"salut\"").equals("'salut'"); // string just after a number
	code("|2 - 3|| 2").equals("3"); // <=> |2 - 3| | 2
	code("|2 - 3| | 2").equals("3");
	code("|5 - 3 or 2|").equals("1");
	code("5 - 3 or 2").equals("true");
	code("||2 - 5| - 10|").equals("7");
	code("|||2 - 5| - 10| - 9|").equals("2");

	section("?? operator");
	code("'foo' ?? 'bar'").equals("'foo'");
	code("null ?? 'bar'").equals("'bar'");
	code("let a = 'foo' a ?? 'bar'").equals("'foo'");
	code("let a = null a ?? 'bar'").equals("'bar'");
	code("[] ?? [12]").equals("[]");
	code("null ?? [12]").equals("[12]");
	code("{} ?? 'default'").equals("{}");
	code("null ?? 'default'").equals("'default'");
	code("let a = null let b = null a ?? b ?? ':)'").equals("':)'");

	section("Value.string()");
	code("Value.unknown.string().size() <= 3").equals("true");
	// integer
	code("0.string()").equals("'0'");
	code("12.string()").equals("'12'");
	code("(-7).string()").equals("'-7'");
	// long
	code("12434324223112123.string()").equals("'12434324223112123'");
	code("(-1243439967898452).string()").equals("'-1243439967898452'");
	// real
	code("12.5.string()").equals("'12.5'");
	code("(-6546.34).string()").equals("'-6546.34'");
	code("Number.pi.string()").equals("'3.1415926536'");
	// boolean
	code("true.string()").equals("'true'");
	code("false.string()").equals("'false'");
	code("(12 > 5).string()").equals("'true'");

	section("Const values");
	code("var c = 2 c++").equals("2");
	code("let c = 2 c++").error(ls::Error::Type::CANT_MODIFY_CONSTANT_VALUE, {"c"});
	code("let c = 2; ++c").error(ls::Error::Type::CANT_MODIFY_CONSTANT_VALUE, {"c"});
	code("let c = 2; c = 5").error(ls::Error::Type::CANT_MODIFY_CONSTANT_VALUE, {"c"});
	code("let c = 2; c += 5").error(ls::Error::Type::CANT_MODIFY_CONSTANT_VALUE, {"c"});

	section("Type changes");
	code("var a a = 12").equals("12");
	code("var a a = 12 a").equals("12");
	code("var a a = 12.5").equals("12.5");
	code("var a a = 12.5 a").equals("12.5");
	code("var a a = 'a'").equals("'a'");
	code("var a a = 'a' a").equals("'a'");
	code("var a a = 12m").equals("12");
	code("var a a = 12m a").equals("12");
	code("var a = 2 a = 'hello'").equals("'hello'");
	code("var a = 'hello' a = 2").equals("2");
	code("var a = 2 a = 'hello' a").equals("'hello'");
	code("var a = 2 a = [1, 2] a").equals("[1, 2]");
	code("var a = 5.5 a = {} a").equals("{}");
	code("var a = [5, 7] a = 7 System.print(a)").output("7\n");
	code("var a = 7 a = [5, 12] a").equals("[5, 12]");
	code("var a = 7 System.print(a) a = <5, 12> System.print(a)").output("7\n<5, 12>\n");
	code("var a = 5 a = 200l").equals("200");
	code("var a = 5 a = 200l a").equals("200");
	code("var a = 200l a = 5").equals("5");
	code("var a = 200l a = 5 a").equals("5");
	code("var a = 5.5 a = 200l a").equals("200");
	code("var a = 5.5 a = 2").equals("2");
	code("var a = 5.5 a = 1000m").equals("1000");
	code("var a = 5.5 a = 2m ** 100").equals("1267650600228229401496703205376");
	code("var a = 2m a = 5").equals("5");
	code("var a = 5.5 System.print(a) a = 2 System.print(a) a = 200l System.print(a) a = 1000m System.print(a) a = 'hello' System.print(a)").output("5.5\n2\n200\n1000\nhello\n");
	code("var a = [] a = 5m").equals("5");

	section("Value.copy()");
	code("2.copy()").equals("2");
	code("2.5.copy()").equals("2.5");
	code("12l.copy()").equals("12");
	code("100m").equals("100");
	code("'abc'.copy()").equals("'abc'");
	code("[].copy()").equals("[]");
	code("[1, 2, 3].copy()").equals("[1, 2, 3]");
	code("[1.5, 2.5, 3.5].copy()").equals("[1.5, 2.5, 3.5]");
	code("[1..1000].copy()").equals("[1..1000]");
	code("[:].copy()").equals("[:]");
	code("{}.copy()").equals("{}");
	code("(x -> x).copy()").equals("<function>");
	code("Number.copy()").equals("<class Number>");

	section("Assignments");
	code("var b = 0 { b = 12 } b").equals("12");
	code("var i = 12 { i = 'salut' } i").equals("'salut'");
	code("var b = 5 if 1 { b = 'salut' } b").equals("'salut'");
	code("var b = 5 if 0 { b = 'salut' } b").equals("5");
	code("var a = 12 if 1 { a = 5 a++ } else { a = 3 } a").equals("6");
	code("var a = 12 if 0 { a = 5 a++ } else { a = 5.5 } a").equals("5.5");
	code("var a = 12 if 0 { a = 5 a++ } else { a = 7l } a").equals("7");
	code("var b = 5 if 1 {} else { b = 'salut' } b").equals("5");
	code("var b = 5 if 0 {} else { b = 'salut' } b").equals("'salut'");
	code("var x = 5 if true if true x = 'a' x").equals("'a'");
	code("var x = 5 if true if true if true if true if true x = 'a' x").equals("'a'");

	section("Assignments with +=");
	code("var a = 10 a += 0.5 a").equals("10.5");
}
