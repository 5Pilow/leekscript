#ifndef TODO_H_
#define TODO_H_

#include <string>
#include "Location.hpp"

namespace ls {

class Todo {

public:

	Location location;
	std::string content;

	Todo(Location location, const std::string& content) : location(location), content(content) {}

    Json json() const {
        return { location.start.line, location.start.column, location.end.line, location.end.column, 2, "TODO", { content } };
    }
};

}

#endif
