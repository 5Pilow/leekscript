#ifndef HOVER_HPP
#define HOVER_HPP

namespace ls {

#include <string>
class Type;

class Hover {
public:
	const Type* type;
	Location location;
	std::string alias = "";
};


}

#endif