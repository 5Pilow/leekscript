#include "Location.hpp"

namespace ls {

bool Location::contains(size_t position) const {
	return position >= start.raw and position <= end.raw;
}

}