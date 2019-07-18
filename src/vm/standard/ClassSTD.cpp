#include "ClassSTD.hpp"
#include "../value/LSClass.hpp"
#include "../../type/Type.hpp"

namespace ls {

ClassSTD::ClassSTD(VM* vm) : Module(vm, "Class") {

	LSClass::_clazz = lsclass.get();

	field("name", Type::string);

	constructor_({
		{Type::clazz(), {Type::i8_ptr}, (void*) LSClass::constructor},
	});

	/** Internal **/
	method("add_field", {
		{Type::void_, {Type::clazz(), Type::i8_ptr, Type::any}, (void*) add_field}
	});
}

void ClassSTD::add_field(LSClass* clazz, char* field_name, LSValue* default_value) {
	clazz->clazz->addField(field_name, Type::any, nullptr);
	clazz->clazz->fields.at(field_name).default_value = default_value;
}

}
