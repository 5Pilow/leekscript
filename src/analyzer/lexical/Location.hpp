#ifndef LOCATION_HPP_
#define LOCATION_HPP_

#include <stdio.h>
#include "../../util/json.hpp"

namespace ls {

class File;

struct Position {
	size_t line;
	size_t column;
	size_t raw;
	Position(size_t line, size_t column, size_t raw) :
		line(line), column(column), raw(raw) {}
	Json json() const;
};

struct Location {
	File* file;
	Position start;
	Position end;
	Location(File* file, Position start, Position end) : file(file), start(start), end(end) {}

	bool contains(size_t position) const;
	Json json() const;
};

}

#endif
