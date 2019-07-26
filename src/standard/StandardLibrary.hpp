#ifndef STANDARD_LIBRARY_HPP
#define STANDARD_LIBRARY_HPP

#include "../analyzer/semantic/Class.hpp"
#include <unordered_map>
#include "Module.hpp"

namespace ls {

class Environment;

class StandardLibrary {
public:
	Environment& env;
	bool legacy = false;
	std::unordered_map<std::string, std::unique_ptr<Module>> classes;

	StandardLibrary(Environment& env, bool legacy = false);
	void add_class(std::unique_ptr<Module> m);
};

}

#endif