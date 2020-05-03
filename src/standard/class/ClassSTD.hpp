#ifndef CLASSSTD_HPP
#define CLASSSTD_HPP

#include "../Module.hpp"

namespace ls {

class ClassSTD : public Module {
public:
	ClassSTD(Environment& env);

	#if COMPILER
	static void add_field(LSClass* clazz, char* field_name, LSValue* default_value);
	#endif

	static Compiler::value construct(Compiler& c, std::vector<Compiler::value>, int);
};

}

#endif
