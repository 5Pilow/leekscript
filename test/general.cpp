#include <string>
#include <iostream>
#include <sstream>

#include "../src/vm/Context.hpp"
#include "../src/compiler/lexical/LexicalAnalyser.hpp"
#include "../src/compiler/syntaxic/SyntaxicAnalyser.hpp"
#include "../src/compiler/semantic/SemanticAnalyser.hpp"
#include "../src/compiler/semantic/SemanticException.hpp"
#include "Test.hpp"

using namespace std;

void Test::test_general() {

	header("General");
	success("", "null");
	success(" ", "null"); // classic space
	success(" ", "null"); // unbreakable space
	success("	", "null"); // tab
	success("null", "null");
	success("()", "null");
	success("12", "12");
	success("true", "true");
	success("false", "false");
	success("'toto'", "'toto'");
	success("[]", "array[]");
	success("{}", "{}");
	success("{a: 12}", "{a: 12}");
	success("{;}", "null");
	success("return 12", "12");
	success("return", "null");

	header("Variables");
	success("let a = 2 a", "2");
	success("let a, b, c = 1, 2, 3 c", "3");
	success("let a", "null");
	success("let a a", "null");
	//success("let a a = 12 a", "12");
	success("let a = 1 let b = (a = 12) b", "12");
	success("let s = 'hello'", "null");
	success("let s = 'hello' s", "'hello'");
	success("let état = 12 état", "12");
	success("let 韭 = 'leek' 韭", "'leek'");
	success("let ♫☯🐖👽 = 5 let 🐨 = 2 ♫☯🐖👽 ** 🐨", "25");

	sem_err("a", ls::SemanticException::Type::UNDEFINED_VARIABLE, "a");
	sem_err("let a = 2 let a = 5", ls::SemanticException::Type::VARIABLE_ALREADY_DEFINED, "a");

	success("let a = 12 a", "12");
	success("let a = 12 { let a = 5 } a", "12");
	success("let a = 12 let b = 0 { let a = 5 b = a } b", "5");
	sem_err("{let a = 5} a", ls::SemanticException::Type::UNDEFINED_VARIABLE, "a");

	success("'foo' ?? 'bar'", "'foo'");
	success("null ?? 'bar'", "'bar'");
	success("let a = 'foo' a ?? 'bar'", "'foo'");
	success("let a = null a ?? 'bar'", "'bar'");
	success("[] ?? [12]", "array[]");
	success("null ?? [12]", "array[12]");
	success("{} ?? 'default'", "{}");
	success("null ?? 'default'", "'default'");
	success("let a = null let b = null a ?? b ?? ':)'", "':)'");
}


