#include "Test.hpp"

void Test::test_arrays() {

	header("Arrays");

	section("Constructor");
	code("Array").equals("<class Array>");
	code("Array()").equals("[]");
	code("new Array").equals("[]");
	code("new Array()").equals("[]");
	code("new Array() + 1").equals("[1]");
	code("[]").equals("[]");
	code("[1]").equals("[1]");
	code("[1, 2, 3]").equals("[1, 2, 3]");
	code("[1l, 2l, 3l]").equals("[1, 2, 3]");
	code("[1.21, -5, 4.55, 12, -6.7]").equals("[1.21, -5, 4.55, 12, -6.7]");
	code("[true, false, true]").equals("[true, false, true]");
	code("[23, true, '', {}, 123]").equals("[23, true, '', {}, 123]");
	code("let a = x -> x [1, 2, a]").equals("[1, 2, <function>]");
	code("[1m, 34324234m, 231232131232132134379897874534243257343341432423m]").equals("[1, 34324234, 231232131232132134379897874534243257343341432423]");
	code("[true, 'hello', 231232131232132134379897874534243257343341432423m]").equals("[true, 'hello', 231232131232132134379897874534243257343341432423]");

	section("No commas");
	code("[1 2 3]").equals("[1, 2, 3]");
	code("['yo' 'ya' 'yu']").equals("['yo', 'ya', 'yu']");
	code("[true false true true]").equals("[true, false, true, true]");
	code("[[1 2] [[3] 4 [5 6] []]]").equals("[[1, 2], [[3], 4, [5, 6], []]]");

	section("Trailing comma");
	code("[1, 2, 3, ]").equals("[1, 2, 3]");
	code("[1,\n2,\n3,\n]").equals("[1, 2, 3]");

	section("Array.operator =");
	code("var x = [0] x = [3]").equals("[3]");
	code("var x = [0] x = [3] x").equals("[3]");
	code("var x = [] x = [3]").equals("[3]");
	code("var x = [] x = [3] x").equals("[3]");

	section("Array.operator +");
	code("[1, 2, 3] + [4, 5, 6]").equals("[1, 2, 3, 4, 5, 6]");
	code("[] + 1").equals("[1]");
	code("[] + 1 + 2 + 3").equals("[1, 2, 3]");
	code("[1] + 2").equals("[1, 2]");
	code("[1] + 0.5").equals("[1, 0.5]");
	code("[0.5] + 'a'").equals("[0.5, 'a']");
	code("['a'] + ['b']").equals("['a', 'b']");
	code("[1] + 0.5 + 'a'").equals("[1, 0.5, 'a']");
	code("[1] + 0.5 + 'a' + 'b'").equals("[1, 0.5, 'a', 'b']");
	code("[1] + null").equals("[1, null]");
	code("[1] + true").equals("[1, true]");
	code("[1] + [2] + [3]").equals("[1, 2, 3]");
	code("[1] + [2.5] + ['a']").equals("[1, 2.5, 'a']");
	code("['a'] + [2.5] + [1]").equals("['a', 2.5, 1]");
	code("[1] + ['a']").equals("[1, 'a']");
	code("['a'] + [1]").equals("['a', 1]");
	code("[12.5] + null").equals("[12.5, null]");
	code("let a = ['yo'] a + null").equals("['yo', null]");
	code("[12.5] + true").equals("[12.5, true]");
	code("['yo'] + true").equals("['yo', true]");
	code("let a = ['yo'] a + true").equals("['yo', true]");
	code("[15.5] + [12, ''][0]").equals("[15.5, 12]");
	code("let a = [15.5] a + [12, ''][0]").equals("[15.5, 12]");
	code("let a = [1] a + [12, ''][0]").equals("[1, 12]");
	code("['yo'] + '!'").equals("['yo', '!']");
	code("let a = ['yo'] a + '!'").equals("['yo', '!']");
	code("let a = [5] let b = ['b'] a + b").equals("[5, 'b']");
	code("['a'] + ['b']").equals("['a', 'b']");
	code("let a = ['a'] a + ['b']").equals("['a', 'b']");
	code("[1, 2] + {}").equals("[1, 2, {}]");
	code("let a = [1, 2] a + {}").equals("[1, 2, {}]");
	code("['a', 'b'] + {}").equals("['a', 'b', {}]");
	code("let a = ['a', 'b'] a + {}").equals("['a', 'b', {}]");
	code("['a', 'b'] + (x -> x)").equals("['a', 'b', <function>]");
	code("let a = ['a', 'b'] a + (x -> x)").equals("['a', 'b', <function>]");
	code("['a', 'b'] + Number").equals("['a', 'b', <class Number>]");
	code("let a = ['a', 'b'] a + Number").equals("['a', 'b', <class Number>]");
	code("let a = [1, 2] a + [3, 4]").equals("[1, 2, 3, 4]");
	code("let a = ['a'] [3.5, 4.6] + a").equals("[3.5, 4.6, 'a']");
	code("var pq = [] pq = pq + 1 pq").equals("[1]");
	code("[1l] + 12l").equals("[1, 12]");
	code("[] + 12l").equals("[12]");

	section("Array.operator ~");
	code("let a = [1, 2, 3]; ~a").equals("[3, 2, 1]");
	code("~[1, 2, 3]").equals("[3, 2, 1]");

	section("Array.operator !");
	code("let a = [1, 2, 3]; !a").equals("false");
	code("![1, 2, 3]").equals("false");
	code("![1.5, 2.5, 3.5]").equals("false");
	code("![]").equals("true");

	section("Array.operator |x|");
	code("|[1, 2, 3]|").equals("3");
	code("|[1.5, 2.5, 3.5]|").equals("3");
	code("|['a', 'b', 'c']|").equals("3");
	code("[1, 2, |[1, 2, 3]|, 4]").equals("[1, 2, 3, 4]");
	code("['1', 2, |[1, 2, 3]|, 4]").equals("['1', 2, 3, 4]");
	code("|[1l, 2l, 3l, 4l]|").equals("4");

	section("Array.operator []");
	code("[1, 2, 3][1]").equals("2");
	code("let a = [1, 2, 3] a[0]").equals("1");
	code("let a = [1.6, 2.5, 3.4] a[0]").equals("1.6");
	code("let a = [1, 2, 3] a[0] = 5 a[0]").equals("5");
	code("let a = [23, 23, true, '', [], 123]; |a|").equals("6");
	code("let a = [] !a").equals("true");
	code("let a = [1, 2, 3] a[1] = 12 a").equals("[1, 12, 3]");
	code("[1.2, 321.42, 23.15]").equals("[1.2, 321.42, 23.15]");
	code("[1, 2, 3, 4, 5][1:3]").equals("[2, 3, 4]");
	code("let a = [5, 'yolo', 12] a[1]").equals("'yolo'");
	code("let a = [12] a[0]++ a").equals("[13]");
	code("[1, 2, 'a'][['salut', 2][0]]").exception(ls::vm::Exception::ARRAY_KEY_IS_NOT_NUMBER);
	code("['a', 'b', 'c'][[2, ''][0]]").equals("'c'");
	code("let a = [[12], ''][0] a[0]++ a").equals("[13]");
	code("let a = [[12], ''][0] a[a]++ a").exception(ls::vm::Exception::ARRAY_KEY_IS_NOT_NUMBER);
	code("let a = [[12], [5.5], ['a']] a[0][0] += 1 a[1][0] += 1 a[2][0] += 1 a").equals("[[13], [6.5], ['a1']]");
	code("let a = [1, 2, 3] a[0l]").equals("1");
	code("let a = [1, 2, 3] a[1l]").equals("2");
	code("let a = [1, 2, 3] a[2m]").equals("3");
	code("let a = ['a', 'b', 'c'] a[0.5]").equals("'a'");
	code("let a = ['a', 'b', 'c'] a[1.9]").equals("'b'");
	code("['', [2][0]]").equals("['', 2]");
	code("['', [2.5][0]]").equals("['', 2.5]");
	code("let a = [1, 2, 3] a[true]").equals("2");
	code("[1, 2.5][1]").equals("2.5");
	code("[1, true][0]").equals("1");
	code("[1, true][1]").equals("true");
	code("[5l, 7l, 9l][2l]").equals("9");

	section("[] operator on unknown arrays");
	code("let v = [['a', 'b'], 12] v[0][0]").equals("'a'");
	code("let v = [['a', 'b'], 12] v[0][1]").equals("'b'");
	code("let v = [['a', 'b'], 12] v[0][true]").equals("'b'");
	code("[['a', 'b'], 12][0][['yolo', 1][0]]").exception(ls::vm::Exception::ARRAY_KEY_IS_NOT_NUMBER);
	code("[['a', 'b'], 12][0][2]").exception(ls::vm::Exception::ARRAY_OUT_OF_BOUNDS);
	code("let v = [['a', 'b'], 12] v[0][0] = 5 v").equals("[[5, 'b'], 12]");
	code("let v = [['a', 'b'], 12] v[0][2] = 5 v").exception(ls::vm::Exception::ARRAY_OUT_OF_BOUNDS);
	code("let a = [[12], [1..10]][1] a[5]").equals("6");

	section("Out of bounds exception");
	code("[][1]").exception(ls::vm::Exception::ARRAY_OUT_OF_BOUNDS);
	code("[1, 2, 3][100]").exception(ls::vm::Exception::ARRAY_OUT_OF_BOUNDS);
	code("let a = [1, 2, 3] a[10]").exception(ls::vm::Exception::ARRAY_OUT_OF_BOUNDS);
	code("[5.6, 7.2][-5]").exception(ls::vm::Exception::ARRAY_OUT_OF_BOUNDS);
	code("['hello', true][2]").exception(ls::vm::Exception::ARRAY_OUT_OF_BOUNDS);
	code("let a = [1, 2, 3] a[100] = 12").exception(ls::vm::Exception::ARRAY_OUT_OF_BOUNDS);
	code("let a = [1, 2, 3] a[-100] = 12").exception(ls::vm::Exception::ARRAY_OUT_OF_BOUNDS);
	code("let a = [[12], ''][0] a[100]++ a").exception(ls::vm::Exception::ARRAY_OUT_OF_BOUNDS);
	code("let a = [5] let e = a[1] !? 5 e").equals("5");

	section("Access with booleans");
	code("[1, 2, 3][false]").equals("1");
	code("[1, 2, 3][true]").equals("2");
	code("['1', '2', '3'][false]").equals("'1'");
	code("[1.5, 2.5, 3.5][true]").equals("2.5");

	section("Push with empty array access");
	code("var a = [] a[] = 12 a").equals("[12]");
	code("var a = [1, 2] a[] = 3 a").equals("[1, 2, 3]");
	code("var a = [1, 2] a[] = 3 a").equals("[1, 2, 3]");
	code("var a = [] a[] = 'a' a").equals("['a']");
	code("var a = ['a', 'b'] a[] = 'c' a").equals("['a', 'b', 'c']");
	code("var a = [1, 'b', true] a[] = x -> x a").equals("[1, 'b', true, <function>]");
	code("var a = ['a', 'b', 'c'] a[] = ['d'][0] a").equals("['a', 'b', 'c', 'd']");

	section("Methods calls on unknown array");
	code("var a = [1, [1, 2]] a[1].size()").equals("2");
	code("var a = [1, [1, 2]] a[1].push(3)").equals("[1, 2, 3]");
	code("var a = [1, [1, 2]] a[1].push(3) a[1]").equals("[1, 2, 3]");
	code("var a = [1, ['a', 'b']] a[1].push('c') a[1]").equals("['a', 'b', 'c']");
	code("var a = [[], ['a']] a[1].push('b') a").equals("[[], ['a', 'b']]");

	section("Array.operator +=");
	code("var a = [1.55] a += 12.9 a").equals("[1.55, 12.9]");
	code("var a = ['a'] a += 'b' a").equals("['a', 'b']");
	code("var a = [1, 2, 3] a[0] += 5 a[0]").equals("6");
	code("var v = 12 var a = [v, 2, 3] a[0] += 5 a[0]").equals("17");
	code("var a = [1, 2, 3] a += 'hello' a").equals("[1, 2, 3, 'hello']");
	code("var a = [1.5] a += ['a', 'b'] a").equals("[1.5, 'a', 'b']");
	code("var a = [1.5] a += false a").equals("[1.5, false]");
	code("var a = [1] a += <2, 3> a").equals("[1, 2, 3]");
	code("var a = [1] a += <5.5, 7.314> a").equals("[1, 5.5, 7.314]");
	code("var a = [1] a += <5, 7.314, 'hello'> a").equals("[1, 5, 7.314, 'hello']");
	code("var a = [1] a += [<5, 7.314, 'hello'>, ''][0] a").equals("[1, 5, 7.314, 'hello']");
	code("var a = [1] a += <'z', 'a'> a").equals("[1, 'a', 'z']");
	code("var a = [1] a += 'a' a").equals("[1, 'a']");
	code("var a = [[1]] a[0] += [12, ''][0] a[0]").equals("[1, 12]");
	code("var a = [1.11] a += [2, 3] a").equals("[1.11, 2, 3]");
	code("var a = [[1.55]] a[0] += [12.9, ''][0] a[0]").equals("[1.55, 12.9]");
	code("var a = [[1.55]] a[0] += [-1.5, 6.7] a[0]").equals("[1.55, -1.5, 6.7]");
	code("var a = [[1.55]] a[0] += <8, 4> a[0]").equals("[1.55, 4, 8]");
	code("var a = [[1.55]] a[0] += < -8.5, 4.7> a[0]").equals("[1.55, -8.5, 4.7]");
	code("var a = [[1.55]] a[0] += < -8.5, 4.7, 'hello'> a[0]").equals("[1.55, -8.5, 4.7, 'hello']");
	code("var a = ['a'] a += [1, 2]").equals("['a', 1, 2]");
	code("var a = ['a'] a += [1.5, 2.5]").equals("['a', 1.5, 2.5]");
	code("var a = ['a'] a += <1, 2>").equals("['a', 1, 2]");
	code("var a = ['a'] a += <1.5, 2.5>").equals("['a', 1.5, 2.5]");
	code("var a = ['a'] var b = <'b'> a += b a").equals("['a', 'b']");
	code("var a = ['a'] var b = [12] a += b a").equals("['a', 12]");
	code("var a = [[5], ''][0] var b = [12] a + b").equals("[5, 12]");
	code("var a = [[1.5], ''][0] var b = ['a'] a + b").equals("[1.5, 'a']");
	code("var a = [['a'], ''][0] var b = [12] a + b").equals("['a', 12]");
	code("var a = [['a'], ''][0] var b = [12.9] a + b").equals("['a', 12.9]");
	code("var a = [1, 2, 3] a[1] += 0.5 a").equals("[1, 2.5, 3]");
	code("var a = [1, 2, 3] a += [4] a").equals("[1, 2, 3, 4]");
	code("var a = [1, 2, 3] a[1] += 500l a").equals("[1, 502, 3]");
	code("var a = [] if (true) a += 12 a").equals("[12]");
	code("var a = [1] if (true) a += 12 a").equals("[1, 12]");
	code("var a = ['a'] if (true) a += 12 a").equals("['a', 12]");

	section("Array.operator ~~");
	code("[1, 2, 3, 4, 5] ~~ x -> x ** 2").equals("[1, 4, 9, 16, 25]");
	code("[1.5, 2.5, 3.5] ~~ x -> x.floor()").equals("[1, 2, 3]");
	code("[1, 2, 3, 4, 5] ~~ (x -> x ** 2)").equals("[1, 4, 9, 16, 25]");
	code("[1l, 2, 3, 4, 5] ~~ (x -> x ** 2)").equals("[1, 4, 9, 16, 25]");
	code("['yo', 'toto', 'salut'] ~~ x -> x + ' !'").equals("['yo !', 'toto !', 'salut !']");
	code("[1, 2, 3] ~~ x -> [x]").equals("[[1], [2], [3]]");
	code("[1, 2, 3] ~~ x -> 'yo'").equals("['yo', 'yo', 'yo']");
	code("let f = x -> x * 10 [1, 2, 3] ~~ f").equals("[10, 20, 30]");
	code("[1.2, 321.42] ~~ x -> x * 1.7").equals("[2.04, 546.414]");
	code("[1, 2, 3, 4, 5] ~~ x -> x.max(3)").equals("[3, 3, 3, 4, 5]");
	code("[1, 2, 3, 4, 5, 6, 7, 8, 9, 10] ~~ x -> x.max(3).min(8)").equals("[3, 3, 3, 4, 5, 6, 7, 8, 8, 8]");
	code("[1, 2, 3, 4] ~~ x -> x + 0.5").equals("[1.5, 2.5, 3.5, 4.5]");
	code("[1, 2, 3] ~~ (x -> x * 5) ~~ (x -> x - 1)").equals("[4, 9, 14]");
	code("let f = x -> x + 10 let g = x -> x ** 2 [1, 2, 3] ~~ f ~~ g").equals("[121, 144, 169]");
	code("[1, 2, 3] ~~ Number.sqrt").equals("[1, 1.41421, 1.73205]");
	code("[1, 2, 3] ~~ x -> [x, x]").equals("[[1, 1], [2, 2], [3, 3]]");
	// TODO not working anymore because print is compiler method, not direct function
	DISABLED_code("[1, 2, 3] ~~ System.print").output("1\n2\n3\n");
	code("var s = '0' let a = [1, 2, 3, 4] let f = x -> s += x a ~~ f s").equals("'01234'");
	code("var s = 0 let a = [1, 2, 3, 4] let f = x -> s += x a ~~ f s").equals("10");

	section("Array.operator <");
	code("[1] < [1, 2]").equals("true");
	code("[1, 1] < [1, 2]").equals("true");
	code("['a', 'b'] < [1.5, 2.8]").equals("false");
	code("[1, 2] < [1, 2, 'c']").equals("true");
	code("[1, 2] < [1, null]").equals("false");
	code("[1, 2] < [1, 'a']").equals("true");
	code("[1, 2] < [1, 5, 'a']").equals("true");
	code("[1, 2] < [1, 1, 'a']").equals("false");
	code("[1, 'a'] < [1]").equals("false");
	code("[1, null] < [1, 2]").equals("true");
	code("[1, 'a'] < [0, 2]").equals("false");
	code("[1, 2, 3, 4] < [1, 2, 3, 5]").equals("true");
	code("[1, 2, 4, 4] < [1, 2, 3, 5]").equals("false");
	code("[1, 2, 3, 4.1] < [1, 2, 3, 5.1]").equals("true");
	code("[1, 2, 4, 4.1] < [1, 2, 3, 5.1]").equals("false");
	code("['1', '2', '3', '4'] < ['1', '2', '3', '5']").equals("true");
	code("['1', '2', '4', '4'] < ['1', '2', '3', '5']").equals("false");
	code("[1, 2, 3, 4] < [1, 2, 3, 5.1]").equals("true");
	code("[1, 2, 4, 4.1] < [1, 2, 3, 5]").equals("false");
	code("[1, 2, '3'] < [1, 2, 3]").equals("false");
	code("[1, 1, '3'] < [1, 2, 3]").equals("true");
	code("[[1, 2, 3], 1][0] < 5").equals("false");
	code("ptr([1, 2, 3]) < <>").equals("true");

	section("Array.operator ==");
	code("[] == []").equals("true");
	code("[1, 1] == [1]").equals("false");
	code("[1, 1] == [1, 2]").equals("false");
	code("[1, 2] == [1, 2]").equals("true");
	code("[1, '1'] == [1, '2']").equals("false");
	code("[1, '2'] == [1, '2']").equals("true");
	code("[1, 1] == [1, 2.5]").equals("false");
	code("[1, 2.5] == [1, 2.5]").equals("true");

	section("Array.operator in");
	code("2 in [1, 2, 3]").equals("true");
	code("4 in [1, 2, 3]").equals("false");
	code("'yo' in ['ya', 'yu', 'yo']").equals("true");
	code("let a = 2 if (a in [1, 2, 3]) { 'ok' } else { 'no' }").equals("'ok'");
	code("'a' in [['a', 'b', 'c'], ''][0]").equals("true");
	code("2$ in [1, 2, 3]").equals("true");
	code("4$ in [1, 2, 3]").equals("false");
	code("'salut' in [1, 2, 3]").equals("false");
	code("let f = x -> x[0]; [f([1]), f([0..3])]").equals("[1, 0]");
	code("let f = x -> x[0]; [f([1]), f([0..3]), f(['a'])]").equals("[1, 0, 'a']");

	/*
	 * Methods
	 */
	section("Array.size()");
	code("Array.size([1, 'yo', true])").equals("3");
	code("Array.size([6, 4, 3, 2, -11])").equals("5");
	code("Array.size([6.8999, 5.56])").equals("2");
	code("[1, 'yo', true].size()").equals("3");
	code("[1, -2, 3, -12, -6].size()").equals("5");
	code("[1, 2, 3].size").equals("<function>");
	code("Array.size([1, [1, 2, 3]][1])").equals("3");
	code("[[1, 2, 3], 'foo'][0].size()").equals("3");
	code("size([1, 2, 3, 4])").equals("4");

	section("Array.average()");
	code("Array.average([1, 2, 3, 4, 5, 6])").equals("3.5");
	code("Array.average([1l, 2l, 3l, 4l, 5l, 6l])").equals("3.5");
	code("Array.average([])").equals("0");
	code("[1, 2, 3, 4, 5, 6].average()").equals("3.5");
	code("[].average()").equals("0");
	code("['salut', true, []].average()").equals("0");
	code("[10, -5.7, 30.89, 66].average()").almost(25.2975);

	section("Array.sum()");
	code("Array.sum([10, -5, 30, 66])").equals("101");
	code("[10, -5, 30, 66].sum()").equals("101");
	code("Array.sum(['ab', 'cd', 'ef'])").equals("'abcdef'");
	code("['ab', 'cd', 'ef'].sum()").equals("'abcdef'");
	code("['abc', true, 12, [1, 2]].sum()").equals("'abctrue12[1, 2]'");
	code("[10, -5.7, 30.89, 66].sum()").almost(101.19);
	code("Array.sum([10, -7, 76])").equals("79");

	section("Array.product()");
	code("[1, 2, 3, 4].product()").equals("24");
	code("[1.5, 2.5, 3.5, 4.5].product()").equals("59.0625");
	code("[3l, 4l, 5l].product()").equals("60");

	section("Array.map()");
	code("Array.map([1, 2, 3], x -> x ** 2)").equals("[1, 4, 9]");
	code("[3, 4, 5].map(x -> x ** 2)").equals("[9, 16, 25]");
	code("let a = [3, 4, 5] a.map(x -> x ** 2)").equals("[9, 16, 25]");
	code("[321, 213, 121].map(x -> x ** 2).size()").equals("3");
	code("[3.2, 4.5, 5.8].map(x -> x ** 2)").equals("[10.24, 20.25, 33.64]");
	code("['a' 'b' 'c'].map(x -> x)").equals("['a', 'b', 'c']");
	code("let a = ['a' 'b' 'c'] a.map(x -> x)").equals("['a', 'b', 'c']");
	code("[65 66 67].map(x -> x.char()).join('')").equals("'ABC'");
	code("[['a', 'b', 'c'], 'foo'][0].map(x -> x + '.')").equals("['a.', 'b.', 'c.']");
	code("[{}, [], true, '42'].map(_ => 42)").equals("[42, 42, 42, 42]");
	code("[2.5, 4.7, 8.19].map(x => x > 5)").equals("[false, false, true]");
	code("[true, false, true].map(x => x + 1)").equals("[2, 1, 2]");
	code("[2, 6, 7, 12, 15].map(Number.isPrime)").equals("[true, false, true, false, false]");
	code("[[5], [1, 2], [4]].map(x => x.sum())").equals("[5, 3, 4]");
	code("[5, 7, 9, 11].map(x => [[x], [x]]).map(x => x[0] + x[1])").equals("[[5, 5], [7, 7], [9, 9], [11, 11]]");
	code("[5, 7, 9, 11].map(x => [[x], [x]]).map(x => x[0][0] + x[1][0])").equals("[10, 14, 18, 22]");
	code("'12,5,1'.split(',').map(x => x.number())").equals("[12, 5, 1]");

	section("Array.map2()");
	code("Array.map2([1, 'yo ', []], [12, 55, 9], (x, y -> x + y))").equals("[13, 'yo 55', [9]]");

	section("Array.max()");
	code("[].max()").exception(ls::vm::Exception::ARRAY_OUT_OF_BOUNDS);
	code("[12].clear().max()").exception(ls::vm::Exception::ARRAY_OUT_OF_BOUNDS);
	code("[4, 20, 1, 4].max()").equals("20");
	code("let a = [4, 12, 1, 4] a.max()").equals("12");
	code("['c', 'a', 'd', 'b'].max()").equals("'d'");
	code("let a = ['c', 'a', 'e', 'b'] a.max()").equals("'e'");
	code("[4, 20.5, 1, 4].max()").equals("20.5");
	code("[4l, 25l, 1l, 4l].max()").equals("25");

	section("Array.min()");
	code("[].min()").exception(ls::vm::Exception::ARRAY_OUT_OF_BOUNDS);
	code("[12].clear().min()").exception(ls::vm::Exception::ARRAY_OUT_OF_BOUNDS);
	code("[4, 20, 1, 4].min()").equals("1");
	code("let a = [4, 12, 1, 4] a.min()").equals("1");
	code("['c', 'a', 'd', 'b'].min()").equals("'a'");
	code("let a = ['c', 'a', 'e', 'b'] a.min()").equals("'a'");
	code("[4, 20.5, 1.5, 4].min()").equals("1.5");
	code("[4l, 20l, -1l, 4l].min()").equals("-1");

	section("Array.chunk()");
	code("let x = [1, 2, 3, 4] x.chunk(2)").equals("[[1, 2], [3, 4]]");
	code("let x = [1, 2, 3, 4] x.chunk(3)").equals("[[1, 2, 3], [4]]");
	code("let x = [1, 2, 3, 4] x.chunk()").equals("[[1], [2], [3], [4]]");
	code("let x = ['a', 'b', 'c', 'd'] x.chunk()").equals("[['a'], ['b'], ['c'], ['d']]");
	code("let x = [1.6, 2.5, 3.1, 4.67] x.chunk()").equals("[[1.6], [2.5], [3.1], [4.67]]");
	code("let x = ['hello', x -> x, true, Number] x.chunk()").equals("[['hello'], [<function>], [true], [<class Number>]]");
	code("let x = [1l, 2l, 3l, 4l] x.chunk(2)").equals("[[1, 2], [3, 4]]");

	section("Array.unique()");
	code("var x = [1, 1, 2, 2, 1] x.unique() x").equals("[1, 2, 1]");
	code("var x = [1.5, 1.5, 2.5, 2.5, 1.5] x.unique() x").equals("[1.5, 2.5, 1.5]");
	code("var x = [1, 1, 2, 2, 1] x.sort() x.unique() x").equals("[1, 2]");
	code("var x = ['a', 'a', 'b'] x.unique() x").equals("['a', 'b']");
	code("var x = ['a', 'b', 'c'] x.unique() x").equals("['a', 'b', 'c']");
	code("var x = ['a', 'a', 'b', 'a', 'a'] x.unique() x").equals("['a', 'b', 'a']");
	code("var x = [1l, 1l, 2l, 2l, 1l] x.unique() x").equals("[1, 2, 1]");

	section("Array.sort()");
	code("var x = [3, 1, 2] x.sort() x").equals("[1, 2, 3]");
	code("var x = [3.1, 3.2, 2.7] x.sort() x").equals("[2.7, 3.1, 3.2]");
	code("var x = ['foo', 'yop', 'abc'] x.sort() x").equals("['abc', 'foo', 'yop']");
	code("var x = [[[]], [[], [], []], [[], []]]; x.sort() x").equals("[[[]], [[], []], [[], [], []]]");
	code("var x = [[1, 2, 3], [3, 1, 2], [2, 3, 1]]; x.sort() x").equals("[[1, 2, 3], [2, 3, 1], [3, 1, 2]]");
	code("Array.sort([3, 2, 1])").equals("[1, 2, 3]");
	code("var x = [3l, 1l, 2l] x.sort() x").equals("[1, 2, 3]");

	section("Array.sort(<callback>)");
	code("[5, 1, 3, 2, 4].sort((a, b) => a < b)").equals("[1, 2, 3, 4, 5]");
	code("[5, 1, 3, 2, 4].sort((a, b) => a > b)").equals("[5, 4, 3, 2, 1]");
	code("[5, 1, 3, 2, 4].sort(<)").equals("[1, 2, 3, 4, 5]");
	code("[5, 1, 3, 2, 4].sort(>)").equals("[5, 4, 3, 2, 1]");
	code("['d', 'a', 'c', 'e', 'b'].sort((a, b) => a < b)").equals("['a', 'b', 'c', 'd', 'e']");
	code("['d', 'a', 'c', 'e', 'b'].sort((a, b) => a > b)").equals("['e', 'd', 'c', 'b', 'a']");
	code("['bonjour', 'tout', 'le', 'monde', '!'].sort((a, b) => |a| < |b|)").equals("['!', 'le', 'tout', 'monde', 'bonjour']");
	code("['bonjour', 'tout', 'le', 'monde', '!'].sort((a, b) => |a| > |b|)").equals("['bonjour', 'monde', 'tout', 'le', '!']");
	code("Array.sort([5, 1, 3, 2, 4], (a, b) => a < b)").equals("[1, 2, 3, 4, 5]");
	code("Array.sort([5, 1, 3, 2, 4], >)").equals("[5, 4, 3, 2, 1]");
	code("let f = (x, y) => x < y [5, 1, 3, 2, 4].sort(f)").equals("[1, 2, 3, 4, 5]");
	code("let f = (x, y) => x < y ['a', 'b', 'c', 'd', 'e'].sort(f)").equals("['a', 'b', 'c', 'd', 'e']");
	code("[{a: 5, b: 7}, {a: 12, b: 1}, {a: 9, b: 4}].sort((x, y) -> x.a < y.a)").equals("[{a: 5, b: 7}, {a: 9, b: 4}, {a: 12, b: 1}]");
	code("[{a: 5, b: 7}, {a: 12, b: 1}, {a: 9, b: 4}].sort((x, y) -> x.b < y.b)").equals("[{a: 12, b: 1}, {a: 9, b: 4}, {a: 5, b: 7}]");
	code("[5l, 1l, 3l, 2l, 4l].sort((a, b) => a < b)").equals("[1, 2, 3, 4, 5]");

	section("Array.filter()");
	code("Array.filter([1, 2, 3, 10, true, 'yo'], x -> x > 2)").equals("[3, 10, 'yo']");
	code("[3, 4, 5].filter(x -> x > 6)").equals("[]");
	code("[1, 2, 3, 4, 5, 6, 7].filter(x -> x % 2 == 0)").equals("[2, 4, 6]");
	code("let a = [3, 4, 5] a.filter(x -> x < 5)").equals("[3, 4]");
	code("let a = ['a', 'b', 'a'] a.filter(x -> x == 'a')").equals("['a', 'a']");
	code("[1l, 2l, 3l, 4l, 5l, 6l, 7l].filter(x -> x > 3)").equals("[4, 5, 6, 7]");

	section("Array.contains()");
	code("Array.contains([1, 2, 3, 10, 1], 1)").equals("true");
	code("[3, 4, 5].contains(6)").equals("false");
	code("[3.5, 4.5, 5.5].contains(6.5)").equals("false");
	code("[3.5, 4.5, 6.5].contains(6.5)").equals("true");
	code("['a', true, {}].contains(true)").equals("true");
	code("['a', true, {}].contains(12)").equals("false");

	section("Array.isEmpty()");
	code("Array.isEmpty([])").equals("true");
	code("[3, 4, 5].isEmpty()").equals("false");

	section("Array.iter()");
	code("Array.iter([1, 2, 3], x -> System.print(x))").output("1\n2\n3\n");
	code("[4, 5, 6].iter(x -> System.print(x))").output("4\n5\n6\n");
	code("var a = 0 Array.iter([1, 2, 3], x -> a += x) a").equals("6");
	code("var a = 2 [1, 2, 3].iter(x -> a *= x) a").equals("12");
	code("var a = '' Array.iter([1, 2, 3], x -> a += x) a").equals("'123'");
	code("var s = 0 [1, 2, 3, 4, 5].iter(x -> s += x) s").equals("15");

	section("Array.partition()");
	code("Array.partition([1, 2, 3, 4, 5], (x -> x < 3))").equals("[[1, 2], [3, 4, 5]]");
	code("Array.partition([1, 2, 3, 10, true, 'yo'], x -> x > 2)").equals("[[3, 10, 'yo'], [1, 2, true]]");
	code("[1, 2, 3, 4, 5].partition(x -> x > 3)").equals("[[4, 5], [1, 2, 3]]");
	code("[1, 2, 3, 4, 5].partition(x -> x == 3)").equals("[[3], [1, 2, 4, 5]]");
	code("[1, 2, 3, 4, 5, 6].filter(x -> x > 2).partition(x -> x > 4)").equals("[[5, 6], [3, 4]]");
	code("[1, 2, 3, 4, 5].partition(x -> 'yolo')").equals("[[1, 2, 3, 4, 5], []]");

	section("Array.first()");
	code("Array.first([1, 2, 3, 10, true, 'yo', null])").equals("1");
	code("['yo', 3, 4, 5].first()").equals("'yo'");
	code("[12, 2].first()").equals("12");
	code("[12.891, 2].first()").equals("12.891");
	code("[].first()").exception(ls::vm::Exception::ARRAY_OUT_OF_BOUNDS);
	code("[12].clear().first()").exception(ls::vm::Exception::ARRAY_OUT_OF_BOUNDS);
	code("[12.5].clear().first()").exception(ls::vm::Exception::ARRAY_OUT_OF_BOUNDS);
	code("[5, true].first()").equals("5");
	code("[[5, true], [6, false], [7, true]].first().first()").equals("5");

	section("Array.last()");
	code("Array.last([1, 2, 3, 10, true, 'yo', null])").equals("null");
	code("['yo', 3, 4, 5].last()").equals("5");
	code("[12, 2].last()").equals("2");
	code("[12.891, 2].last()").equals("2");
	code("[].last()").exception(ls::vm::Exception::ARRAY_OUT_OF_BOUNDS);
	code("[12].clear().last()").exception(ls::vm::Exception::ARRAY_OUT_OF_BOUNDS);
	code("[12.5].clear().last()").exception(ls::vm::Exception::ARRAY_OUT_OF_BOUNDS);
	code("[[321, 21], [23, 212], [654, 9876]].first().last()").equals("21");
	code("[[321, 21], [23, 212], [654, 9876]].last().first()").equals("654");
	code("[[321, 'a'], [23, 'b'], [654, 'c']].first().last()").equals("'a'");
	code("[[321, 'a'], [23, 'b'], [654, 'c']].last().first()").equals("654");

	section("Array.foldLeft()");
	code("Array.foldLeft([1, 2, 3, 10, true, 'yo', null], (x, y -> x + y), 'concat:')").equals("'concat:12310trueyonull'");
	code("Array.foldLeft([2, 2, 3], (x, y -> x ** y), 1)").equals("1");
	code("Array.foldLeft([2.5, 3.5], (x, y -> x ** y), 1.5)").equals("34.7374965567");
	code("Array.foldLeft(['a', 'b', 'c', 'd'], (x, y -> x + y), 'X')").equals("'Xabcd'");
	code("['1', '2', '3'].foldLeft(+, '0')").equals("'0123'");
	code("[1, 2, 3, 4, 5].foldLeft(+, 0)").equals("15");
	code("let a = +; [1, 2, 3, 4, 5].foldLeft(a, 0)").equals("15");
	code("[1, 2, 3, 4, 5].foldLeft(-, 0)").equals("-15");
	code("let a = -; [1, 2, 3, 4, 5].foldLeft(a, 0)").equals("-15");
	code("[1, 3, 4, 2, 7, 5, 8, 9, 6].foldLeft((+), 0)").equals("45");
	code("[1, 3, 4, 2, 7, 5, 8, 9, 6].foldLeft((-), 0)").equals("-45");
	code("[1, 3, 4, 2, 7, 5, 8, 9, 6].foldLeft(*, 1)").equals("362880");
	code("['a', 'b', 'c'].foldLeft(=> '%', '')").equals("'%'");
	code("[1, 2, 3].foldLeft((acc, x) -> acc.push(x), [])").equals("[1, 2, 3]");

	section("Array.foldRight()");
	code("Array.foldRight([2, 2, 3], (x, y -> x ** y), 1)").equals("256");
	code("Array.foldRight(['a', 'b', 'c', 'd'], (x, y -> x + y), 'X')").equals("'abcdX'");
	code("Array.foldRight([1.5, 2.0, 2.5], (x, y -> x ** y), 1.5)").equals("533.166813742");
	code("[1, 2, 3].foldRight((x, acc -> acc.push({w: x})), [])").equals("[{w: 3}, {w: 2}, {w: 1}]");

	section("Array.nextPermutation");
	code("var a = [] a.nextPermutation() a").equals("[]");
	code("var a = [1] a.nextPermutation() a").equals("[1]");
	code("var a = [1, 2, 3, 4, 5] a.nextPermutation() a").equals("[1, 2, 3, 5, 4]");
	code("var a = [5, 4, 3, 2, 1] a.nextPermutation() a").equals("[1, 2, 3, 4, 5]");
	code("var a = [1l] a.nextPermutation() a").equals("[1]");
	code("var a = [1l, 2l, 3l, 4l, 5l] a.nextPermutation() a").equals("[1, 2, 3, 5, 4]");

	section("Array.shuffle()");
	code("[].shuffle()");
	code("[1.5, 2.5].shuffle().size()").equals("2");
	code("Array.shuffle([1, 2, 3, 10, true, 'yo', null]).size()").equals("7");
	code("var a = [1, 2, 3, 10, true, 'yo', null] a.shuffle().size()").equals("7");
	code("([] + 'a' + 'b').shuffle()").works();

	section("Array.reverse()");
	code("Array.reverse([1, 2, 3, 10, true, 'yo', null])").equals("[null, 'yo', true, 10, 3, 2, 1]");
	code("[null].reverse()").equals("[null]");
	code("[].reverse()").equals("[]");
	code("[1, 2, 3].reverse()").equals("[3, 2, 1]");
	code("[1.5, 2.5, 3.5].reverse()").equals("[3.5, 2.5, 1.5]");
	code("let a = [1, 2, 3] a.reverse()").equals("[3, 2, 1]");

	section("Array.search()");
	code("Array.search([1, 2, 3, 10, true, 'yo', null], 10, 0)").equals("3");
	code("Array.search([1, 2, 3, 4, 5], 5, 0)").equals("4");
	code("Array.search([1, 2, 3, 10, true, 'yo', null], 3, 3)").equals("-1");
	code("[1, 2, 3, 10, true, 'yo', null].search('yo', 0)").equals("5");
	code("Array.search([1, 2, 3, 10, true, 'yo', null], false, 0)").equals("-1");
	code("[null].search(null, 0)").equals("0");
	code("[100, 125, 112].search(12, 0)").equals("-1");

	section("Array.subArray()");
	code("Array.subArray([1, 2, 3, 10, true, 'yo', null], 3, 5)").equals("[10, true, 'yo']");
	code("Array.subArray([1, 2, 3, 10, true, 'yo', null], 3, 1)").equals("[]");
	code("Array.subArray([1, 2, 3, 10, true, 'yo', null], 0, 100)").equals("[1, 2, 3, 10, true, 'yo', null]");
	code("Array.subArray([1, 2, 3, 10, true, 'yo', null], 1, 1)").equals("[2]");

	section("Array.pop()");
	code("[].pop()").exception(ls::vm::Exception::ARRAY_OUT_OF_BOUNDS);
	code("Array.pop(['1', '2'])").equals("'2'");
	code("['1', '2'].pop()").equals("'2'");
	code("var a = ['1', '2', '3'] a.pop() a").equals("['1', '2']");
	code("[1, 2].pop()").equals("2");
	code("[1.9, 2.78].pop()").equals("2.78");
	code("var a = [1, 2] a.pop()").equals("2");
	code("var a = [1.9, 2.78] a.pop()").equals("2.78");
	code("[12].clear().pop()").exception(ls::vm::Exception::ARRAY_OUT_OF_BOUNDS);
	code("[12.5].clear().pop()").exception(ls::vm::Exception::ARRAY_OUT_OF_BOUNDS);

	section("Array.push()");
	code("var a = [1, 2, 3] Array.push(a, 4)").equals("[1, 2, 3, 4]");
	code("[].push([])").equals("[[]]");
	code("var a = [1, 2] a.push(3) a").equals("[1, 2, 3]");
	code("var a = [1, 2] a.push(3.5) a").equals("[1, 2, 3.5]");
	code("var a = [1.5, -2.9] a.push(3.5) a").equals("[1.5, -2.9, 3.5]");
	code("var s = new Array() Array.push(s, 'a')").equals("['a']");
	code("var a = [] a.push(1) a").equals("[1]");
	code("var a = [] a.push(1)").equals("[1]");
	code("var a = [] a.push('a') a").equals("['a']");
	code("var a = [] a.push('a')").equals("['a']");
	code("var a = [1] a.push('a') a").equals("[1, 'a']");
	code("var a = [1] a.push('a')").equals("[1, 'a']");
	code("var a = [] for i in [1..10] { a = a.push(i) } a").equals("[1, 2, 3, 4, 5, 6, 7, 8, 9, 10]");
	code("let p = (a, e) => a.push(e) var a = [] for i in [1..10] { a = p(a, i) } a").equals("[1, 2, 3, 4, 5, 6, 7, 8, 9, 10]");

	section("Array.pushAll()");
	code("Array.pushAll([], [true, 'yo'])").equals("[true, 'yo']");
	code("[0].pushAll([1, 2, 3])").equals("[0, 1, 2, 3]");
	code("[0].pushAll([3.5])").equals("[0, 3.5]");
	code("[0.5].pushAll(['a'])").equals("[0.5, 'a']");

	section("Array.join()");
	code("[].join('a')").equals("''");
	code("['salut', 'ça', 'va'].join(' ')").equals("'salut ça va'");
	code("[1, null, 'va'].join(' ')").equals("'1 null va'");
	code("[-14, 21, -45, 5].join(' ')").equals("'-14 21 -45 5'");
	code("[-14.67, 21.05, -45, 5.81].join(' ')").equals("'-14.67 21.05 -45 5.81'");
	code("[12].clear().join('whatever')").equals("''");

	section("Array.clear()");
	code("var a = [1, 2, 3] a.clear() a").equals("[]");
	code("var a = [1, 2, 3] Array.clear(a)").equals("[]");
	code("var a = [1, 'yo', true] a.clear() a").equals("[]");

	section("Array.fill()");
	code("var a = [] a.fill('lol') a").equals("[]");
	code("var a = [1, 2, 3] a.fill(12, 4) a").equals("[12, 12, 12, 12]");
	code("var a = [1, 2, 3] a.fill(12) a").equals("[12, 12, 12]");
	code("var a = [1, 2, 3] a.fill(12.5, 4) a").equals("[12.5, 12.5, 12.5, 12.5]");
	code("var a = [1, 2, 3] a.fill(12.5) a").equals("[12.5, 12.5, 12.5]");
	code("var a = [] Array.fill(a, 'test', 2)").equals("['test', 'test']");
	code("var a = [1.5] a.fill(12.5, 4) a").equals("[12.5, 12.5, 12.5, 12.5]");
	code("var a = [1.5] a.fill(12, 4) a").equals("[12, 12, 12, 12]");
	code("var a = [] a.fill('test', true) a").equals("['test']");
	code("var a = [1, 2, 3] a.fill('a', 4) a").equals("['a', 'a', 'a', 'a']");
	code("var a = [1, 2, 3] a.fill('a') a").equals("['a', 'a', 'a']");
	code("var a = ['a', 'b', 'c'] a.fill(true, 4) a").equals("[true, true, true, true]");
	code("var a = ['a', 'b', 'c'] a.fill(true) a").equals("[true, true, true]");
	code("var a = ['a', 'b', 'c'] a.fill(1, 3) a").equals("[1, 1, 1]");

	section("fill() v1");
	code_v1("var a = ['a', 'b'] fill(a, 'hello') a").equals("['hello', 'hello']");
	code_v1("var a = [] fill(a, 'hello', 5) a").equals("['hello', 'hello', 'hello', 'hello', 'hello']");

	section("Array.insert()");
	code("var a = ['a', 'b', 'c'] Array.insert(a, 'hello', 1)").equals("['a', 'hello', 'b', 'c']");
	code("var a = ['a', 'b', 'c'] Array.insert(a, 'hello', 6)").equals("['a', 'b', 'c', null, null, null, 'hello']");
	code("var a = [1, 2, 3] Array.insert(a, 12, 1)").equals("[1, 12, 2, 3]");
	code("var a = [1, 2, 3] Array.insert(a, 12, 6)").equals("[1, 2, 3, 0, 0, 0, 12]");

	section("Array.isPermutation()");
	code("[].isPermutation([])").equals("true");
	code("[1].isPermutation([1])").equals("true");
	code("[1, 2, 3, 4, 5].isPermutation([4, 1, 3, 5, 2])").equals("true");
	code("[1, 2, 3, 4, 5].isPermutation([4, 1, 3, 5, 3])").equals("false");
	code("[1, 2, 3, 4, 5].isPermutation([1, 2, 3, 4, 5, 6])").equals("false");
	code("[1, 2, 3.2, 4, 5.5].isPermutation([3.2, 4, 5.5, 2, 1])").equals("true");
	code("['a'].isPermutation(['a'])").equals("true");
	code("['a', 'b'].isPermutation(['b', 'a'])").equals("true");
	code("['a', 'b', 'c', 'd', 'e'].isPermutation(['d', 'a', 'c', 'e', 'b'])").equals("true");
	code("['a', 'b', 'c', 'd', 'e'].isPermutation(['d', 'a', 'c', 'd', 'c'])").equals("false");
	code("var a = [1, 2, 3, 'a'] a.pop() a.isPermutation([3, 2, 1])").equals("true");
	code("var a = [1, 2, 3.5, 'a'] a.pop() a.isPermutation([3.5, 2, 1])").equals("true");
	code("var a = [1, 2, 3, 4.5] a.pop() a.isPermutation([3, 2, 1])").equals("true");
	code("var a = [1, 2, 3, 'a'] a.pop() [2, 1, 3].isPermutation(a)").equals("true");
	code("var a = [1, 2, 3.5, 'a'] a.pop() [2, 1, 3.5].isPermutation(a)").equals("true");
	code("var a = [1, 2, 3, 4.5] a.pop() [3, 1, 2].isPermutation(a)").equals("true");

	section("Array.random()");
	code("[].random(1)").equals("[]");
	code("[1, 2, 3].random(3).size()").equals("3");
	code("[1, 2, 3].random(3).sum()").equals("6");
	code("[1, 2, 3].random(0)").equals("[]");
	code("[1, 2, 3].random(-10)").equals("[]");
	code("['a', 'b', 'c'].random(2).size()").equals("2");
	code("[1, 2, 3].random(1000).size()").equals("3");
	code("['a', 'b', 'c'].random(0)").equals("[]");
	code("let a = ['a', 'b', 'c'] a.random(0)").equals("[]");
	code("let a = ['a', 'b', 'c'] a.random(1).size()").equals("1");
	code("let a = ['a', 'b', 'c'] a.random(3).size()").equals("3");

	section("Array.remove()");
	code("var a = [1, 2, 3] Array.remove(a, 1)").equals("2");
	code("var a = [1, 2, 3] Array.remove(a, 1) a").equals("[1, 3]");
	code("var a = [1, 2, 3] Array.remove(a, 1)").equals("2");
	code("var a = [1, 'yo', true] Array.remove(a, 1)").equals("'yo'");

	section("Array.removeElement()");
	code("[].removeElement(12)").equals("false");
	code("var a = [1, 2, 3] a.removeElement(1) a").equals("[3, 2]");
	code("var a = [1, 2, 3] a.removeElement('key') a").equals("[1, 2, 3]");
	code("var a = [1, 2, 3] a.removeElement('key')").equals("false");
	code("[1, 2, 3].removeElement(3)").equals("true");
	code("[1, 2, 3].removeElement(4)").equals("false");
	code("[1, 2, 3].removeElement('key')").equals("false");
	code("var a = [true, 'hello', [1, 2, 3]] a.removeElement([1, 2, 3]) a").equals("[true, 'hello']");
	code("[true, 'hello', [1, 2, 3]].removeElement('hello')").equals("true");
	code("[true, 'hello', [1, 2, 3]].removeElement('yolo')").equals("false");

	section("inArray");
	code("inArray([1, 2, 3, 4], 3)").equals("true");
	code("inArray([1, 2, 3, 4], 5)").equals("false");
	code("inArray([1.5, 2.2, 3.7, 4.6], 2.2)").equals("true");
	code("inArray([1.5, 2.2, 3.7, 4.6], 2.1)").equals("false");
	code("inArray(['a', 'b', 'c', 'd'], 'd')").equals("true");
	code("inArray(['a', 'b', 'c', 'd'], 'e')").equals("false");

	section("Postfix expressions");
	code("let a = [10, ''] a[0]++").equals("10");
	code("let a = [10, ''] a[0]++ a[0]").equals("11");
	code("let a = [10, ''] a[0]--").equals("10");
	code("let a = [10, ''] a[0]-- a[0]").equals("9");
	code("var v = 10 [v++, '']").equals("[10, '']");
	code("var v = 90 [v--, '']").equals("[90, '']");

	section("Range access");
	code("[1 2 3 4 5 6 7 8][0:0]").equals("[1]");
	code("[1 2 3 4 5 6 7 8][0:1]").equals("[1, 2]");
	code("[1 2 3 4 5 6 7 8][3:5]").equals("[4, 5, 6]");
	// TODO add more

	section("Misc");
	code("let a = [1, []] a[1].push('str') a").equals("[1, ['str']]");
	code("let a = ['hello', 1] a[0].reverse()").equals("'olleh'");
	code("let a = [['a', 'b', 'c'], 1] a[0].reverse()").equals("['c', 'b', 'a']");
	code("let a = [['a', 'b', 'c'], 'hello'] [a[0].reverse(), a[1].reverse()]").equals("[['c', 'b', 'a'], 'olleh']");
	code("let h = [1, 'text', [1,2,3], x -> x + 1] h[2].push('test') h[0] = [h[3](h[0]), h[3](h[1])] h").equals("[[2, 'text1'], 'text', [1, 2, 3, 'test'], <function>]");

	section("Array v1 pushAll");
	code_v1("var a = [] pushAll(a, ['a', 'b', 'c']) a").equals("['a', 'b', 'c']");

	section("Array v1 count");
	code_v1("var a = ['a', 'b', 'c'] count(a)").equals("3");

	/*
	[1, 2, 3] ~~= (x -> x * 5 + 2)
	[1, 2, 3] ~+= 1   =>   [2, 3, 4]
	[1, 2, 3] ~*= 5
	[1, 2, 3] ~= (x -> x * 5)
	a[10:*]
	*/
}
