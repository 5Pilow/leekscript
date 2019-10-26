#include "Location.hpp"

namespace ls {

Position Position::after() const {
	return { line, column + 1, raw + 1 };
}

Json Position::json() const {
	return Json::array({
		line, column, raw
	});
}

bool Location::contains(size_t position) const {
	return position >= start.raw and position <= end.raw;
}

Location Location::after() const {
	return { file, end.after(), end.after() };
}

Json Location::json() const {
	return Json::array({
		start.json(), end.json()
	});
}

}