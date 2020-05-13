#ifndef SYNTAXICAL_HOVER_HPP
#define SYNTAXICAL_HOVER_HPP

#include <string>
#include "../lexical/Location.hpp"

namespace ls {

class Type;
class Environment;

class Hover {
public:
	const Type* type;
	Location location;
	std::string alias = "";
	std::string defined_file = "";
	size_t defined_line = -1;

	Hover(Environment& env);
	Hover(const Type*, Location location, std::string alias = "");
};

}

#endif