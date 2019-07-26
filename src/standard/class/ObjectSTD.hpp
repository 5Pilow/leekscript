#ifndef OBJECTSTD_HPP
#define OBJECTSTD_HPP

#include "../Module.hpp"

namespace ls {

class LSObject;

class ObjectSTD : public Module {
public:
	ObjectSTD(Environment& env);

	#if COMPILER
	static LSObject* readonly;
	static LSNumber* readonly_value;
	#endif

	static Compiler::value in_any(Compiler& c, std::vector<Compiler::value> args, int);
	static LSValue* object_new(LSClass* clazz);
};

}

#endif
