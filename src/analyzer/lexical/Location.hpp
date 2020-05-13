#ifndef LOCATION_HPP_
#define LOCATION_HPP_

#include <stdio.h>
#include "../../util/json.hpp"

namespace ls {

class File;

struct Position {
	size_t line = 0;
	size_t column = 0;
	size_t raw = 0;
	Position() {}
	Position(size_t line, size_t column, size_t raw) :
		line(line), column(column), raw(raw) {}

	Position after() const;
	Json json() const;
};

struct Location {
	File* file = nullptr;
	Position start;
	Position end;
	Location() {}
	Location(File* file, Position start, Position end) : file(file), start(start), end(end) {}

	bool contains(size_t position) const;
	Location after() const;
	Json json() const;
};

}

#endif
