#ifndef FILE_RESOLVER_HPP
#define FILE_RESOLVER_HPP

#include <string>
#include <filesystem>

namespace ls {

class FileContext {
public:
	std::filesystem::path folder;
	
	FileContext(std::filesystem::path folder = ".") : folder(folder) {}
};

}

#endif