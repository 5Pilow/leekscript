#include "Test.hpp"

void Test::test_loops() {

	/*
	 * Conditions
	 */
	header("Conditions");
	success("if true then 12 else 5 end", "12");
	success("if false then 12 else 5 end", "5");
	success("if (true) { 12 } else { 5 }", "12");
	success("if (false) { 12 } else { 5 }", "5");
	success("let a = if (false) { 12 } else { 5 } a", "5");
//	success("let a = if (true) { 'yo' } else { 'hello' } a", "'yo'");
	success("let a = if (true) { 12 } else { 'hello' } a", "12");
//	success("let a = if (true) { 'hello' } else { 12 } a", "'hello'");
	success("if (true) {} else {}", "{}");
	success("if (true) {;} else {}", "null");
	success("if (true) { {} } else {}", "{}");
	success("if (true) null else {}", "null");
//	success("if true", "null");
//	success("if true else", "null"); crash
	success("if (true) {a: 12} else {b: 5}", "{a: 12}");
	success("if (true) { {a: 12} } else { {b: 5} }", "{a: 12}");
	success("if (true) 12 else 5", "12");
	success("if (false) 12 else 5", "5");
	success("if (true) 12", "12");
	success("if (false) 12", "null");

	/*
	 * While loops
	 */
	header("While loops");
	success("let i = 0 while (i < 10) { i++ } i", "10");
	success("let i = 0 let s = 0 while (i < 10) { s += i i++ } s", "45");
	success("let i = 0 while (i < 100) { i++ if (i == 50) break } i", "50");
	success("let i = 0 let a = 0 while (i < 10) { i++ if (i < 8) continue a++ } a", "3");
	success("while (true) { break }", "null");

	/*
	 * For loops
	 */
	header("For loops");
	success("for let i = 0; ; i++ {}", "null");
	success("for let i = 0; i < 10; i++ {}", "null");
	success("let s = 0 for let i = 0; i < 5; i++ do s += i end s", "10");
	success("let s = 0 for let i = 0; i < 10; i += 2 do s += i end s", "20");
	success("let i = 0 for i = 0; i < 10; i++ { } i", "10");
	success("let i = 0 for i = 0; i < 10; i++ { if i == 5 { break } } i", "5");
	success("let a = 0 for let i = 0; i < 10; i++ { a++ } a", "10");
	success("let a = 0 for let i = 0; i < 10; i++ { if i < 5 { continue } a++ } a", "5");
//	 success("let c = 0 for var t = []; t.size() < 10; t += 'x' { c++ } c", "10"),

	/*
	 * Foreach loops
	 */
	header("Foreach loops");
	success("for v in [1, 2, 3, 4] {}", "null");
	success("let s = 0 for v in [1, 2, 3, 4] { s += v } s", "10");
	success_almost("let s = 0 for v in [1.2, 2, 3.76, 4.01] { s += v } s", 10.969999999999998863);
	success("let s = '' for v in ['salut ', 'ça ', 'va ?'] { s += v } s", "'salut ça va ?'");
	success("let a = 0 let x = [0, 1, 2, 3, 4, 5, 6, 7, 8, 9] for i in x { if i < 5 { continue } a++ } a", "5");
	success("let s = 0 for k : v in [1, 2, 3, 4] { s += k * v } s", "20");
	success("let s = '' for k : v in map['a':1, 'b':2, 'c':3, 'd':4] { s += v * k } s", "'abbcccdddd'");
	success("(a->{ let s = 0; for x in a { s+=x } s })([1,2,3,4.25])", "10.25");
	success("let y = '' for k, x in { let x = [] x.push(4) x } { y += k + ':' + x + ' ' } y", "'0:4 '");
	success("let y = '' for k, x in { let x = map[1:2] x.insert(3,4) x } { y += k + ':' + x + ' ' } y", "'1:2 3:4 '");
	success("let y = '' for k, x in { let x = map[1:2.5] x.insert(3,4) x } { y += k + ':' + x + ' ' } y", "'1:2.5 3:4 '");
	success("let y = '' for k, x in { let x = map[1:'2'] x.insert(3,4) x } { y += k + ':' + x + ' ' } y", "'1:2 3:4 '");
	success("let y = 'test' for x in 1 { y = x } y", "'test'");
	success("let y = 'test' for x in 'salut' { y = x } y", "'test'");
	success("let x = 'test' for x in [1] {} x", "'test'");

	/*
	 * Break & continue
	 */
	sem_err("break", ls::SemanticException::Type::BREAK_MUST_BE_IN_LOOP, "");
	sem_err("continue", ls::SemanticException::Type::CONTINUE_MUST_BE_IN_LOOP, "");
	sem_err("while (true) { x -> {x break} }", ls::SemanticException::Type::BREAK_MUST_BE_IN_LOOP, "");
	sem_err("while (true) { x -> {x continue} }", ls::SemanticException::Type::CONTINUE_MUST_BE_IN_LOOP, "");

	/*
	 * Match
	 */
	header("Match");
	success("match 3 { 1 : 1 2 : 2 3 : 3 }", "3");
	success("match 3 { 1 : 1 2 : 2 .. : 3 }", "3");
	success("match 'a' { 'a' : 1 'b' : 2 .. : 3 }", "1");
	success("match 4 { 1 : 1 2 : 2 3 : 3 }", "null");
	success("match 'a' { 1 : 1 'a' : 'a' }", "'a'");
	success("match 1 { 1 : 1 'a' : 'a' }", "1");
	success("match 1 { 1|2 : 1 'a'|[]|{} : 'a' }", "1");
	success("match 1 { (4-2)|2| |-1| : 1 'a'|[]|{} : 'a' }", "1");
	success("match 1+5 { (4+1)|2|8 : 1 'a'|[]|{} : 'a' }", "null");
	success("match 50 { 0..50: 1 50..100: 2 }", "2");
	success("match 50 { ..50: 1 50..: 2 }", "2");
	success("match 50 { ..10: 1 ..100: 2 }", "2");
	success("match 50 { ..100: 1 ..100: 2 }", "1");
	success("match 'e' { ..'b': 1 ..'z': 2 }", "2");
	success("match 'e' { 'z'..: 1 'b'..: 2 }", "2");
	success("match [1] { ..[]: 1 ..[2, 2]: 2 }", "2");
	success("match [1] { [2, 2]..: 1 []..: 2 }", "2");
	success("match 'e' { ..'b': 1 1..6|0..9: 2 ..|..: 3}", "3");
}
