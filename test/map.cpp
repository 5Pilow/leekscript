#include "Test.hpp"
#include "../src/type/Type.hpp"

void Test::test_map() {
	auto& env = getEnv();

	header("Map");

	section("Constructor");
	code("[:]").equals("[:]");
	code("[1: 1, 2: 2]").equals("[1: 1, 2: 2]");
	code("[1: 1, 2: '2']").equals("[1: 1, 2: '2']");
	code("['1': '1', '1': '2', '1': '3']").equals("['1': '1']");

	section("Map::to_bool()");
	code("![:]").equals("true");
	code("![2: 2]").equals("false");
	code("if ([2: 2]) { 12 } else { 5 }").equals("12");

	section("Array of maps");
	code("[[:], [1: 1], [1: 2]]").equals("[[:], [1: 1], [1: 2]]");
	code("let m = ['a': 'b'] [m]").equals("[['a': 'b']]");

	/*
	 * Operators
	 */
	std::vector<std::string> maps = {"[5: 5]", "[5: 9.99]", "[5: 'abc']", "[9.99: 5]", "[9.99: 9.99]", "[9.99: 'abc']", "['abc': 5]", "['abc': 9.99]", "['abc': 'abc']"};

	section("Map.operator ==");
	code("['a': 'b'] == [1: 1]").equals("false");
	code("['a': 'b'] == ['a': 'b']").equals("true");
	code("['a': 'b'] == ['a': 'b', 'c': 'd']").equals("false");
	code("var x = ['a' : 'b'] var y = [1 : 1] x.clear() == y.clear()").equals("true");
	for (auto& m1 : maps)
		for (auto& m2 : maps)
			code(m1 + " == " + m2).equals(m1 == m2 ? "true" : "false");
	code("[12.5: 9.99] == 'hello'").equals("false");

	section("Map.operator <");
	code("['a': 1, 'b': 2] < ['a': 1, 'b': 3]").equals("true");
	code("[1: 1, 2: 0] < [1: 1, 2: true]").equals("false");
	code("[1: 1, 2: 0.5] < [1: 1, 2: true]").equals("false");
	code("[1: 1, 2: 'a'] < [1: 1.5, 2: 5.5]").equals("true");
	for (size_t i = 0; i < maps.size(); ++i)
		for (size_t j = 0; j < maps.size(); ++j)
			code(maps[i] + " < " + maps[j]).equals(i < j ? "true" : "false");

	section("Map.operator in");
	code("let m = ['salut': 12] 'salut' in m").equals("true");
	code("let m = ['salut': 12] 'salum' in m").equals("false");
	code("let m = ['salut': 12] 12 in m.values()").equals("true");

	section("Map.operator []");
	code("let m = [1: 1] m[1]").equals("1");
	code("let m = ['a': 'b'] m['a']").equals("'b'");
	code("let m = [5: 12] m[5]").equals("12");
	code("let m = [5: 12.5] m[5]").equals("12.5");
	code("let m = [5.5: 12] m[5.5]").equals("12");
	code("let m = [5.5: 12.5] m[5.5]").equals("12.5");
	code("let m = ['salut': 12] m['salut']").equals("12");
	code("let m = ['salut': 12.5] m['salut']").equals("12.5");
	code("let m = ['salut': 'yolo'] m['salut']").equals("'yolo'");
	code("let m = ['a': 'b'] m['a'] = 'c' m").equals("['a': 'c']");
	code("let m = ['salut': 12] m['salut'] = 13 m['salut']").equals("13");
	code("let m = ['salut': 'yo'] m['salut'] = 'ya' m['salut']").equals("'ya'");
	code("let m = [5: 12] m[5.7]").equals("12");
	code("let m = [5: 12] m['salut']").error(ls::Error::INVALID_MAP_KEY, {"'salut'", "m", env.tmp_string->to_string()});
	code("let m = [5.7: 'hello'] m['salut']").error(ls::Error::INVALID_MAP_KEY, {"'salut'", "m", env.tmp_string->to_string()});
	code("var m = [1: 'a', 2: 'b'] m[2] = 'B' m").equals("[1: 'a', 2: 'B']");
	code("var m = [1: 'a', 2: 'b'] m[3]").exception(ls::vm::Exception::ARRAY_OUT_OF_BOUNDS);
	code("var m = [1: 2, 3: 4] m[5] = 6 m").equals("[1: 2, 3: 4, 5: 6]");
	code("var m = ['a': 2, 'b': 4] m['c'] = 6 m").equals("['a': 2, 'b': 4, 'c': 6]");
	code("var m = ['a': 2.5, 'b': 4.8] m['c'] = 6.9 m").equals("['a': 2.5, 'b': 4.8, 'c': 6.9]");
	code("var m = [1: 'a', 2: 'b'] m[3] = 'c' m").equals("[1: 'a', 2: 'b', 3: 'c']");
	code("var m = ['a': '2', 'b': '4'] m['c'] = '6' m").equals("['a': '2', 'b': '4', 'c': '6']");
	code("var m = [1: 2, 3: 4] m[3] = 6 m").equals("[1: 2, 3: 6]");
	code("var m = [1: 2.5, 3: 4.5] m[3] = 6.5 m").equals("[1: 2.5, 3: 6.5]");
	code("var m = [1.5: 2, 3.5: 4] m[3.5] = 6 m").equals("[1.5: 2, 3.5: 6]");
	code("var m = [1.5: 2.5, 3.5: 4.5] m[3.5] = 6.5 m").equals("[1.5: 2.5, 3.5: 6.5]");
	code("var m = ['1': 2, '3': 4] m['3'] = 6 m").equals("['1': 2, '3': 6]");
	code("var m = [1.5: 'a', 2.5: 'b'] m[2.5] = 'c' m").equals("[1.5: 'a', 2.5: 'c']");
	code("['', [1: 2][1]]").equals("['', 2]");
	code("['', [1: 2.5][1]]").equals("['', 2.5]");
	code("var m = [:] var ns = '01234566' m[ns] = 1").equals("1");

	section("Map.operator [] left-value");
	code("var m = [1: 2] m[1]++ m").equals("[1: 3]");
	code("var m = ['a': 2] m['a']++ m").equals("['a': 3]");
	code("var k = ['a', 12][0] var m = ['a': 2] m[k]++ m").equals("['a': 3]");

	section("Map.operator [] on unknown maps");
	code("var m = ptr(['a': '2']) m['c'] = '6' m").equals("['a': '2', 'c': '6']");
	code("var m = ptr([2: 'a']) m[3] = 'b' m").equals("[2: 'a', 3: 'b']");
	code("var m = ptr([2.5: 'a']) m[3.6] = 'b' m").equals("[2.5: 'a', 3.6: 'b']");
	code("var m = ptr(['a': 2]) m['c'] = 6 m").exception(ls::vm::Exception::NO_SUCH_OPERATOR);
	code("var m = ptr(['a': 2.2]) m['c'] = 6.6 m").exception(ls::vm::Exception::NO_SUCH_OPERATOR);
	code("var m = ptr([2: 2]) m[3] = 6 m").exception(ls::vm::Exception::NO_SUCH_OPERATOR);
	code("var m = ptr([2.5: 2]) m[3.5] = 6 m").exception(ls::vm::Exception::NO_SUCH_OPERATOR);
	code("var m = ptr([2.5: 2.8]) m[3.5] = 6 m").exception(ls::vm::Exception::NO_SUCH_OPERATOR);
	code("var m = ptr([2: 2.8]) m[3] = 6 m").exception(ls::vm::Exception::NO_SUCH_OPERATOR);
	code("var m = ptr([2: 'a']) m['toto'] = 'b' m").exception(ls::vm::Exception::NO_SUCH_OPERATOR);
	code("var m = ptr([2.5: 'a']) m['toto'] = 'b' m").exception(ls::vm::Exception::NO_SUCH_OPERATOR);

	/*
	 * Iteration
	 */
	section("Map iteration");
	code("for k, v in [:] { System.print(k + ' ' + v) }").output("");
	code("for k, v in [1:2] { System.print(k + ' ' + v) }").output("1 2\n");
	code("for k, v in [1:2,3:4] { System.print(k + ' ' + v) }").output("1 2\n3 4\n");
	code("for k, v in [1:2,3:4,5:6] { System.print(k + ' ' + v) }").output("1 2\n3 4\n5 6\n");
	code("for k, v in ['a':'b'] { System.print(k + ' ' + v) }").output("a b\n");
	code("for k, v in ['a':'b','c':'d'] { System.print(k + ' ' + v) }").output("a b\nc d\n");
	code("for k, v in ['a':'b','c':'d','e':'f'] { System.print(k + ' ' + v) }").output("a b\nc d\ne f\n");

	/*
	 * Methods
	 */
	section("Map.size()");
	code("let x = [1 : 1 1 : 2 1 : 3] x.size()").equals("1");
	code("let x = [1 : 1 1 : 2 2 : '3'] x.size()").equals("2");
	code("size([1 : 1 2 : '3'])").equals("2");

	section("Map.insert()");
	code("var x = [1 : 1] x.insert(2, 2)").equals("true");
	code("var x = ['a' : 'a'] x.insert(2, 2) x").equals("[2: 2, 'a': 'a']");
	code("var x = [1 : 'a'] x.insert(2, 3) x").equals("[1: 'a', 2: 3]");
	code("var x = ['a' : 1] x.insert(2, 3) x").equals("[2: 3, 'a': 1]");
	code("var x = ['a' : 1] x.insert('a', 3)").equals("false");
	code("var x = ['a' : 1.5] x.insert('b', 3.2) x").equals("['a': 1.5, 'b': 3.2]");
	code("var x = ['a' : 1.5] x.insert('a', 3.2)").equals("false");

	section("Map.clear()");
	code("var x = [:] x.clear()").equals("[:]");
	code("var x = [1: 1] x.clear()").equals("[:]");
	code("var x = [1: 'a'] x.clear()").equals("[:]");
	code("var x = ['a': 1] x.clear()").equals("[:]");
	code("var x = ['a': 'a'] x.clear()").equals("[:]");
	code("var x = ['a': 'a', 'b': 'b'] x.clear()").equals("[:]");

	section("Map.erase()");
	code("var x = [1 : 1] x.erase(1)").equals("true");
	code("var x = ['a' : 'a'] x.erase('a') x").equals("[:]");
	code("var x = ['a' : 'a'] x.erase('b') x").equals("['a': 'a']");
	code("var x = ['a' : 1] x.erase(3.14) x").equals("['a': 1]");

	section("Map.foldLeft()");
	code("[:].foldLeft((r, k, v) => r + k * 1000 + v, 0)").equals("0");
	code("[1: 2, 3: 4].foldLeft((r, k, v) => r + k * 1000 + v, 0)").equals("4006");
	code("[1: 2.5, 3: 4.7].foldLeft((r, k, v) => r + k * 10000 + v, 0)").equals("40007.2");
	code("[1: 'b', 3: 'd'].foldLeft((r, k, v) => r + k * 1000 + v, 0)").equals("'1000b3000d'");
	code("['a': 2, 'c': 4].foldLeft((r, k, v) => r + k * 10 + v, 0)").equals("'0aaaaaaaaaa2cccccccccc4'");
	code("['a': 2.5, 'c': 4.7].foldLeft((r, k, v) => r + k * 10 + v, 0)").equals("'0aaaaaaaaaa2.5cccccccccc4.7'");
	code("['a': [55], 'c': [77]].foldLeft((r, k, v) => r + k * 10 + v, 0)").equals("'0aaaaaaaaaa[55]cccccccccc[77]'");

	section("Map.foldRight()");
	code("[:].foldRight((k, v, r) => r + k * 1000 + v, 0)").equals("0");
	code("[1: 2, 3: 4].foldRight((k, v, r) => r + k * 1000 + v, 0)").equals("4006");
	code("[1: 2.5, 3: 4.7].foldRight((k, v, r) => r + k * 10000 + v, 0)").equals("40007.2");
	code("[1: 'b', 3: 'd'].foldRight((k, v, r) => r + k * 1000 + v, 0)").equals("'3000d1000b'");
	code("['a': 2, 'c': 4].foldRight((k, v, r) => r + k * 10 + v, 0)").equals("'0cccccccccc4aaaaaaaaaa2'");
	code("['a': 2.5, 'c': 4.7].foldRight((k, v, r) => r + k * 10 + v, 0)").equals("'0cccccccccc4.7aaaaaaaaaa2.5'");
	code("['a': [55], 'c': [77]].foldRight((k, v, r) => r + k * 10 + v, 0)").equals("'0cccccccccc[77]aaaaaaaaaa[55]'");

	section("Map.look()");
	code("let x = [1: 1] x.look(1, 0)").equals("1");
	code("let x = ['a': 'a'] x.look('a', 'b')").equals("'a'");
	code("let x = ['a': 'a'] x.look('b', 'b')").equals("'b'");
	code("let x = ['a': 1] x.look(3.14, 'a')").error(ls::Error::METHOD_NOT_FOUND, {ls::Type::const_map(env.any, env.integer)->to_string() + ".look(" + env.real->to_string() + ", " + env.tmp_string->to_string() + ")"});
	code("[1 : 1].look(1, 0)").equals("1");
	code("[1 : 'a'].look(2, 10)").equals("10");
	code("[1 : 'a'].look(2, 5.5)").equals("5.5");
	code("['a' : 'a'].look(2, 5.5)").equals("5.5");

	section("Map.values()");
	code("let m = [5: 1, 7: 2, -21: 3] m.values()").equals("[3, 1, 2]");
	code("let m = [5: 1.5, 7: 2.5, -21: 3.5] m.values()").equals("[3.5, 1.5, 2.5]");
	code("let m = [5: [], 7: 'str', -21: true] m.values()").equals("[true, [], 'str']");
	code("[5.5: 10, 7.2: 100, -21.95: 1000].values()").equals("[1000, 10, 100]");
	code("[5.1: 12.1, -7.6: 0, 14.88: -0xfe5a].values()").equals("[0, 12.1, -65114]");
	code("[-1.111: ':)', 6.6: ':/', 9: ':D'].values()").equals("[':)', ':/', ':D']");
	code("['yolo': 3, false: 1, 12: 4].values()").equals("[1, 4, 3]");
	code("[[]: 3.7, [1: 2]: 1.3, {x: 12}: 4.8].values()").equals("[3.7, 1.3, 4.8]");
	code("[(x, y -> x + y): (x, y -> x - y), null: 'null', <'a', 'b'>: 0].values()").equals("['null', 0, <function>]");

	section("Map.iter");
	code("[1: 2, 3: 4].iter((k, v) -> System.print(k + ' ' + v))").output("1 2\n3 4\n");
	code("[1: 2.5, 3: 4.5].iter((k, v) -> System.print(k + ' ' + v))").output("1 2.5\n3 4.5\n");
	code("[1: 'a', 3: 'b'].iter((k, v) -> System.print(k + ' ' + v))").output("1 a\n3 b\n");
	code("['a': 2, 'b': 4].iter((k, v) -> System.print(k + ' ' + v))").output("a 2\nb 4\n");
	code("['a': 2.5, 'b': 4.5].iter((k, v) -> System.print(k + ' ' + v))").output("a 2.5\nb 4.5\n");
	code("['a': 'b', 'c': 'd'].iter((k, v) -> System.print(k + ' ' + v))").output("a b\nc d\n");
	code("var s = 0 [1: 2, 3: 4, 5: 6].iter((k, v) -> s += k) s").equals("9");
	code("var s = 0 [:].iter((k, v) -> s += v) s").equals("0");

	section("Map.max()");
	code("[:].max()").exception(ls::vm::Exception::ARRAY_OUT_OF_BOUNDS);
	code("[12 : 0].clear().max()").exception(ls::vm::Exception::ARRAY_OUT_OF_BOUNDS);
	code("[1 : 4, 2 : 20, 12 : 1,  0 : 4].max()").equals("20");
	code("let a = ['idx0' : 4, 'idx1' : 12, 'idx2' : 1, 'idx3' : 4] a.max()").equals("12");
	code("['c' : 'c', 'a' : 'a', 'd' : 'd', 'b' : 'b'].max()").equals("'d'");
	code("let a = ['c' : 'c', 'a' : 'a', 'd' : 'd', 'b' : 'b'] a.max()").equals("'d'");
	code("[0 : 4, 42 : 20.5, 100 : 1, -1 : 4.99].max()").equals("20.5");

	section("Map.maxKey()");
	code("[:].maxKey()").exception(ls::vm::Exception::ARRAY_OUT_OF_BOUNDS);
	code("[12 : 0].clear().maxKey()").exception(ls::vm::Exception::ARRAY_OUT_OF_BOUNDS);
	code("[1 : 4, 2 : 20, 12 : 1,  0 : 4].maxKey()").equals("12");
	code("let a = ['idx0' : 4, 'idx1' : 12, 'idx2' : 1, 'idx3' : 4] a.maxKey()").equals("'idx3'");
	code("['c' : 'c', 'a' : 'a', 'd' : 'd', 'b' : 'b'].maxKey()").equals("'d'");
	code("let a = ['c' : 'c', 'a' : 'a', 'd' : 'd', 'b' : 'b'] a.maxKey()").equals("'d'");
	code("[0 : 4, 42 : 20.5, 100 : 1, -1 : 4.99].maxKey()").equals("100");

	section("Map.min()");
	code("[:].min()").exception(ls::vm::Exception::ARRAY_OUT_OF_BOUNDS);
	code("[12 : 0].clear().min()").exception(ls::vm::Exception::ARRAY_OUT_OF_BOUNDS);
	code("[1 : 4, 2 : 20, 12 : 1,  0 : 4].min()").equals("1");
	code("let a = ['idx0' : 4, 'idx1' : 12, 'idx2' : 1, 'idx3' : 4] a.min()").equals("1");
	code("['c' : 'c', 'a' : 'a', 'd' : 'd', 'b' : 'b'].min()").equals("'a'");
	code("let a = ['c' : 'c', 'a' : 'a', 'd' : 'd', 'b' : 'b'] a.min()").equals("'a'");
	code("[0 : 4.01, 42 : 20.5, 100 : 10, -1 : 4.99].min()").equals("4.01");

	section("Map.minKey()");
	code("[:].minKey()").exception(ls::vm::Exception::ARRAY_OUT_OF_BOUNDS);
	code("[12 : 0].clear().minKey()").exception(ls::vm::Exception::ARRAY_OUT_OF_BOUNDS);
	code("[1 : 4, 2 : 20, 12 : 1,  0 : 4].minKey()").equals("0");
	code("let a = ['idx0' : 4, 'idx1' : 12, 'idx2' : 1, 'idx3' : 4] a.minKey()").equals("'idx0'");
	code("['c' : 'c', 'a' : 'a', 'd' : 'd', 'b' : 'b'].minKey()").equals("'a'");
	code("let a = ['c' : 'c', 'a' : 'a', 'd' : 'd', 'b' : 'b'] a.minKey()").equals("'a'");
	code("[0 : 4.01, 42 : 20.5, 100 : 10, -1 : 4.99].minKey()").equals("-1");
}
