#ifndef VIRTUAL_RESOLVER_HPP
#define VIRTUAL_RESOLVER_HPP

#include "File.hpp"
#include "FileContext.hpp"
#include <string>

namespace ls {

class Program;

class VirtualResolver {
public:
    File* create(std::string path, Program* program) const;
	File* delete_(std::string path) const;
	File* resolve(std::string path, FileContext context) const;

    static std::unordered_map<std::string, std::unique_ptr<File>>& get_cache();
};

}

#endif