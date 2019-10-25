#include "Test.hpp"
#include "../src/util/Util.hpp"

void Test::test_utils() {

	header("Utils");
	section("is_file_name");

	ls::Util::is_file_name("Makefile"); // true
	ls::Util::is_file_name("12 + 5"); // false
	ls::Util::is_file_name("salut.leek"); // true
	ls::Util::is_file_name("hello.ls"); // true
	ls::Util::is_file_name("helloworld"); // false

	section("read_file_lines");
	ls::Util::read_file_lines("Makefile");

	section("replace_all");
	std::string haystack = "bonjour";
	std::string needle = "o";
	std::string replacement = "_";
	std::string res = ls::Util::replace_all(haystack, needle, replacement);
	assert(res == "b_nj_ur");

	section("file_short_name");
	assert(ls::Util::file_short_name("foo/bar/toto.txt") == "toto.txt");
}
