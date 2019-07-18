#ifndef FUNCTION_STD_HPP
#define FUNCTION_STD_HPP

#include "../Module.hpp"

namespace ls {

class FunctionSTD : public Module {
public:
	FunctionSTD(VM* vm);

	static Compiler::value field_return(Compiler& c, Compiler::value a);
	static Compiler::value field_args(Compiler& c, Compiler::value a);
};

}

#endif
