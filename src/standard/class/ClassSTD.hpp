#ifndef CLASSSTD_HPP
#define CLASSSTD_HPP

#include "../Module.hpp"

namespace ls {

class ClassSTD : public Module {
public:
	ClassSTD(StandardLibrary* stdLib);

	static void add_field(LSClass* clazz, char* field_name, LSValue* default_value);
};

}

#endif
