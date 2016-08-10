#include "Test.hpp"

void Test::test_arrays() {

	header("Arrays");

	success("[]", "array[]");
	success("[1]", "array[1]");
	success("[1, 2, 3]", "array[1, 2, 3]");
	success("[1.21, -5, 4.55, 12, -6.7]", "array[1.21, -5, 4.55, 12, -6.7]");
	success("[true, false, true]", "array[true, false, true]");
	success("[23, true, '', {}, 123]", "array[23, true, '', {}, 123]");
	success("[1, 2, 3] + [4, 5, 6]", "array[1, 2, 3, 4, 5, 6]");
	success("[] + 1", "array[1]");
	success("[] + 1 + 2 + 3", "array[1, 2, 3]");
	success("[1] + [2] + [3]", "array[1, 2, 3]");
	success("[1, 2, 3][1]", "2");
	success("let a = [1, 2, 3] a[0]", "1");
	success("let a = [1, 2, 3] a[0] = 5 a[0]", "5");
	success("let a = [1, 2, 3] a[0] += 5 a[0]", "6");
	success("let v = 12 let a = [v, 2, 3] a[0] += 5 a[0]", "17");
	success("let a = [23, 23, true, '', [], 123]; |a|", "6");
	success("let a = [1, 2, 3]; ~a", "array[3, 2, 1]");
	success("let a = [1, 2, 3] a[1] = 12 a", "array[1, 12, 3]");
	success("[1.2, 321.42, 23.15]", "array[1.2, 321.42, 23.15]");
	success("[1, 2, 3, 4, 5][1:3]", "array[2, 3, 4]");
	success("2 in [1, 2, 3]", "true");
	success("4 in [1, 2, 3]", "false");
	success("'yo' in ['ya', 'yu', 'yo']", "true");
	success("let a = 2 if (a in [1, 2, 3]) { 'ok' } else { 'no' }", "'ok'");
	success("[1 2 3]", "array[1, 2, 3]");
	success("['yo' 'ya' 'yu']", "array['yo', 'ya', 'yu']");
	success("[true false true true]", "array[true, false, true, true]");
	success("[1,2,3,4] < [1,2,3,5]", "true");
	success("[1,2,4,4] < [1,2,3,5]", "false");
	success("[1,2,3,4.1] < [1,2,3,5.1]", "true");
	success("[1,2,4,4.1] < [1,2,3,5.1]", "false");
	success("['1','2','3','4'] < ['1','2','3','5']", "true");
	success("['1','2','4','4'] < ['1','2','3','5']", "false");
	success("[1,2,3,4] < [1,2,3,5.1]", "true");
	success("[1,2,4,4.1] < [1,2,3,5]", "false");
	success("[1,2,'3'] < [1,2,3]", "false");
	success("[1,1,'3'] < [1,2,3]", "true");


	/*
	 * Array operations
	 */
	header("Array operations");
	success("[1, 2, 3, 4, 5] ~~ x -> x ** 2", "array[1, 4, 9, 16, 25]");
	success("[1.5, 2.5, 3.5] ~~ x -> x.floor()", "array[1, 2, 3]");
	success("[1, 2, 3, 4, 5] ~~ (x -> x ** 2)", "array[1, 4, 9, 16, 25]");
	success("['yo', 'toto', 'salut'] ~~ x -> x + ' !'", "array['yo !', 'toto !', 'salut !']");
	success("[1, 2, 3] ~~ x -> [x]", "array[array[1], array[2], array[3]]");
	success("[1, 2, 3] ~~ x -> 'yo'", "array['yo', 'yo', 'yo']");
	success("let f = x -> x * 10 [1, 2, 3] ~~ f", "array[10, 20, 30]");
	success("[1.2, 321.42] ~~ x -> x * 1.7", "array[2.04, 546.414]");
	success("[1, 2, 3, 4, 5] ~~ x -> x.max(3)", "array[3, 3, 3, 4, 5]");
	success("[1, 2, 3, 4, 5, 6, 7, 8, 9, 10] ~~ x -> x.max(3).min(8)", "array[3, 3, 3, 4, 5, 6, 7, 8, 8, 8]");
	success("[1] < [1, 2]", "true");

//	let f=x->x[0]; [f([1]), f([0..3])]

	/*
	 * Intervals
	 */
	header("Intervals");
	success("[1..10]", "array[1..10]");
	success("145 in [1..1000]", "true");
	success("1 in [1..1000]", "true");
	success("1000 in [1..1000]", "true");
	success("0 in [1..1000]", "false");
	success("1001 in [1..1000]", "false");
	success("[0..1000][500]", "500");
	success("[1..1000][500]", "501");
	success("[1000..2000][12]", "1012");
	sem_err("[1..10]['hello']", ls::SemanticException::Type::ARRAY_ACCESS_KEY_MUST_BE_NUMBER, "<key 1>");
	// success("[1..10][-10]", ""); must throw exception

	/*
	 * Array standard library
	 */
	header("Array standard library");
	success("Array", "<class Array>");
	success("Array()", "array[]");
	success("new Array", "array[]");
	success("new Array()", "array[]");

	success("Array.size([1, 'yo', true])", "3");
	success("Array.size([6, 4, 3, 2, -11])", "5");
	success("[1, 'yo', true].size()", "3");
	success("[1, -2, 3, -12, -6].size()", "5");

	success("Array.average([1, 2, 3, 4, 5, 6])", "3.5");
	success("Array.average([])", "0");
	success("[1, 2, 3, 4, 5, 6].average()", "3.5");
	success("[].average()", "0");
	success("['salut', true, []].average()", "0");
	success_almost("[10, -5.7, 30.89, 66].average()", 25.2975);

	success("Array.sum([10, -5, 30, 66])", "101");
	success("[10, -5, 30, 66].sum()", "101");
	success("Array.sum(['ab', 'cd', 'ef'])", "'abcdef'");
	success("['ab', 'cd', 'ef'].sum()", "'abcdef'");
	success("['abc', true, 12, [1, 2]].sum()", "'abctrue12<array>'");
	success_almost("[10, -5.7, 30.89, 66].sum()", 101.19);

	success("Array.map([1, 2, 3], x -> x ** 2)", "array[1, 4, 9]");
	success("[3, 4, 5].map(x -> x ** 2)", "array[9, 16, 25]");
	success("[321, 213, 121].map(x -> x ** 2).size()", "3");
	success("['a' 'b' 'c'].map(x -> x)", "array['a', 'b', 'c']");
	success("[65 66 67].map(x -> x.char()).join('')", "'ABC'");

	success("Array.map2([1, 'yo ', []], [12, 55, 9], (x, y -> x + y))", "array[13, 'yo 55', array[9]]");

	success("let x = [1, 2, 3, 4] x.chunk(2)", "array[array[1, 2], array[3, 4]]");
	success("let x = [1, 2, 3, 4] x.chunk(3)", "array[array[1, 2, 3], array[4]]");
	success("let x = [1, 2, 3, 4] x.chunk()", "array[array[1], array[2], array[3], array[4]]");

	success("let x = [1, 1, 2, 2, 1] x.unique() x", "array[1, 2, 1]");
	success("let x = [1, 1, 2, 2, 1] x.sort().unique() x", "array[1, 2]");
	success("let x = ['a', 'a', 'b'] x.unique() x", "array['a', 'b']");
	success("let x = ['a', 'b', 'c'] x.unique() x", "array['a', 'b', 'c']");
	success("let x = ['a', 'a', 'b', 'a', 'a'] x.unique() x", "array['a', 'b', 'a']");

	success("let x = [3, 1, 2] x.sort() x", "array[1, 2, 3]");
	success("let x = ['foo', 'yop', 'abc'] x.sort() x", "array['abc', 'foo', 'yop']");
	success("let x = [[[]], [[], [], []], [[], []]]; x.sort() x", "array[array[array[]], array[array[], array[]], array[array[], array[], array[]]]");
	success("let x = [[1, 2, 3], [3, 1, 2], [2, 3, 1]]; x.sort() x", "array[array[1, 2, 3], array[2, 3, 1], array[3, 1, 2]]");

	success("Array.filter([1, 2, 3, 10, true, 'yo'], x -> x > 2)", "array[3, 10, 'yo']");
	success("[3, 4, 5].filter(x -> x > 6)", "array[]");

	success("Array.contains([1, 2, 3, 10, 1], 1)", "true");
	success("[3, 4, 5].contains(6)", "false");

	success("Array.isEmpty([])", "true");
	success("[3, 4, 5].isEmpty()", "false");
	//success("let a = 0 Array.iter([1,2,3], x -> a += x) a", "6");
	//success("let a = 2 [1,2,3].iter(x -> a *= x) a", "12");

	success("Array.partition([1, 2, 3, 4, 5], (x -> x < 3))", "array[array[1, 2], array[3, 4, 5]]");
	success("Array.partition([1, 2, 3, 10, true, 'yo'], x -> x > 2)", "array[array[3, 10, 'yo'], array[1, 2, true]]");
	success("[1, 2, 3, 4, 5].partition(x -> x > 3)", "array[array[4, 5], array[1, 2, 3]]");
	success("[1, 2, 3, 4, 5].partition(x -> x == 3)", "array[array[3], array[1, 2, 4, 5]]");
	success("[1, 2, 3, 4, 5, 6].filter(x -> x > 2).partition(x -> x > 4)", "array[array[5, 6], array[3, 4]]");
	// success("[1, 2, 3, 4, 5].partition(x -> 'yolo')", "**error**");

	success("Array.first([1, 2, 3, 10, true, 'yo', null])", "1");
	success("['yo', 3, 4, 5].first()", "'yo'");
	success("Array.last([1, 2, 3, 10, true, 'yo', null])", "null");
	// success("['yo', 3, 4, 5].last()", "5");

	// TODO : the return type of first() must be the element type of the array if it's homogeneous
//	success("[[321, 21], [23, 212], [654, 9876]].first().last()", "21");

	success("Array.foldLeft([1, 2, 3, 10, true, 'yo', null], (x, y -> x + y), 'concat:')", "'concat:12310trueyonull'");
	success("Array.foldRight([1, 2, 3, 10, true, 'yo', null], (x, y -> x + y), 'concat:')", "16");

//	success("Array.shuffle([1, 2, 3, 10, true, 'yo', null])", "test shuffle ?");

	success("Array.reverse([1, 2, 3, 10, true, 'yo', null])", "array[null, 'yo', true, 10, 3, 2, 1]");
	success("[null].reverse()", "array[null]");
	success("[].reverse()", "array[]");
	success("[1, 2, 3].reverse()", "array[3, 2, 1]");
	success("Array.search([1, 2, 3, 10, true, 'yo', null], 10, 0)", "3");

	success("Array.search([1, 2, 3, 4, 5], 5, 0)", "4");
	success("Array.search([1, 2, 3, 10, true, 'yo', null], 3, 3)", "-1");
	success("[1, 2, 3, 10, true, 'yo', null].search('yo', 0)", "5");
	success("Array.search([1, 2, 3, 10, true, 'yo', null], false, 0)", "-1");
	success("[null].search(null, 0)", "0");

	success("Array.subArray([1, 2, 3, 10, true, 'yo', null], 3, 5)", "array[10, true, 'yo']");
	success("Array.subArray([1, 2, 3, 10, true, 'yo', null], 3, 1)", "array[]");
	success("Array.subArray([1, 2, 3, 10, true, 'yo', null], 0, 100)", "array[1, 2, 3, 10, true, 'yo', null]");
	success("Array.subArray([1, 2, 3, 10, true, 'yo', null], 1, 1)", "array[2]");

	success("[].pop()", "null");
	success("Array.pop(['1', '2'])", "'2'");
	success("['1', '2'].pop()", "'2'");
	success("let a = ['1', '2', '3'] a.pop() a", "array['1', '2']");

	success("let a = [1, 2, 3] Array.push(a, 4)", "array[1, 2, 3, 4]");
	success("[].push([])", "array[array[]]");
	success("[0].pushAll([1, 2, 3])", "array[0, 1, 2, 3]");
	success("let a = [1, 2] a.push(3) a", "array[1, 2, 3]");
//	success("let a = [1, 2] a.push(3.5) a", "array[1, 2, 3.5]");
	success("let a = [1.5, -2.9] a.push(3.5) a", "array[1.5, -2.9, 3.5]");

	success("Array.concat([], [true, 'yo'])", "array[true, 'yo']");

	success("[].join('a')", "''");
	success("['salut', 'ça', 'va'].join(' ')", "'salut ça va'");
	success("[1, null, 'va'].join(' ')", "'1 null va'");

	success("let a = [1, 2, 3] a.clear() a", "array[]");
	success("let a = [1, 2, 3] Array.clear(a)", "array[]");

	success("let a = [1, 2, 3] a.fill(12, 4) a", "array[12, 12, 12, 12]");
	success("let a = [] Array.fill(a, 'test', 2)", "array['test', 'test']");

	success("let a = [1, 2, 3] Array.insert(a, 12, 1)", "array[1, 12, 2, 3]");
	success("let a = [1, 2, 3] Array.insert(a, 12, 6)", "array[1, 2, 3, 0, 0, 0, 12]");

	success("let a = [1, 2, 3] Array.remove(a, 1)", "2");
	success("let a = [1, 2, 3] Array.remove(a, 1) a", "array[1, 3]");
//	success("let a = [1, 2, 3] Array.remove(a, 1) a", "array[0: 1, 2: 3]");
	success("let a = [1, 2, 3] Array.remove(a, 1)", "2");

//	success("let a = [] Array.removeKey(a, 'key')", "null");
//	success("let a = [1, 2, 3] a.insert('test', 'key') a.removeKey('key')", "'test'");
//	success("let a = [1, 2, 3] a.insert('test', 'key') a.removeKey('key') a", "array[0: 1, 1: 2, 2: 3]");

	success("let a = [1, 2, 3] a.removeElement(1) a", "array[2, 3]");
	success("let a = [1, 2, 3] a.removeElement('key') a", "array[1, 2, 3]");

	/*
	success("3 ~ x -> x ^ x", "27");
	success("[1, 2, 3] ~ x -> x + 4", "array[1, 2, 3, 4]");
	success("[1, 2, 3] ~~ print", "array[null, null, null]");
	success("[1, 2, 3] ~~ x -> [x, x]", "array[[1, 1], [2, 2], [3, 3]]");
	success("let s = 0 let a = [1, 2, 3, 4] let f = x -> s += x a ~~ f s", "10");
	success("let s = 0 [1, 2, 3, 4, 5].each(x -> s += x)", "15");
	success("[1, 2, 3, 4, 5].fold_left(+,0)", "15");
	//success("let f = x -> x + 10 let g = x -> x ^ 2 [1, 2, 3] ~~ f ~~ g", "array[121, 144, 169]");
	//success("[1, 2, 3] ~~ (x -> x * 5) ~~ (x -> x - 1)", "array[4, 9, 14]");
	*/

	/*
	[1, 2, 3] ~~= (x -> x * 5 + 2)
	[1, 2, 3] ~+= 1   =>   [2, 3, 4]
	[1, 2, 3] ~*= 5
	[1, 2, 3] ~= (x -> x * 5)
	*/

	// let a = [for let i = 0; i < 100; i++ do i end]
	// a[10:*]
	// a[] = 12 (a += 12)
}
