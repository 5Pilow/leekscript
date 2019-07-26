#include "ClassSTD.hpp"
#include "../../type/Type.hpp"
#include "../../environment/Environment.hpp"
#if COMPILER
#include "../../vm/value/LSClass.hpp"
#endif

namespace ls {

ClassSTD::ClassSTD(Environment& env) : Module(env, "Class") {

	#if COMPILER
	LSClass::_clazz = lsclass.get();
	#endif

	field("name", env.string);

	constructor_({
		{env.clazz(), {env.i8_ptr, env.i8_ptr}, ADDR((void*) LSClass::constructor)},
	});

	/** Internal **/
	method("add_field", {
		{env.void_, {env.clazz(), env.i8_ptr, env.any}, ADDR((void*) add_field)}
	});
}

#if COMPILER
void ClassSTD::add_field(LSClass* clazz, char* field_name, LSValue* default_value) {
	clazz->clazz->addField(field_name, clazz->clazz->env.any, nullptr);
	clazz->clazz->fields.at(field_name).default_value = default_value;
}
#endif

}
