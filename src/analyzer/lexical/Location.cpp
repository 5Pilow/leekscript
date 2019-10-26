#include "Location.hpp"

namespace ls {

Json Position::json() const {
	return Json::array({
		line, column, raw
	});
}

bool Location::contains(size_t position) const {
	return position >= start.raw and position <= end.raw;
}

Json Location::json() const {
	return Json::array({
		start.json(), end.json()
	});
}

}