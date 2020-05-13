#ifndef RESOLVER_HPP
#define RESOLVER_HPP

#include "File.hpp"
#include <string>

namespace ls {

class FileResolver {
public:
    File* create(std::string path, Program* program) const;
	File* resolve(std::string path, FileContext context) const;
};

}

#endif