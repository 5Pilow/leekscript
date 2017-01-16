#include "ObjectSTD.hpp"
#include "../value/LSObject.hpp"

namespace ls {

LSObject ObjectSTD::readonly;
LSNumber ObjectSTD::readonly_value(12);

ObjectSTD::ObjectSTD() : Module("Object") {

	readonly.values.insert({"v", &readonly_value});
	readonly.readonly = true;
	readonly.native = true;

	static_field("readonly", Type::OBJECT, (void*)+[&](Compiler& c) {
		return c.new_pointer(&ObjectSTD::readonly);
	});

	operator_("in", {
		{Type::OBJECT, Type::POINTER, Type::BOOLEAN, (void*) &LSObject::in, Method::NATIVE}
	});

	Type map_fun_type = Type::FUNCTION_P;
	map_fun_type.setArgumentType(0, Type::POINTER);
	map_fun_type.setReturnType(Type::POINTER);
	method("map", {
		{Type::OBJECT, Type::OBJECT, {map_fun_type}, (void*) &LSObject::ls_map, Method::NATIVE}
	});

	method("keys", {
		{Type::OBJECT, Type::STRING_ARRAY, {}, (void*) &LSObject::ls_get_keys, Method::NATIVE}
	});
	method("values", {
		{Type::OBJECT, Type::PTR_ARRAY, {}, (void*) &LSObject::ls_get_values, Method::NATIVE}
	});
}

}
