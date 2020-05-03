#include "ClassSTD.hpp"
#include "../../type/Type.hpp"
#include "../../environment/Environment.hpp"
#if COMPILER
#include "../../vm/value/LSClass.hpp"
#endif

namespace ls {

ClassSTD::ClassSTD(Environment& env) : Module(env, "Class") {

	#if COMPILER
	env.class_class = std::make_unique<LSClass>(clazz.get());
	lsclass = env.class_class.get();
	#endif

	field("name", env.string);

	constructor_({
		{env.clazz(), {env.i8_ptr, env.i8_ptr}, ADDR((void*) LSClass::constructor)},
	});

	method("construct", {
		{env.any, {env.clazz()}, ADDR(construct)}
	});

	/** Internal **/
	method("add_field", {
		{env.void_, {env.clazz(), env.i8_ptr, env.any}, ADDR((void*) add_field)}
	}, PRIVATE);
}

#if COMPILER

Compiler::value ClassSTD::construct(Compiler& c, std::vector<Compiler::value> args, int) {
	return c.new_object_class(args[0]);
}

void ClassSTD::add_field(LSClass* clazz, char* field_name, LSValue* default_value) {
	clazz->clazz->addField(field_name, clazz->clazz->env.any, nullptr);
	clazz->clazz->fields.at(field_name).default_value = default_value;
}

#endif

}
