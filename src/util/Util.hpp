#ifndef UTIL_HPP_
#define UTIL_HPP_

#include <string>
#include <vector>
#include <queue>

namespace ls {

class Util {
public:

	static std::string read_file(std::string file);
	static std::vector<std::string> read_file_lines(std::string file);
	static bool is_file_name(std::string data);
	static std::string replace_all(std::string& haystack, const std::string& needle, const std::string& replacement);
	static std::string file_short_name(std::string path);

	static std::string toupper(const std::string& string);
	static std::string tolower(const std::string& string);
};

}

#endif
