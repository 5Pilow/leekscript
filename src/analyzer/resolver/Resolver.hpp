#ifndef RESOLVER_HPP
#define RESOLVER_HPP

#include "File.hpp"
#include <string>

namespace ls {

class Resolver {
public:
	File* resolve(std::string path, FileContext* context) const;
};

}

#endif