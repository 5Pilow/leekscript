#ifndef OBJECTSTD_HPP
#define OBJECTSTD_HPP

#include "../Module.hpp"

namespace ls {

class LSObject;
class LSNumber;

class ObjectSTD : public Module {
public:
	ObjectSTD(Environment& env);
	~ObjectSTD();

	#if COMPILER

	std::unique_ptr<LSObject> readonly;
	std::unique_ptr<LSNumber> readonly_value;

	static Compiler::value in_any(Compiler& c, std::vector<Compiler::value> args, int);
	static LSValue* object_new(LSClass* clazz);

	#endif
};

}

#endif
