#include "Test.hpp"
#include "../src/type/Type.hpp"

void Test::test_intervals() {
	auto& env = getEnv(false);

	header("Intervals");
	code("[1..10]").equals("[1..10]");
	code("[1..Number.sqrt(1989)]").equals("[1..44]");
	code("[5..5]").equals("[5]");
	code("[-100..0]").equals("[-100..0]");
	code("[0..44.523]").equals("[0..44]");
	code("[0l..544l]").equals("[0..544]");
	code("[45.73..248l]").equals("[45..248]");
	code("[15m..50m]").equals("[15..50]");
	code("[-50m..-15m]").equals("[-50..-15]");
	code("[0..15m]").equals("[0..15]");
	// TODO Backlog Interval with mpz bounds
	// code("[0..100000000000000m]").equals("[0..100000000000000]");

	section("Interval.operator !");
	code("![1..1000]").equals("false");
	code("![1..0]").equals("true");

	section("Interval.operator to_bool");
	code("[1..1000] ? 2 : 5").equals("2");
	code("[1..0] ? 2 : 5").equals("5");

	section("Interval.operator in");
	code("145 in [1..1000]").equals("true");
	code("1 in [1..1000]").equals("true");
	code("1000 in [1..1000]").equals("true");
	code("0 in [1..1000]").equals("false");
	code("1001 in [1..1000]").equals("false");
	code("'a' in [[1..1000], ''][0]").equals("false");
	code("500 in [[1..1000], ''][0]").equals("true");
	code("1500 in [[1..1000], ''][0]").equals("false");
	code("500$ in [[1..1000], ''][0]").equals("true");

	section("Interval.operator ==");
	code("[1..1000] == [1..1000]").equals("true");
	code("[1..1000] == [1..500]").equals("false");
	code("[1..1000] == 12").equals("false");
	code("[1..10] == [1 2 3 4 5 6 7 8 9 10]").equals("true");
	code("[1..10] == [1 2 3 4 5 6 7 8 9 10 11]").equals("false");
	code("[1..10] == [1 2 3 4 5 6 7 8 9]").equals("false");
	code("[1..3] == [1 2 3]").equals("true");
	code("[1..3] == ['a', 'b', 'c']").equals("false");

	section("Interval.operator []");
	code("[0..1000][500]").equals("500");
	code("[1..1000][500]").equals("501");
	code("[1000..2000][12]").equals("1012");
	code("[-100..0][5]").equals("-95");
	code("['', [10..20]][1][5]").equals("15");
	code("['', [10..20]][1][50]").exception(ls::vm::Exception::ARRAY_OUT_OF_BOUNDS);
	code("[1..10]['hello']").error( ls::Error::Type::ARRAY_ACCESS_KEY_MUST_BE_NUMBER, {"'hello'", "[1..10]", env.tmp_string->to_string()});
	code("let i = ['', [10..20]][1] i['hello']").exception(ls::vm::Exception::ARRAY_KEY_IS_NOT_NUMBER);
	code("[1..10][50]").exception(ls::vm::Exception::ARRAY_OUT_OF_BOUNDS);
	code("[1..10][-10]").exception(ls::vm::Exception::ARRAY_OUT_OF_BOUNDS);
	code("['', [1..10][5]]").equals("['', 6]");
	code("let a = [1..10] a[5] = 2").exception(ls::vm::Exception::NO_SUCH_OPERATOR);

	section("Interval.operator [:]");
	code("[5..5][0:0]").equals("[5]");
	code("[1..10][3:5]").equals("[4..6]");
	code("[1..10000000][500000:600000]").equals("[500001..600001]");
	code("[1..10][100:200]").exception(ls::vm::Exception::ARRAY_OUT_OF_BOUNDS);
	code("[1..10][-1:5]").exception(ls::vm::Exception::ARRAY_OUT_OF_BOUNDS);
	code("[1..10][20:5]").exception(ls::vm::Exception::ARRAY_OUT_OF_BOUNDS);
	code("[1..10][5:20]").exception(ls::vm::Exception::ARRAY_OUT_OF_BOUNDS);

	section("|Interval|");
	code("|[0..1]|").equals("2");
	code("|[1..100]|").equals("100");

	section("Interval.operator ~~");
	code("[1..10] ~~ x -> x ** 2").equals("[1, 4, 9, 16, 25, 36, 49, 64, 81, 100]");
	code("[1..5] ~~ x -> x.sqrt()").equals("[1, 1.41421, 1.73205, 2, 2.23607]");
	code("[1..10] ~~ x -> x.isPrime()").equals("[false, true, true, false, true, false, true, false, false, false]");
	code("[-10..-5] ~~ x -> x + '!'").equals("['-10!', '-9!', '-8!', '-7!', '-6!', '-5!']");
	code("[15..18] ~~ (x -> x ** 2) ~~ x -> x.sqrt()").equals("[15, 16, 17, 18]");
	code("[1..10] ~~ Number.isPrime").equals("[false, true, true, false, true, false, true, false, false, false]");

	section("Iteration");
	code("for k, i in [1..0] { System.print(k + ' => ' + i) }").output("");
	code("for k, i in [1..1] { System.print(k + ' => ' + i) }").output("0 => 1\n");
	code("for k, i in [5..7] { System.print(k + ' => ' + i) }").output("0 => 5\n1 => 6\n2 => 7\n");
	code("var s = 0 for i in [100..110] { s += i } s").equals("1155");
	code("var s = 0 for k, i in [100..110] { s += k * i } s").equals("5885");
	code("var s = 0l for i in [0..1000] { s += i ** 2 } s").equals("333833500");

	/*
	 * Methods
	 */
	section("Interval.filter()");
	code("[1..6].filter(x -> x > 3)").equals("[4, 5, 6]");
	code("[1..6].filter(x -> x < 3)").equals("[1, 2]");
	code("[1..100].filter(x -> x > 50 and x < 53)").equals("[51, 52]");
	code("[1..Number.sqrt(1989)].filter(x -> 1989 % x == 0)").equals("[1, 3, 9, 13, 17, 39]");
	code("[1..1989.sqrt()].filter(x -> !(1989 % x)).max()").equals("39");
	// code("let divisors = n -> [1..n.sqrt()].filter(x -> !(n % x)) divisors(1989)").equals("[1, 3, 9, 13, 17, 39]");
	code("[0..10].filter(-> true)").equals("[0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10]");
	code("[-5..5].filter(-> true)").equals("[-5, -4, -3, -2, -1, 0, 1, 2, 3, 4, 5]");
	code("[1..10000].filter(x -> x.isPrime() && (x + 256).isPrime()).size()").equals("197");
	code("[1..1000000].filter(x -> x.isPrime()).size()").equals("78498");
	code("[1..10000].filter(isPrime).size()").equals("1229");

	section("Interval.map");
	code("[1..10].map(x -> x + 5)").equals("[6, 7, 8, 9, 10, 11, 12, 13, 14, 15]");
	code("[1..10].map(x -> x ** 2)").equals("[1, 4, 9, 16, 25, 36, 49, 64, 81, 100]");
	code("[1..5].map(x -> x.sqrt())").equals("[1, 1.41421, 1.73205, 2, 2.23607]");
	code("[1..10].map(x -> x.isPrime())").equals("[false, true, true, false, true, false, true, false, false, false]");
	code("[-10..-5].map(x -> x + '!')").equals("['-10!', '-9!', '-8!', '-7!', '-6!', '-5!']");
	code("[15..18].map(x -> x ** 2).map(x -> x.sqrt())").equals("[15, 16, 17, 18]");
	// TODO not working anymore because print is compiler method, not direct function
	DISABLED_code("[1..10].map(System.print)").output("1\n2\n3\n4\n5\n6\n7\n8\n9\n10\n");

	section("Interval.sum");
	code("[1..0].sum()").equals("0");
	code("[1..1].sum()").equals("1");
	code("[1..5].sum()").equals("15");
	code("[5..10].sum()").equals("45");
	code("[1..10].sum()").equals("55");
	code("[1..10000].sum()").equals("50005000");
	code("[-100..100].sum()").equals("0");
	code("[-100..0].sum() + [0..200].sum()").equals("15050");
	code("[-100..200].sum()").equals("15050");

	section("Interval.product");
	code("[1..0].product()").equals("1");
	code("[1..1].product()").equals("1");
	code("[6..6].product()").equals("6");
	code("[1..5].product()").equals("120");
	code("[1..10].product()").equals("3628800");
	code("[5..7].product()").equals("210");
	code("[-7..7].product()").equals("0");

	section("Interval clone()");
	code("let i = [1..10] [i]").equals("[[1..10]]");
}
