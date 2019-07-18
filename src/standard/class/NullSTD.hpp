#ifndef NULLSTD_HPP
#define NULLSTD_HPP

#include "../Module.hpp"

namespace ls {

class LSNull;

class NullSTD : public Module {
public:
	NullSTD(VM* vm);
};

}

#endif
