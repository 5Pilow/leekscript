#include "Test.hpp"
#include "../src/util/Util.hpp"

void Test::test_utils() {

	header("Utils");
	section("is_file_name");

	Util::is_file_name("Makefile"); // true
	Util::is_file_name("12 + 5"); // false
	Util::is_file_name("salut.leek"); // true
	Util::is_file_name("hello.ls"); // true
	Util::is_file_name("helloworld"); // false

	section("read_file_lines");
	Util::read_file_lines("Makefile");

	section("replace_all");
	std::string haystack = "bonjour";
	std::string needle = "o";
	std::string replacement = "_";
	std::string res = Util::replace_all(haystack, needle, replacement);
	assert(res == "b_nj_ur");

	section("file_short_name");
	assert(Util::file_short_name("foo/bar/toto.txt") == "toto.txt");
}
