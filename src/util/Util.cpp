#include "Util.hpp"
#include <iostream>
#include <stdlib.h>
#include <ctime>
#include <sstream>
#include <fstream>
#include <istream>
#include <iterator>
#include <algorithm>
#include <queue>
#include "../colors.h"

namespace ls {

std::string Util::read_file(std::string file) {
	std::ifstream ifs(file.data());
	if (!ifs.good()) {
		std::cout << "[" << C_YELLOW << "warning" << END_COLOR << "] File '" << BOLD << file << END_STYLE << "' does not exist." << std::endl;
		return "";
	}
	std::string content = std::string((std::istreambuf_iterator<char>(ifs)), (std::istreambuf_iterator<char>()));
	ifs.close();
	return content;
}

std::vector<std::string> Util::read_file_lines(std::string file) {
	std::ifstream f(file);
	std::vector<std::string> lines;
	for (std::string line; std::getline(f, line);) {
		lines.push_back(line);
	}
	return lines;
}

bool Util::is_file_name(std::string data) {
	// Real file?
	std::ifstream test(data);
	if (test) return true;
	// Contains spaces => no
	if (data.find_first_of("\t\n ") != std::string::npos) return false;
	// Ends with '.leek' or '.ls' => yes
	if (data.size() <= 4) return false;
	std::string dot_leek = ".leek";
	std::string dot_ls = ".ls";
    if (std::equal(dot_leek.rbegin(), dot_leek.rend(), data.rbegin())) return true;
	if (std::equal(dot_ls.rbegin(), dot_ls.rend(), data.rbegin())) return true;
	// Not a file
	return false;
}

std::string Util::replace_all(std::string& haystack, const std::string& needle, const std::string& replacement) {
	size_t pos = 0;
    while ((pos = haystack.find(needle, pos)) != std::string::npos) {
        haystack.replace(pos, needle.length(), replacement);
        pos += replacement.length();
    }
    return haystack;
}

std::string Util::file_short_name(std::string path) {
	return {
		std::find_if(path.rbegin(), path.rend(), [](char c) {
			return c == '/';
		}).base(), path.end()
	};
}

std::string Util::toupper(const std::string& string) {
	std::string result = string;
	std::for_each(result.begin(), result.end(), [](char& in) {
		in = ::toupper(in);
	});
	return result;
}

std::string Util::tolower(const std::string& string) {
	std::string result = string;
	std::for_each(result.begin(), result.end(), [](char& in) {
		in = ::tolower(in);
	});
	return result;
}

}