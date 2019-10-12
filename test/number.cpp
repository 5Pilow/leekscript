#include "Test.hpp"
#include "../src/type/Type.hpp"

void Test::test_numbers() {
	auto& env = getEnv();

	header("Numbers");

	section("Basic numbers");
	code("0").equals("0");
	code("-1").equals("-1");
	code("-(-1)").equals("1");
	code("π").almost(3.141592653589793116);

	section("Lexical errors");
	code("12345r").error(ls::Error::Type::NUMBER_INVALID_REPRESENTATION);
	code("0b011001711").error(ls::Error::Type::NUMBER_INVALID_REPRESENTATION);
	code("0b").error(ls::Error::Type::NUMBER_INVALID_REPRESENTATION);
	code("0x").error(ls::Error::Type::NUMBER_INVALID_REPRESENTATION);
	code("0x+").error(ls::Error::Type::NUMBER_INVALID_REPRESENTATION);
	code("0b#").error(ls::Error::Type::NUMBER_INVALID_REPRESENTATION);
	code("0b'").error(ls::Error::Type::NUMBER_INVALID_REPRESENTATION);
	code("0b\"").error(ls::Error::Type::NUMBER_INVALID_REPRESENTATION);

	section("Basic operations");
	code("0 + 5").equals("5");
	code("5 + 5").equals("10");
	code("10 - 3").equals("7");
	code("-2 + 3").equals("1");
	code("5 * 5").equals("25");
	code("15 / 3").equals("5");
	code("15 / 2").equals("7.5");
	code("12 ** 2").equals("144");
	code("2 ** 5").equals("32");
	code("2 < 5").equals("true");
	code("12 < 5").equals("false");
	code("5 == 12").equals("false");
	code("12 == 12").equals("true");
	code("0.2 + 0.1").almost(0.3);
	code("|-12|").equals("12");
	code("-12 * 2").equals("-24");
	code("(-12) * 2").equals("-24");
	code("-12 ** 2").equals("-144");
	code("-12 ** 2").equals("-144");
	code("(-12) ** 2").equals("144");
	code("-12 + 2").equals("-10");
	code("let a = [2, 'a'] [-a[0], +a[0], ~a[0]] == [-2, 2, ~2]").equals("true");

	section("Hexadecimal representation");
	code("0x0").equals("0");
	code("0x00000000").equals("0");
	code("0x1").equals("1");
	code("0x00000001").equals("1");
	code("0xf").equals("15");
	code("0x0000000f").equals("15");
	code("-0xf").equals("-15");
	code("0xff").equals("255");
	code("0x10").equals("16");
	code("-0xffff").equals("-65535");
	code("0xffffffff").equals("4294967295");
	code("0x8fa6cd83e41a6f4ec").equals("165618988158544180460");
	code("-0xa71ed8fa6cd83e41a6f4eaf4ed9dff8cc3ab1e9a4ec6baf1ea77db4fa1c").equals("-72088955549248787618860543269425825306377186794534918826231778059287068");
	code("0xfe54c4ceabf93c4eaeafcde94eba4c79741a7cc8ef43daec6a71ed8fa6cd8b3e41a6f4ea7f4ed9dff8cc3ab61e9a4ec6baf1ea77deb4fa1c").equals("722100440055342029825617696009879717719483550913608718409456486549003139646247155371523487552495527165084677501327990299146441654073884");

	section("Binary representation");
	code("0b0").equals("0");
	code("0b00001").equals("1");
	code("0b1001010110").equals("598");
	code("-0b0101101001111").equals("-2895");
	code("0b010101010101110101010101011111111110111110111110000000011101101010101001").equals("1574698668551521295017");
	code("-0b101010101011101010101010111111111101111101111100000000111011010101010010011111100000011111111111110000").equals("-3381639641241763826573319995376");

	section("null must not be considered as 0");
	code("null == 0").equals("false");
	code("null < 0").equals("true");
	code("null + 5").exception(ls::vm::Exception::NO_SUCH_OPERATOR);
	code("5 + null").exception(ls::vm::Exception::NO_SUCH_OPERATOR);
	code("5 / null").exception(ls::vm::Exception::NO_SUCH_OPERATOR);
	code("null / 12").exception(ls::vm::Exception::NO_SUCH_OPERATOR);
	code("null * 5").exception(ls::vm::Exception::NO_SUCH_OPERATOR);
	code("5 * null").exception(ls::vm::Exception::NO_SUCH_OPERATOR);

	section("Numbers with variables");
	code("var a = 2 a++").equals("2");
	code("var a = 2; ++a").equals("3");
	code("var a = 2 a--").equals("2");
	code("var a = 2; --a").equals("1");
	code("var a = 2 a += 5").equals("7");
	code("var a = 2 a -= 5").equals("-3");
	code("var a = 2 a *= 5").equals("10");
	code("var a = 100 a /= 5").equals("20");
	code("var a = 56 a %= 17").equals("5");
	code("var a = 15 a **= 2").equals("225");
	code("let a = 1.5 a * 0.5").equals("0.75");
	code("var i = 1m i = i + 2m").equals("3");
	code("var a = 10; a += 10 - 2 * 3; a").equals("14");

	section("multiple operations");
	code("(33 - 2) / 2").equals("15.5");
	code("12 < (45 / 4)").equals("false");
	code("12 == (24 / 2)").equals("true");
	code("2.5 + 4.7").almost(7.2);
	code("2.5 × 4.7").equals("11.75");
	code("5 * 2 + 3 * 4").equals("22");

	section("String conversions");
	code("65.char()").equals("'A'");
	code("Number.char(65)").equals("'A'");
	code("126.char()").equals("'~'");
	code("Number.char(128040)").equals("'🐨'");
	code("126.784.char()").equals("'~'");
	code("Number.char([126.784, 'hello'][0])").equals("'~'");
	code("let c = 65 (c.char())").equals("'A'");
	code("let c = 65 (c.char() + '!')").equals("'A!'");
	code("0x2764.char()").equals("'❤'");

	section("Multiple precision numbers");
	code("12344532132423").equals("12344532132423");
	code("var a = 10m a").equals("10");
	code("0m").equals("0");
	code("0xf45eab5c9d13aab44376beff").equals("75628790656539575381594128127");
	// TODO floating-point multiple precision numbers
	// TODO code("123456.78910m").equals("123456.7891");
	// TODO code("123456789123456789123456789.5").equals("");
	// TODO code("1234567891234567891234567891234567891234567891234567891234567891234567891234567891234567891234567891234567891234567891234567891234567891234567891234567891234567891234567891234567891234567891234567891234567891234567891234567891234567883459720303390827584524332795121111123456788999999999999999999999999999999999.5").equals("");
	code("let a = 1209876543789765432456765432087654321 a").equals("1209876543789765432456765432087654321");
	code("let a = { 1209876543789765432456765432087654321 } a").equals("1209876543789765432456765432087654321");
	code("var a = 5m a = 12m").equals("12");
	code("var a = 5m a = 12m a").equals("12");
	code("let f = -> 12m f().string()").equals("'12'");

	section("Integer division by zero");
	code("1 \\ 0").exception(ls::vm::Exception::DIVISION_BY_ZERO);
	code("1 % 0").exception(ls::vm::Exception::DIVISION_BY_ZERO);

	/*
	 * Number standard library
	 */
	header("Number standard library");
	section("Constructor");
	code("Number").equals("<class Number>");
	code("Number()").equals("0");
	code("Number(12)").equals("12");
	code("Number(12.5)").equals("12.5");
	code("Number(12l)").equals("12");
	code("Number(12m)").equals("12");
	code("[Number(), 'str']").equals("[0, 'str']");
	code("new Number").equals("0");
	code("new Number()").equals("0");
	code("new Number(12)").equals("12");
	code("['', new Number()]").equals("['', 0]");
	code("['', new Number]").equals("['', 0]");
	code("['', Number()]").equals("['', 0]");
	code("['', new Number(12)]").equals("['', 12]");
	code("['', Number(12)]").equals("['', 12]");

	section("Constants");
	code("Number.pi").almost(3.141592653589793116);
	code("['', Number.pi]").equals("['', 3.1415926536]");
	code("2 × Number.pi").almost(6.283185307179586232);
	code("Number.e").almost(2.718281828459045091);
	code("Number.phi").almost(1.618033988749894903);
	code("Number.epsilon").almost(0.000000000000000222);
	code("pi").almost(3.141592653589793116);
	code("e").almost(2.718281828459045091);
	code("phi").almost(1.618033988749894903);
	code("epsilon").almost(0.000000000000000222);
	code("let pi = 3 pi").equals("3");
	code("{ let pi = 3 } pi").almost(3.141592653589793116);

	/*
	 * Operators
	 */
	section("Number.operator unary -");
	code("var a = [12, ''] var b = a[0]; -b").equals("-12");
	code("-(12m ** 2)").equals("-144");
	code("-100m").equals("-100");

	section("Number.operator unary !");
	code("var a = [12, ''] var b = a[0]; !b").equals("false");

	section("Number.operator unary ~");
	code("var a = [12, ''] var b = a[0]; ~b").equals("-13");
	code("var a = 12 ['', ~a]").equals("['', -13]");

	section("Number.operator ++x");
	code("var a = 20$; ++a").equals("21");
	code("var a = 30$; ++a a").equals("31");
	code("var a = 20m; ++a").equals("21");
	code("var a = 20m; ++a a").equals("21");
	code("var a = 20m; let b = ++a b").equals("21");
	code("++5").error(ls::Error::Type::VALUE_MUST_BE_A_LVALUE, {"5"});
	code("var a = 5 ['', ++a]").equals("['', 6]");

	section("Number.operator --x");
	code("var a = 20$; --a").equals("19");
	code("var a = 30$; --a a").equals("29");
	code("--5").error(ls::Error::Type::VALUE_MUST_BE_A_LVALUE, {"5"});
	code("var a = 5 ['', --a]").equals("['', 4]");

	section("Number.operator x++");
	code("var a = 20m; a++").equals("20");
	code("var a = 20m; a++ a").equals("21");
	code("var a = 20m; let b = a++ b").equals("20");
	code("5++").error(ls::Error::Type::VALUE_MUST_BE_A_LVALUE, {"5"});

	section("Number.operator x--");
	code("var a = 20m; a--").equals("20");
	code("var a = 20m; a-- a").equals("19");
	code("var a = 20m; let b = a-- b").equals("20");
	code("5--").error(ls::Error::Type::VALUE_MUST_BE_A_LVALUE, {"5"});

	section("Number.operator in");
	// TODO idea : a in b returns true if a is a divisor of b
	code("2 in 12").error(ls::Error::Type::VALUE_MUST_BE_A_CONTAINER, {"12"});

	section("Number.operator =");
	code("var a = 1m, b = 4m; a = b").equals("4");

	section("Number.operator ==");
	code("12m == 12m").equals("true");
	code("13m == 12m").equals("false");
	code("12m ** 5m == 12m ** 5m").equals("true");
	code("12m ** 5m == (3m * 4m) ** 5m").equals("true");
	code("12m ** 5m == 248832").equals("true");
	code("248832 == 12m ** 5m").equals("true");

	section("Number.operator +");
	code("1m + 2m").equals("3");
	code("1m + (2m + 3m)").equals("6");
	code("(1m + 2m) + 3m").equals("6");
	code("(1m + 2m) + (3m + 4m)").equals("10");
	code("15$ + false").equals("15");
	code("15$ + true").equals("16");
	code("let a = 15$ a + true").equals("16");
	code("10000m + 15").equals("10015");
	code("let a = ['a', 12321111111111111111111111111111111321321321999999] a[1] + 123456789").equals("12321111111111111111111111111111111321321445456788");
	code("10000m + (-15)").equals("9985");

	section("Number.operator +=");
	code("var a = 15$ a += true a").equals("16");
	code("var a = 15$ a += []").exception(ls::vm::Exception::NO_SUCH_OPERATOR);
	code("var a = 15$ a += [] a").exception(ls::vm::Exception::NO_SUCH_OPERATOR);
	code("var a = 10m a += 4m").equals("14");
	code("var a = 10m a += 4m a").equals("14");
	code("var a = 15 ['', a += 7]").equals("['', 22]");
	code("var a = 10m a += 5 a").equals("15");
	code("var a = 10m a += 78m a").equals("88");
	code("var a = 10m a += (-6) a").equals("4");

	section("Number.operator -");
	code("1m - 2m").equals("-1");
	code("1m - (2m - 3m)").equals("2");
	code("(1m - 2m) - 3m").equals("-4");
	code("(1m - 2m) - (3m - 4m)").equals("0");
	code("(10m + 10m) - 1").equals("19");
	code("15$ - 3$").equals("12");
	code("1000m - 12").equals("988");
	code("1000m - (-12)").equals("1012");
	code("15$ - false").equals("15");
	code("15$ - true").equals("14");
	code("let a = 15$ a - true").equals("14");
	code("12$ - []").exception(ls::vm::Exception::NO_SUCH_OPERATOR);
	code("let a = 100$ a - 20").equals("80");

	section("Number.operator -=");
	code("var a = 15$ a -= true a").equals("14");
	code("var a = 15$ a -= []").exception(ls::vm::Exception::NO_SUCH_OPERATOR);
	code("var a = 15$ a -= [] a").exception(ls::vm::Exception::NO_SUCH_OPERATOR);
	code("var a = 15 ['', a -= 6]").equals("['', 9]");

	section("Number.operator *");
	code("3m * 4m").equals("12");
	code("10m + 3m * 4m").equals("22");
	code("(5m + 2m) * (16m * 2m)").equals("224");
	code("12$ * false").equals("0");
	code("let a = 13$; a * false").equals("0");
	code("13$ * true").equals("13");
	code("7$ * 2").equals("14");
	code("let a = 6$; a * 3").equals("18");
	code("14$ * []").exception(ls::vm::Exception::NO_SUCH_OPERATOR);
	code("12344532132423m * 987657897613412m").equals("12192174652930109838844857276");
	code("5 * 'yo'").equals("'yoyoyoyoyo'");
	code("50m * 10").equals("500");
	code("50 * 10m").equals("500");
	code("let a = ['a', 12321111111111111111111111111111111321321321999999] a[1] * 123456789").equals("1521124814690000000000000000000000025951877651354934543211");

	section("Number.operator *=");
	code("var a = 15$ a *= true a").equals("15");
	code("var a = 15$ a *= false a").equals("0");
	code("var a = 15$ a *= []").exception(ls::vm::Exception::NO_SUCH_OPERATOR);
	code("var a = 15$ a *= [] a").exception(ls::vm::Exception::NO_SUCH_OPERATOR);
	code("var a = 15; ['', a *= 2]").equals("['', 30]");
	code("var a = 5m a *= 0 a").equals("0");
	code("var a = 5m a *= 12 a").equals("60");
	code("var a = 5m a *= 5m a").equals("25");
	code("var a = 91591785496891278315799124157189514175m a *= 157854689278315792457851475m a").equals("14458192840057923568549758280294876918394393505787702519557158125");
	code("var a = 78m a *= true a").equals("78");

	section("Number.operator **");
	code("14$ ** 3").equals("2744");
	code("14$ ** true").equals("14");
	code("14$ ** false").equals("1");
	code("let a = 14$ a ** false").equals("1");
	code("14$ ** []").exception(ls::vm::Exception::NO_SUCH_OPERATOR);
	code("2 ** 50").equals("-2147483648");
	code("2l ** 50").equals("1125899906842624");
	code("257l ** 20").equals("-9223372036854775808"); // overflow
	code("257m ** 20").equals("1580019571820317063568778786121273112555213952001");
	code("2m ** 50").equals("1125899906842624");
	code("(5m + 2m) ** (16m * 2m)").equals("1104427674243920646305299201");
	code("123m ** 1900").exception(ls::vm::Exception::NUMBER_OVERFLOW);
	code("var s = 0 s = 5 ** 2 s").equals("25");

	section("Number.operator **=");
	code("var a = 5$; a **= 4").equals("625");
	code("var a = 5$; a **= true").equals("5");
	code("var a = 5$; a **= false").equals("1");
	code("var a = 5$; a **= []").exception(ls::vm::Exception::NO_SUCH_OPERATOR);
	code("var a = 5; ['', a **= 4]").equals("['', 625]");

	section("Number.operator %");
	code("721 % 57").equals("37");
	code("721$ % 57$").equals("37");
	code("let a = 721$ a % 57").equals("37");
	code("let a = 721$ a % []").exception(ls::vm::Exception::NO_SUCH_OPERATOR);
	code("721$ % true").equals("0");
	code("721$ % false").exception(ls::vm::Exception::DIVISION_BY_ZERO);
	code("let a = 721$ a % false").exception(ls::vm::Exception::DIVISION_BY_ZERO);
	code("let a = 721$ a % true").equals("0");
	code("123456789123456789m % 234567m").equals("221463");
	code("(12m ** 40m) % 234567m").equals("228798");
	code("100000m % (12m ** 3m)").equals("1504");
	code("(100000m * 10m) % (12m ** 3m)").equals("1216");
	code("['salut', 123][0] % 5").exception(ls::vm::Exception::NO_SUCH_OPERATOR);
	// TODO should have semantic error
	// code("['salut', 'a'][0] % 5").error(ls::Error::NO_SUCH_OPERATOR, {});

	section("Number.operator %=");
	code("var a = 721$ a %= true").equals("0");
	code("var a = 721$ a %= []").exception(ls::vm::Exception::NO_SUCH_OPERATOR);

	section("Number.operator %%");
	code("0 %% 1").equals("0");
	code("2 %% 5").equals("2");
	code("(-2) %% 5").equals("3");
	code("(-12) %% 5").equals("3");
	code("721 %% 57").equals("37");
	code("(-721) %% 57").equals("20");
	code("(-721$) %% 57$").equals("20");

	section("Number.operator %%=");
	code("var a = 0 a %%= 1").equals("0");
	code("var a = 2 a %%= 5").equals("2");
	code("var a = -2 a %%= 5").equals("3");
	code("var a = -12 a %%= 5").equals("3");
	code("var a = 721 a %%= 57").equals("37");
	code("var a = -721 a %%= 57").equals("20");
	code("var a = -721$ a %%= 57$").equals("20");

	section("Number.operator /");
	code("12$ / false").exception(ls::vm::Exception::DIVISION_BY_ZERO);
	code("let a = 13$; a / false").exception(ls::vm::Exception::DIVISION_BY_ZERO);
	code("13$ / true").equals("13");
	code("14$ / 2").equals("7");
	code("let a = 18$; a / 3").equals("6");
	code("14$ / []").exception(ls::vm::Exception::NO_SUCH_OPERATOR);
	code("let a = 17, b = 5 a / b").equals("3.4");

	section("Number.operator /=");
	code("var a = 12$ a /= true a").equals("12");
	code("var a = 12$ a /= false a").equals("nan");
	code("var a = 12$ a /= []").exception(ls::vm::Exception::NO_SUCH_OPERATOR);
	code("var a = 12$ a /= [] a").exception(ls::vm::Exception::NO_SUCH_OPERATOR);
	code("var a = 15; ['', a /= 2]").equals("['', 7.5]");

	section("Number.operator <");
	code("5 < 2").equals("false");
	code("2 < 5").equals("true");
	code("5.1 < 2.1").equals("false");
	code("2.1 < 5.1").equals("true");
	code("3m < 4m").equals("true");
	code("10m < (3m * 4m)").equals("true");
	code("(5m + 5m) < (3m * 4m)").equals("true");
	code("(5m + 5m) < 12m").equals("true");
	code("3m < 4").equals("true");

	section("Number.operator <=");
	code("5 <= 2").equals("false");
	code("2 <= 5").equals("true");
	code("5.1 <= 2.1").equals("false");
	code("2.1 <= 5.1").equals("true");
	code("3 <= 4").equals("true");
	code("3 <= []").equals("true");
	code("3 <= 4$").equals("true");

	section("Number.operator >");
	code("5 > 2").equals("true");
	code("2 > 5").equals("false");
	code("5.1 > 2.1").equals("true");
	code("2.1 > 5.1").equals("false");
	code("12 > 5m").equals("true");
	code("[] > true").equals("true");
	code("-100m > 0").equals("false");

	section("Number.operator >=");
	code("5 >= 2").equals("true");
	code("2 >= 5").equals("false");
	code("5.1 >= 2.1").equals("true");
	code("2.1 >= 5.1").equals("false");

	section("Number.operator \\");
	code("10 \\ 2").equals("5");
	code("10 \\ 4").equals("2");
	code("2432431 \\ 2313").equals("1051");
	code("let a = 420987$ a \\ 546$").equals("771");
	code("420987$ \\ 12").equals("35082");
	code("12345678912345l \\ 1234").equals("10004602035");
	code("12$ \\ false").exception(ls::vm::Exception::DIVISION_BY_ZERO);
	code("let a = 13$; a \\ false").exception(ls::vm::Exception::DIVISION_BY_ZERO);
	code("13$ \\ true").equals("13");
	code("17$ \\ 4").equals("4");
	code("let a = 10.7$; a \\ true").equals("10");
	code("let a = 10$; a \\ 4").equals("2");
	code("14$ \\ []").exception(ls::vm::Exception::NO_SUCH_OPERATOR);
	code("67.89$ \\ 1").equals("67");
	code("['', 10 \\ 2]").equals("['', 5]");
	code("['', 10$ \\ 2]").equals("['', 5]");

	section("Number.operator \\=");
	code("var a = 12 a \\= 5").equals("2");
	code("var a = 12$ a \\= 5").equals("2");
	code("var a = 30$ a \\= 4 a").equals("7");
	code("var a = 12$ a \\= true a").equals("12");
	code("var a = 12$ a \\= false a").exception(ls::vm::Exception::DIVISION_BY_ZERO);
	code("var a = 12$ a \\= []").exception(ls::vm::Exception::NO_SUCH_OPERATOR);
	code("var a = 12$ a \\= [] a").exception(ls::vm::Exception::NO_SUCH_OPERATOR);
	code("var a = 12 ['', a \\= 5]").equals("['', 2]");

	section("Number.operator &");
	code("0 & 0").equals("0");
	code("1 & 0").equals("0");
	code("1 & 1").equals("1");
	code("5 & 12").equals("4");
	code("87619 & 18431").equals("17987");
	code("87619$ & [18431, ''][0]").equals("17987");
	code("var a = 87619 a &= 18431").equals("17987");
	code("var a = 87619 a &= 18431 a").equals("17987");
	code("87619$ & 18431").equals("17987");
	code("87619$ &= 18431").error(ls::Error::VALUE_MUST_BE_A_LVALUE, {"87619"});
	code("var a = 87619$ a &= 18431 a").equals("17987");
	code("[12, 'hello'][1] & 5").exception(ls::vm::Exception::NO_SUCH_OPERATOR);
	code("var a = [12, 'hello'][1] a &= 18431 a").exception(ls::vm::Exception::NO_SUCH_OPERATOR);

	section("Number.operator |");
	code("0 | 0").equals("0");
	code("1 | 0").equals("1");
	code("1 | 1").equals("1");
	code("5 | 12").equals("13");
	code("[5, ''][0] | [12, ''][0]").equals("13");
	code("87619 | 18431").equals("88063");
	code("var a = 87619 a |= 18431").equals("88063");
	code("var a = 87619 a |= 18431 a").equals("88063");
	code("[87619, ''][0] | 18431").equals("88063");
	code("87619$ |= 18431").error(ls::Error::VALUE_MUST_BE_A_LVALUE, {"87619"});
	code("var a = 87619$ a |= 18431 a").equals("88063");
	code("[12, 'hello'][1] | 5").exception(ls::vm::Exception::NO_SUCH_OPERATOR);

	section("Number.operator ^");
	code("0 ^ 0").equals("0");
	code("1 ^ 0").equals("1");
	code("1 ^ 1").equals("0");
	code("5 ^ 12").equals("9");
	code("87619 ^ 18431").equals("70076");
	code("[87619, ''][0] ^ [18431, ''][0]").equals("70076");
	code("var a = 87619 a ^= 18431").equals("70076");
	code("var a = 87619 a ^= 18431 a").equals("70076");
	code("[87619, ''][0] ^ 18431").equals("70076");
	code("87619$ ^= 18431").error(ls::Error::VALUE_MUST_BE_A_LVALUE, {"87619"});
	code("var a = 87619$ a ^= 18431 a").equals("70076");
	code("[12, 'hello'][1] ^ 5").exception(ls::vm::Exception::NO_SUCH_OPERATOR);

	section("Number.operator <<");
	code("0 << 0").equals("0");
	code("1 << 0").equals("1");
	code("123456 << 0").equals("123456");
	code("0 << 1").equals("0");
	code("0 << 12").equals("0");
	code("1 << 8").equals("256");
	code("123 << 12").equals("503808");
	code("[123, ''][0] << 12").equals("503808");
	code("var a = 123 a <<= 11").equals("251904");
	code("var a = 123 a <<= 13 a").equals("1007616");
	code("var a = [123, ''] a[0] <<= 13").equals("1007616");
	code("var a = 123 ['', a <<= 13]").equals("['', 1007616]");
	code("'salut' << 5").exception(ls::vm::Exception::NO_SUCH_OPERATOR);

	section("Number.operator >>");
	code("0 >> 0").equals("0");
	code("1 >> 0").equals("1");
	code("123456 >> 0").equals("123456");
	code("0 >> 1").equals("0");
	code("0 >> 12").equals("0");
	code("155 >> 3").equals("19");
	code("-155 >> 3").equals("-20");
	code("12345 >> 8").equals("48");
	code("123123123 >> 5").equals("3847597");
	code("[123123123, ''][0] >> 5").equals("3847597");
	code("var a = 123123123 a >>= 6").equals("1923798");
	code("var a = 123123123 a >>= 7 a").equals("961899");
	code("var a = [123123123, ''] a[0] >>= 7").equals("961899");
	code("var a = 12345 ['', a >>= 8]").equals("['', 48]");
	code("'salut' >> 5").error(ls::Error::NO_SUCH_OPERATOR, {env.tmp_string->to_string(), ">>", env.integer->to_string()});

	section("Number.operator >>>");
	code("155 >>> 3").equals("19");
	code("-155 >>> 3").equals("536870892");
	code("[-155, ''][0] >>> 3").equals("536870892");
	code("var a = -155 a >>>= 4").equals("268435446");
	code("var a = -155 a >>>= 5 a").equals("134217723");
	code("var a = [-155, ''] a[0] >>>= 5").equals("134217723");
	code("var a = -155 ['', a >>>= 5]").equals("['', 134217723]");
	code("'salut' >>> 5").error(ls::Error::NO_SUCH_OPERATOR, {env.tmp_string->to_string(), ">>>", env.integer->to_string()});

	section("Number.operator |x|");
	code("var a = -12 [] + |a|").equals("[12]");

	/*
	 * Iteration
	 */
	section("Number iterators");
	code("for d in 123456 { System.print(d) }").output("1\n2\n3\n4\n5\n6\n");
	code("for d in -123456 { System.print(d) }").output("");
	code("for d in 0 { System.print(d) }").output("");
	code("var s = 0 for d in 159753 { s += d } s").equals("30");
	code("for d in 0 { System.print(d) }").output("");
	code("var s = 0 for k : d in 987654 { s += k * d } s").equals("80");
	code("var s = 0 for d in 123456789123456789 { s += d } s").equals("90");
	code("var s = 0 for k, d in 123456789123456789 { s += k * d } s").equals("885");
	code("var s = '' for d in 1234567891234567891 { s += d + ' ' } s").equals("'1 2 3 4 5 6 7 8 9 1 2 3 4 5 6 7 8 9 1 '");

	/*
	 * Methods
	 */
	section("Number.abs()");
	code("Number.abs").equals("<function>");
	code("Number.abs(-12)").equals("12");
	code("Number.abs(-16436435l)").equals("16436435");
	code("Number.abs(-12.67)").almost(12.67);
	code("Number.abs(['a', -15][1])").equals("15");
	code("(-17).abs()").equals("17");
	code("(-19.5).abs()").equals("19.5");
	code("12.abs").equals("<function>");
	code("Number.abs([1, 'salut'][1])").exception(ls::vm::Exception::WRONG_ARGUMENT_TYPE);

	section("Number.exp()");
	code("Number.exp(0)").equals("1");
	code("Number.exp(1)").almost(M_E);
	code("Number.exp(4)").almost(54.598150033144236204);
	code("Number.exp(4.89)").almost(132.953574051282743085);
	code("Number.exp(-2.97)").almost(0.051303310331919108);
	code("Number.exp(['a', 7.78][1])").almost(2392.274820537377763685);
	code("0.exp()").equals("1");
	code("1.exp()").almost(M_E);
	code("7.exp()").almost(1096.633158428458500566);
	code("(-7).exp()").almost(0.000911881965554516);
	code("(-3.33).exp()").almost(0.035793105067655297);
	code("Number.e ** 5").almost(148.413159102576571513);

	section("Number.floor()");
	code("Number.floor(5.9)").equals("5");
	code("var a = 5 Number.floor(a)").equals("5");
	code("var a = 5.4 Number.floor(a)").equals("5");
	code("Number.floor(['a', -14.7][1])").equals("-15");
	code("floor(5.5)").equals("5");

	section("Number.round()");
	code("Number.round(5.7)").equals("6");
	code("Number.round(5.4)").equals("5");
	code("Number.round(['a', -15.89][1])").equals("-16");
	code("Number.round(12)").equals("12");
	code("Number.round(-1000)").equals("-1000");

	section("Number.ceil()");
	code("Number.ceil(5.1)").equals("6");
	code("Number.ceil(188)").equals("188");

	section("Number.max()");
	code("Number.max(5, 12)").equals("12");
	code("Number.max(75.7, 12)").almost(75.7);
	code("Number.max(5, 12.451)").almost(12.451);
	code("Number.max([5, 'a'][0], 4)").equals("5");
	code("Number.max([5, 'a'][0], 76)").equals("76");
	code("Number.max(4, [5, 'a'][0])").equals("5");
	code("Number.max(77, [5, 'a'][0])").equals("77");
	code("Number.max([55, 'a'][0], [5, 'a'][0])").equals("55");
	code("Number.max(5, 12.8)").equals("12.8");
	code("Number.max(15.7, 12.8)").equals("15.7");
	code("Number.max([15.7, ''][0], 12.8)").equals("15.7");
	code("Number.max(5.5, [12.8, ''][0])").equals("12.8");
	code("2.max([7.5, ''][0])").equals("7.5");
	code("[2, ''][0].max([7.5, ''][0])").equals("7.5");
	code("2.max([7.5, ''][1])").exception(ls::vm::Exception::WRONG_ARGUMENT_TYPE);
	code("max(5l, 10.5)").equals("10.5");
	code("max(5l, 10)").equals("10");
	code("max(true, 10l)").equals("10");
	code("max('string', 12)").error(ls::Error::METHOD_NOT_FOUND, {"max(" + env.tmp_string->to_string() + ", " + env.integer->to_string() + ")"});

	section("Number.min()");
	code("Number.min(5, 12)").equals("5");
	code("Number.min(75.7, 12)").almost(12.0);
	code("Number.min(5, 12.451)").almost(5.0);
	code("Number.min([5, 'a'][0], 4)").equals("4");
	code("Number.min([5, 'a'][0], 76)").equals("5");
	code("Number.min(4, [5, 'a'][0])").equals("4");
	code("Number.min(77, [5, 'a'][0])").equals("5");
	code("Number.min([55, 'a'][0], [5, 'a'][0])").equals("5");
	code("Number.min(5, 12.8)").equals("5");
	code("Number.min(15.7, 12.8)").equals("12.8");
	code("Number.min([15.7, ''][0], 12.8)").equals("12.8");
	code("Number.min(5.5, [12.8, ''][0])").equals("5.5");
	code("min(5l, 10.5)").equals("5");
	code("min(5l, 10)").equals("5");
	code("min(true, 10l)").equals("1");
	code("min('string', 12)").error(ls::Error::METHOD_NOT_FOUND, {"min(" + env.tmp_string->to_string() + ", " + env.integer->to_string() + ")"});

	section("Number.cos()");
	code("Number.cos(0)").equals("1");
	code("Number.cos(π)").equals("-1");
	code("Number.cos(π / 2)").almost(0.0);
	code("π.cos()").equals("-1");
	code("['', π][1].cos()").equals("-1");
	code("Number.cos(['', π][1])").equals("-1");
	code("cos(π)").equals("-1");

	section("Number.acos()");
	code("Number.acos(1)").equals("0");
	code("Number.acos(-1)").almost(M_PI);
	code("Number.acos(0)").almost(M_PI / 2);
	code("(-0.33).acos()").almost(1.907099901948877019);
	code("Number.acos(['y', 0][1])").almost(M_PI / 2);

	section("Number.sin()");
	code("Number.sin(0)").equals("0");
	code("Number.sin(π)").almost(0.0);
	code("Number.sin(π / 2)").equals("1");
	code("Number.sin(- π / 2)").equals("-1");
	code("Number.sin(['', π / 2][1])").equals("1");
	code("sin(π / 2)").equals("1");

	section("Number.tan()");
	code("Number.tan(0)").equals("0");
	code("Number.tan(π)").almost(0.0);
	code("Number.tan(π / 4)").almost(1.0);
	code("Number.tan(- π / 4)").almost(-1.0);
	code("Number.tan(['', π / 4][1])").almost(1.0);

	section("Number.asin()");
	code("Number.asin(0)").equals("0");
	code("Number.asin(-1)").almost(-M_PI / 2);
	code("Number.asin(1)").almost(M_PI / 2);
	code("0.33.asin()").almost(0.33630357515398035);
	code("Number.asin(['y', -1][1])").almost(-M_PI / 2);

	section("Number.atan()");
	code("Number.atan(1)").almost(M_PI / 4);
	code("Number.atan(-1)").almost(-M_PI / 4);
	code("Number.atan(0.5)").almost(0.463647609000806094);
	code("0.atan()").equals("0");
	code("Number.atan(['y', 0.5][1])").almost(0.463647609000806094);

	section("Number.atan2()");
	code("Number.atan2(1, 1)").almost(M_PI / 4);
	code("Number.atan2(150.78, 150.78)").almost(M_PI / 4);
	code("Number.atan2(1, 0)").almost(M_PI / 2);
	code("Number.atan2(-1, 0)").almost(-M_PI / 2);
	code("Number.atan2(0, 1)").equals("0");
	code("Number.atan2(0, -1)").almost(M_PI);
	code("Number.atan2(12.12, 42.42)").almost(0.278299659005111333);
	code("1.atan2(1)").almost(M_PI / 4);
	code("['', -1][1].atan2(1)").almost(-M_PI / 4);
	code("1.atan2(['', -1][1])").almost(3 * M_PI / 4);
	code("['', -1][1].atan2(['', -1][1])").almost(-3 * M_PI / 4);
	code("Number.atan2(1, 1)").almost(M_PI / 4);
	code("Number.atan2(['', -1][1], 1)").almost(-M_PI / 4);
	code("Number.atan2(1, ['', -1][1])").almost(3 * M_PI / 4);
	code("Number.atan2(['', -1][1], ['', -1][1])").almost(-3 * M_PI / 4);
	code("Number.atan2(1, true)").almost(M_PI / 4);
	code("Number.atan2(true, false)").almost(M_PI / 2);

	section("Number.cbrt()");
	code("Number.cbrt(1728)").almost(12.0, 1e-14);
	code("1728.cbrt()").almost(12.0, 0.00000000000001);
	code("Number.cbrt(['', 1728][1])").almost(12.0, 0.00000000000001);
	code("['', 1728][1].cbrt()").almost(12.0, 0.00000000000001);

	section("Number.int()");
	code("Number.int(15.1)").equals("15");
	code("Number.int(15.5)").equals("15");
	code("Number.int(15.9)").equals("15");

	section("Number.isInteger()");
	code("Number.isInteger(12)").equals("true");
	code("Number.isInteger(0)").equals("true");
	code("Number.isInteger(-5)").equals("true");
	code("Number.isInteger(12.9)").equals("false");
	code("Number.isInteger(-5.2)").equals("false");
	code("Number.isInteger(π)").equals("false");
	code("12.isInteger()").equals("true");
	code("12.5.isInteger()").equals("false");
	code("[12, 0][0].isInteger()").equals("true");
	code("[12.5, 0][0].isInteger()").equals("false");

	section("Number.fold");
	code("1234567.fold((x, y) -> x + y, 0)").equals("28");
	code("1234567.fold((x, y) -> x + y, 1000)").equals("1028");
	code("1234567.fold((x, y) -> x * y, 1)").equals("5040");
	code("1234567.fold((x, y) -> x + y ** 2, 0)").equals("140");

	section("Number.hypot");
	code("Number.hypot(3, 4)").equals("5");
	code("3.hypot(4)").equals("5");
	code("Number.hypot(34, 74)").almost(81.437092286);
	code("Number.hypot([34, ''][0], 74)").almost(81.437092286);

	section("Number.signum");
	code("0.signum()").equals("0");
	code("-0.signum()").equals("0");
	code("12.signum()").equals("1");
	code("12.5.signum()").equals("1");
	code("-12.signum()").equals("-1");
	code("-12.5.signum()").equals("-1");
	code("Number.signum(0)").equals("0");
	code("Number.signum(12)").equals("1");
	code("Number.signum(-17)").equals("-1");

	section("Number.sqrt");
	code("Number.sqrt(123456789123456789123456789)").equals("11111111066111");
	code("Number.sqrt(55m ** 20m)").equals("253295162119140625");
	code("Number.sqrt(12m + 5m)").equals("4");
	code("var n = 12; n.sqrt()").equals("3.4641016151");
	code("let f = Number.sqrt f(5)").equals("2.2360679775");
	code("sqrt(16)").equals("4");

	section("Number.toDegrees");
	code("π.toDegrees()").equals("180");
	code("(π / 2).toDegrees()").equals("90");
	code("(-π / 2).toDegrees()").equals("-90");
	code("0.toDegrees()").equals("0");

	section("Number.toRadians");
	code("180.toRadians()").almost(M_PI);
	code("90.toRadians()").almost(M_PI / 2);
	code("(-90).toRadians()").almost(-M_PI / 2);
	code("0.toRadians()").equals("0");

	section("Number.log");
	code("1.log()").equals("0");
	code("123456.log()").equals("11.7236400963");
	code("Number.log(654321)").equals("13.3913533357");
	code("Number.log([55555, ''][0])").equals("10.9251288");

	section("Number.log10");
	code("1.log10()").equals("0");
	code("123456.log10()").equals("5.0915122016");
	code("Number.log10(654321)").equals("5.8157908589");
	code("Number.log10([55555, ''][0])").equals("4.7447231519");

	section("Number.pow");
	code("2.pow(10)").equals("1024");
	code("Number.pow([10, ''][0], 5)").equals("100000");

	section("Object-like calls");
	code("(-12).abs()").equals("12");
	code("π.cos()").equals("-1");
	code("(π / 2).sin()").equals("1");
	code("12.sqrt()").almost(3.464101615137754386);
	code("12.8.floor()").equals("12");
	code("-12.8.floor()").equals("-12");
	code("(-12.8).floor()").equals("-13");
	code("12.2.ceil()").equals("13");
	code("12.8.round()").equals("13");
	code("-12.8.round()").equals("-13");
	code("2.pow(10)").equals("1024");
	code("0.isInteger()").equals("true");
	code("56.7.isInteger()").equals("false");
	code("(-56.7).isInteger()").equals("false");
	code("3.max(5)").equals("5");
	code("5.max(3)").equals("5");

	section("Combinated");
	code("3.max(5).min(2)").equals("2");
	code("3.max(5).max(10).max(12)").equals("12");
	code("10.max(5).max(8.7).max(-3.91)").equals("10");
	code("10.sqrt().cos()").almost(-0.99978607287932586);

	section("Number.isPrime()");
	code("0.isPrime()").equals("false");
	code("1.isPrime()").equals("false");
	code("2.isPrime()").equals("true");
	code("3.isPrime()").equals("true");
	code("4.isPrime()").equals("false");
	code("5.isPrime()").equals("true");
	code("1993.isPrime()").equals("true");
	code("3972049.isPrime()").equals("false");
	code("(1993l).isPrime()").equals("true");
	code("4398042316799.isPrime()").equals("true");
	code("(4398042316799m).isPrime()").equals("1");
	code("359334085968622831041960188598043661065388726959079837.isPrime()").equals("1");
	code("(146m ** 45m).isPrime()").equals("0");
	code("1993l.isPrime()").equals("true");
	code("1993m.isPrime()").equals("2");

	section("Number.rand()");
	code("var a = Number.rand() a >= 0 and a <= 1").equals("true");
	code("var a = Number.rand() a > 1").equals("false");
	code("var a = Number.randInt(2067, 2070) a >= 2067 and a < 2070").equals("true");
	code("var a = Number.randFloat(500, 510) a >= 500 and a < 510 and !a.isInteger()").equals("true");
}
