#include "Test.hpp"
#include "../src/type/Type.hpp"

void Test::test_loops() {
	auto& env = getEnv();
	/*
	 * Conditions
	 */
	header("Conditions");
	code("if true then 12 else 5 end").equals("12");
	code("if false then 12 else 5 end").equals("5");
	code("if (true) { 12 } else { 5 }").equals("12");
	code("if (false) { 12 } else { 5 }").equals("5");
	code("let a = if (false) { 12 } else { 5 } a").equals("5");
	code("let a = if (true) { 'yo' } else { 'hello' } a").equals("'yo'");
	code("let a = if (true) { 12 } else { 'hello' } a").equals("12");
	code("let a = if (true) { 'hello' } else { 12 } a").equals("'hello'");
	code("if (true) {} else {}").equals("{}");
	code("if (true) {;} else {}").equals("null");
	code("if (true) { {} } else {}").equals("{}");
	code("if (true) null else {}").equals("null");
	code("if true").error(ls::Error::UNEXPECTED_TOKEN, {""});
	code("if true else").error(ls::Error::UNEXPECTED_TOKEN, {"else"});
	code("if (true) {a: 12} else {b: 5}").equals("{a: 12}");
	code("if (true) { {a: 12} } else { {b: 5} }").equals("{a: 12}");
	code("if (true) 12 else 5").equals("12");
	code("if (false) 12 else 5").equals("5");
	code("if (true) 12").equals("12");
	code("if (false) 12").equals("null");
	code("if true then 12 end").equals("12");
	code("if false then 12 end").equals("null");
	code("if true { 5 } else { return 2 }").equals("5");
	code("if true { return 5 } else { 2 }").equals("5");
	code("if false { 5 } else { return 2 }").equals("2");
	code("if false { return 5 } else { 2 }").equals("2");
	// code("let a = 5m if true { a } else { 2m }").equals("5");
	// code("let a = 5m if true { a } else { a }").equals("5");
	code("if true then 1 else 2 end").equals("1");
	code("if true then if false then 1 else 2 end end").equals("2");
	code("if true then if false then 1 end else 2 end").equals("null");
	code("if (false) { return 12 } 5").equals("5");
	code("var k = '121212' if (false) { return 12 } 5").equals("5");
	code("var L = 5 if L < 1 {;}").equals("(void)");
	code("var L = 5 if L > 1 {;}").equals("(void)");
	code("if (false) { return 'hello' }").equals("null");

	section("Conditions with other types");
	code("if 1212 { 'ok' } else { 5 }").equals("'ok'");
	code("if ['str', true][0] { 12 } else { 5 }").equals("12");
	code("if null { 12 } else { 5 }").equals("5");

	section("Different branch types");
	code("if (1) ['a'] else if (0) [2] else [5.5]").equals("['a']");
	code("if (0) ['a'] else if (1) [2] else [5.5]").equals("[2]");
	code("if (0) ['a'] else if (0) [2] else [5.5]").equals("[5.5]");

	section("Ternary conditions");
	code("true ? 5 : 12").equals("5");
	code("false ? 5 : 12").equals("12");
	code("true ? 'a' : 'b'").equals("'a'");
	code("false ? 'a' : 'b'").equals("'b'");
	code("true ? 'a' : 5").equals("'a'");
	code("false ? 'a' : 5").equals("5");
	code("true ? 5 : 'b'").equals("5");
	code("false ? 5 : 'b'").equals("'b'");
	code("'good' ? 5 : 12").equals("5");
	code("'' ? 5 : 12").equals("12");
	code("'good' ? 'a' : 'b'").equals("'a'");
	code("true ? true ? 5 : 12 : 7").equals("5");
	code("true ? false ? 5 : 12 : 7").equals("12");
	code("false ? false ? 5 : 12 : 7").equals("7");
	code("false ? true ? 5 : 12 : 7").equals("7");
	code("true ? true ? true ? 5 : 12 : 7 : 8").equals("5");
	code("true ? true ? false ? 5 : 12 : 7 : 8").equals("12");
	code("true ? false ? false ? 5 : 12 : 7 : 8").equals("7");
	code("(5 > 10) ? 'a' : (4 == 2 ** 2) ? 'yes' : 'no'").equals("'yes'");


	/*
	 * While loops
	 */
	header("While loops");
	code("var i = 0 while (i < 10) { i++ } i").equals("10");
	code("var i = 0 var s = 0 while (i < 10) { s += i i++ } s").equals("45");
	code("var i = 0 while (i < 100) { i++ if (i == 50) break } i").equals("50");
	code("var i = 0 var a = 0 while (i < 10) { i++ if (i < 8) continue a++ } a").equals("3");
	code("while (true) { break }").equals("(void)");
	code("var i = 10 while (['hello', i][1]) { i-- } i").equals("0");
	code("var i = 0 while i < 10 do i++ end i").equals("10");
	code("var i = 5 while (i-- > 0) { System.print(i) }").output("4\n3\n2\n1\n0\n");
	code("while (true) { return 12 }").equals("12");
	code("var n = 5 var a = [] while n-- { a += 1 }").equals("(void)");
	code("var n = 5 var a = [] while n-- { a += 1 } a").equals("[1, 1, 1, 1, 1]");
	code("var mp = 10, grow = [100] while mp-- { grow = [1] }").equals("(void)");
	code("var a = [] while |a += 'a'| < 5 { a += 'b' } a").equals("['a', 'b', 'a', 'b', 'a']");
	code("var s = 0 var j = [] while j.size() < 10 { j += 'a' s++ } s").equals("10");
	code("var s = [] var i = 10 while (i--) { if i < 5 { s += i } else { s += ('a'.code() + i).char() } } s").equals("['j', 'i', 'h', 'g', 'f', 4, 3, 2, 1, 0]");
	code("var s = <> var i = 10 while (i--) { if i < 5 { s += i } else { s += ('a'.code() + i).char() } } s").equals("<0, 1, 2, 3, 4, 'f', 'g', 'h', 'i', 'j'>");
	DISABLED_code("var s = [:] var i = 10 while (i--) { if i < 5 { s[i] = 5.5 } else { s['a'] = i } } s").equals("[:]");
	DISABLED_code("var s = [:] var i = 10 while (i--) { if i < 5 { s[i] = 12 } else { s[1 / i] = 7 } } s").equals("[:]");
	code("var s = [:] var i = 10 while (i--) { if i < 5 { s[i] = 12 } else { s[12] = i } } s").equals("[0: 12, 1: 12, 2: 12, 3: 12, 4: 12, 12: 5]");

	section("Double while loops");
	code("var s = [] var i = 0 while i < 2 { i++ var j = 0 while j < 3 { j++ s += j }} s").equals("[1, 2, 3, 1, 2, 3]");
	code("var s = [] var i = 0 var j = 0 while i < 2 { i++ j = 0 while j < 3 { j++ s += j }} s").equals("[1, 2, 3, 1, 2, 3]");
	code("var s = [] var i = 0 var j = 0 while i < 4 { j = i i++ while j < 4 { j++ s += j }} s").equals("[1, 2, 3, 4, 2, 3, 4, 3, 4, 4]");
	code("var s = [] var i = 0 while i < 2 { i++ var j = 0 while j < 2 { j++ var k = 0 while k < 2 { k++ s += k }}} s").equals("[1, 2, 1, 2, 1, 2, 1, 2]");
	code("var s = [] var i = 0 while i < 2 { i++ s += 0.5 var j = 0 while j < 3 { j++ s += j }} s").equals("[0.5, 1, 2, 3, 0.5, 1, 2, 3]");
	code("var s = [] var i = 0 while i < 2 { i++ s.push([]) var j = 0 while j < 3 { j++ s[|s| - 1] += 1 }} s").equals("[[1, 1, 1], [1, 1, 1]]");
	code("var s = [] var i = 0 while i < 2 { i++ s.push([]) var j = 0 while j < 3 { j++ s[|s| - 1] += ('a'.code() + 3 * (i - 1) + j - 1).char() }} s").equals("[['a', 'b', 'c'], ['d', 'e', 'f']]");
	file("test/code/loops/lot_of_whiles_int.leek").equals("30030");
	file("test/code/loops/lot_of_whiles_array.leek").equals("30030");

	/*
	 * For loops
	 */
	header("For loops");
	code("for var i = 0; ; i++ {}").ops_limit(1000).exception(ls::vm::Exception::OPERATION_LIMIT_EXCEEDED);
	code("for var i = 0; false; i++ {}").equals("(void)");
	code("for var i = 0; i < 10; i++ {}").equals("(void)");
	code("var s = 0 for var i = 0; i < 5; i++ do s += i end s").equals("10");
	code("var s = 0 for var i = 0; i < 10; i += 2 do s += i end s").equals("20");
	code("var a = 0 for var i = 0; i < 10; i++ { a++ } a").equals("10");
	code("var a = 0 for var i = 0; i < 10; i++ { if i < 5 { continue } a++ } a").equals("5");
	code("var a = 0 for var i = 0; i < 10; i++ { if i > 5 { break } a++ } a").equals("6");
	code("var c = 0 for var t = []; t.size() < 10; t.push('x') { c++ } c").equals("10");
	// code("var s = 0 for var m = [1: 3, 2: 2, 3: 1]; m; var l = 0 for k, x in m { l = k } m.erase(l) { for x in m { s += x } } s").equals("14");
	code("for var i = 0; ['', i < 10][1]; i++ {}").equals("(void)");
	code("var i = ['', 1][1] for ; i < 10; i <<= 1 {}").equals("(void)");
	code("for (var i = 0, j = 0; i < 5; i++, j++) { System.print(i + ', ' + j) }").output("0, 0\n1, 1\n2, 2\n3, 3\n4, 4\n");
	code("for (var i = 0, j = 10; i < 5; i++, j += 2) { System.print(i + ', ' + j) }").output("0, 10\n1, 12\n2, 14\n3, 16\n4, 18\n");
	code("for (var i = 0, j = 1, k = 2, l = 3; i < 5; i++, j++, k++, l++) { System.print([i j k l]) }").output("[0, 1, 2, 3]\n[1, 2, 3, 4]\n[2, 3, 4, 5]\n[3, 4, 5, 6]\n[4, 5, 6, 7]\n");
	code("for var i = 0m; i < 10m; i++ {}").equals("(void)");
	code("var s = 0m for var i = 0m; i < 10m; i++ { s += i } s").equals("45");
	code("var s = 0m for var i = 0m; i < 10m; i += 2m { s += i } s").equals("20");

	section("For variable defined before the loop");
	code("var i = 0 for ; i < 10; i++ { } i").equals("10");
	code("var i = 0 for i = 0; i < 10; i++ { } i").equals("10");
	code("var i = 0 for i = 0; i < 10; i++ { if i == 5 { break } } i").equals("5");
	code("var i var c = 0 for i = 0; i < 20; i += 0.573 { c++ } i").equals("20.055");
	code("var i = 's' var c = 0 for i = []; i.size() < 8; i += 1 { c++ } i").equals("[1, 1, 1, 1, 1, 1, 1, 1]");
	code("var i = 0 for ; i < 10; i += 0.5 { } i").equals("10");
	code("var i = 0 for i = 2l; i < 10; i += 0.5 { } i").equals("10");

	section("For whitout braces");
	code("var s = 0 for (var i = 0; i < 10; i++) s += i s").equals("45");

	section("For loops with returns");
	code("for return 12; true; null {}").equals("12");
	code("for ;; return 'hello' {}").equals("'hello'");

	section("Nested for loops");
	code("var s = 0 for var i = 0; i < 10; ++i { for var j = 0; j < 10; ++j { s++ }} s").equals("100");
	code("var s = 0 for var i = 0; i < 5; ++i { for var j = 0; j < 5; ++j { for var k = 0; k < 5; ++k { s++ }}} s").equals("125");
	code("var s = 0 for var i = 0; i < 10; i += 1 { for var j = 0; j < 10; j += 1 { s++ }} s").equals("100");
	code("var s = 0 for var i = 0; i < 10; i += 1 { var j = 0 for ; j < 10; j += 1 { s++ }} s").equals("100");
	file("test/code/loops/lot_of_fors_int.leek").equals("15015");
	file("test/code/loops/lot_of_fors_array.leek").equals("15015");

	section("Mix for and while loops");
	code("var s = 0 for var i = 0; i < 10; i += 1 { var j = 10 while (j--) { s++ }} s").equals("100");
	code("var s = [] for var i = 0; i < 2; i += 1 { var j = 0 while j < 3 { j++ s += 1 }} s").equals("[1, 1, 1, 1, 1, 1]");
	code("var s = 0 for var i = 0; i < 10; i += 1 { var j = [] while j.size() < 10 { j += 'a' s++ }} s").equals("100");
	code("var s = [] var i = 3 while (i--) { for var j = 0; j < 2; ++j { s += j }} s").equals("[0, 1, 0, 1, 0, 1]");
	DISABLED_code("var s = [] var i = 3 var j while (i--) { for j = 0; j < 2; ++j { s += j }} s").equals("[0, 1, 0, 1, 0, 1]");


	/*
	 * Foreach loops
	 */
	header("Foreach loops");
	code("for v in [] {}").equals("(void)");
	code("for v in new Array {}").equals("(void)");
	code("for v in [1, 2, 3, 4] {}").equals("(void)");
	code("for (v in [1, 2, 3, 4]) {}").equals("(void)");
	code("for (v in [1, 2, 3, 4]) do end").equals("(void)");
	code("var s = 0 for v in [1, 2, 3, 4] { s += v } s").equals("10");
	code("var s = 0 for v in [1l, 2l, 3l, 4l] { s += v } s").equals("10");
	code("var s = 0.0 for v in [1.2, 2, 3.76, 4.01] { s += v } s").almost(10.97);
	code("var s = 0 for v in [1.2, 2, 3.76, 4.01] { s += v } s").almost(10.97);
	code("var s = '' for v in ['salut ', 'ça ', 'va ?'] { s += v } s").equals("'salut ça va ?'");
	code("var a = 0 let x = [0, 1, 2, 3, 4, 5, 6, 7, 8, 9] for i in x { if i < 5 { continue } a++ } a").equals("5");
	code("var s = 0 for k : v in [1, 2, 3, 4] { s += k * v } s").equals("20");
	code("var s = '' for k : v in ['a': 1, 'b': 2, 'c': 3, 'd': 4] { s += v * k } s").equals("'abbcccdddd'");
	// code("(a -> { var s = 0.0; for x in a { s += x } s })([1, 2, 3, 4.25])").equals("10.25");
	// code("var y = '' for k, x in { var x = [] x.push(4) x } { y += k + ':' + x + ' ' } y").equals("'0:4 '");
	// code("var y = '' for k, x in { var x = [1: 2] x.insert(3, 4) x } { y += k + ':' + x + ' ' } y").equals("'1:2 3:4 '");
	// code("var y = '' for k, x in { var x = [1: 2.5] x.insert(3, 4) x } { y += k + ':' + x + ' ' } y").equals("'1:2.5 3:4 '");
	// code("var y = '' for k, x in { var x = [1: '2'] x.insert(3, 4) x } { y += k + ':' + x + ' ' } y").equals("'1:2 3:4 '");
	code("var y = 'test' for x in 1 { y = x } y").equals("1");
	code("var y = 'test' for x in 'salut' { y = x } y").equals("'t'");
	code("var x = 'test' for x in [1] {} x").equals("'test'");
	code("var y = '' for k, x in { var x = <> x.insert(4) x } { y += k + ':' + x } y").equals("'0:4'");
	DISABLED_code("var fs = [] fs.push(s -> {var sum = 0 for v in s {sum += v} sum}) fs[0](<1, 2>)").equals("3"); // TODO issue #243
	DISABLED_code("var fs = [] fs.push(s -> {[for v in s {v}]}) fs[0](<2,1>)").equals("[1, 2]"); // TODO issue #243
	code("var s = 0l for i in [0..1000] { s += i ** 2 } s").equals("333833500");

	header("Foreach - unknown container");
	// TODO : unknown container iterator
	// code("for x in ['hello', 12345][0] { print(x) }").equals("h\ne\nl\nl\no\n");

	header("Foreach - not iterable");
	code("for x in null {}").error(ls::Error::Type::VALUE_NOT_ITERABLE, {"null", env.null->to_string()});
	code("for x in true {}").error(ls::Error::Type::VALUE_NOT_ITERABLE, {"true", env.boolean->to_string()});
	code("for x in Number {}").error(ls::Error::Type::VALUE_NOT_ITERABLE, {"Number", env.const_class()->to_string()});

	/*
	 * Array For
	 */
	header("Array For");
	code("[for var i = 0; i < 5; ++i { i }]").equals("[0, 1, 2, 3, 4]");
	code("[for var i = 1; i <= 10; ++i { [for var j = 1; j <= 3; ++j { if i == 3 break 2 i * j}] }]").equals("[[1, 2, 3], [2, 4, 6]]");
	code("[for x in [1, 2, 3] { x }]").equals("[1, 2, 3]");
	DISABLED_code("let a = ['a': 'b', 'c': 'd'] [for k, x in a { k + x }]").equals("['ab', 'cd']");
	DISABLED_code("[for x in [1, 2, 3] {[ for y in [1, 2, 3] { if y == 2 continue x * y }] }]").equals("[[1, 3], [2, 6], [3, 9]]");
	DISABLED_code("let sorted = [for x in <5, 2, 4, 1, 3> { x }] sorted").equals("[1, 2, 3, 4, 5]");
	code("[for i in [1..10] { i }]").equals("[1, 2, 3, 4, 5, 6, 7, 8, 9, 10]");
	code("function attrs(o) { [for k : v in o {v}] } attrs(['a'])").equals("['a']");
	code("function attrs(o) { [for k : v in o {v}] } attrs([1])").equals("[1]");
	code("function attrs(o) { [for k : v in o {v}] } attrs([])").equals("[]");

	/*
	 * Break & continue
	 */
	header("Breaks and Continues");
	DISABLED_code("break").error(ls::Error::Type::BREAK_MUST_BE_IN_LOOP, {});
	DISABLED_code("continue").error(ls::Error::Type::CONTINUE_MUST_BE_IN_LOOP, {});
	code("while (true) { break 2 }").error(ls::Error::Type::BREAK_MUST_BE_IN_LOOP, {});
	code("while (true) { continue 2 }").error(ls::Error::Type::CONTINUE_MUST_BE_IN_LOOP, {});
	DISABLED_code("var r = 0 for x in [1, 2] { for y in [3, 4] { r = 10 * x + y if x + y >= 5 break 2 }} r").equals("14");
	DISABLED_code("var r = 0 for x in [1, 2] { for y in [3, 4] { r = 10 * x + y continue 2 } r = 0 } r").equals("23");
	code("for x in ['a'] { let a = 'a' { let b = 'b' break let c = 'c' } let d = 'd' } 0").equals("0");
	code("for x in ['a'] { let a = 'a' for y in ['a'] { let b = 'b' break let c = 'c' } let d = 'd' } 0").equals("0");
	code("for x in ['a'] { let a = 'a' for y in ['a'] { let b = 'b' break 2 let c = 'c' } let d = 'd' } 0").equals("0");
	code("for var x = 0; x < 2; ++x { let a = 'a' { let b = 'b' break let c = 'c' } let d = 'd' } 0").equals("0");
	code("for var x = 0; x < 2; ++x { let a = 'a' for var y = 0; y < 2; ++y { let b = 'b' break let c = 'c' } let d = 'd' } 0").equals("0");
	code("for var x = 0; x < 2; ++x { let a = 'a' for var y = 0; y < 2; ++y { let b = 'b' break 2 let c = 'c' } let d = 'd' } 0").equals("0");
	code("while (true) { break 0 }").error(ls::Error::Type::BREAK_LEVEL_ZERO, {});
	code("while (true) { continue 0 }").error(ls::Error::Type::CONTINUE_LEVEL_ZERO, {});

	/*
	 * Match
	 */
	header("Match");
	/*
	code("match 50 {}").equals("null");
	code("match 50 { ..: 4 }").equals("4");
	code("match 3 { 1 : 1 2 : 2 3 : 3 }").equals("3");
	code("match 3 { 1 : 1 2 : 2 .. : 3 }").equals("3");
	code("match 'a' { 'a' : 1 'b' : 2 .. : 3 }").equals("1");
	code("match 4 { 1 : 1 2 : 2 3 : 3 }").equals("null");
	code("match 'a' { 1 : 1 'a' : 'a' }").equals("'a'");
	code("match 1 { 1 : 1 'a' : 'a' }").equals("1");
	code("match 1 { 1|2 : 1 'a'|[]|{} : 'a' }").equals("1");
	code("match 1 { (4 - 2)|2| |-1| : 1 'a'|[]|{} : 'a' }").equals("1");
	code("match 1 + 5 { (4 + 1)|2|8 : 1 'a'|[]|{} : 'a' }").equals("null");
	code("match 50 { 0..50: 1 50..100: 2 }").equals("2");
	code("match 50 { ..50: 1 50..: 2 }").equals("2");
	code("match 50 { ..10: 1 ..100: 2 }").equals("2");
	code("match 50 { ..100: 1 ..100: 2 }").equals("1");
	code("match 50 { 100..0: 4 }").equals("null");
	code("let e = 'e' match e { ..'b': 1 ..'z': 2 }").equals("2");
	code("match 'e' { 'z'..: 1 'b'..: 2 }").equals("2");
	code("match [1] { ..[]: 1 ..[2, 2]: 2 }").equals("2");
	code("match [1] { [2, 2]..: 1 []..: 2 }").equals("2");
	code("let b = 'b' match 'e' { ..b: 1 1..6|0..9: 2 ..|..: 3}").equals("3");
	code("let a = match 3 { 1 : 1 2 : 2 3 : 3 } a").equals("3");
	code("match 2 { 1 : 1 2 : {} 3 : 3 }").equals("{}");
	*/
}
