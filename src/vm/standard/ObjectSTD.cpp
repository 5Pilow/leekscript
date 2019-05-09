#include "ObjectSTD.hpp"
#include "ValueSTD.hpp"
#include "../value/LSObject.hpp"
#include "../value/LSNumber.hpp"

namespace ls {

LSObject ObjectSTD::readonly;
LSNumber* ObjectSTD::readonly_value = LSNumber::get(12);

ObjectSTD::ObjectSTD() : Module("Object") {

	LSObject::object_class = clazz;

	readonly.addField("v", readonly_value);
	readonly.readonly = true;
	readonly.native = true;

	static_field("readonly", Type::object(), [&](Compiler& c) {
		return c.new_pointer(&ObjectSTD::readonly, Type::object());
	});

	/*
	 * Constructor
	 */
	constructor_({
		{Type::tmp_object(), {}, (void*) &LSObject::constructor},
		{Type::tmp_object(), {Type::clazz()}, (void*) ObjectSTD::object_new},
	});

	/*
	 * Operators
	 */
	operator_("in", {
		{Type::object(), Type::any(), Type::boolean(), (void*) &LSObject::in, {}, Method::NATIVE},
		{Type::object(), Type::number(), Type::boolean(), (void*) &in_any}
	});

	/*
	 * Methods
	 */
	method("copy", {
		{Type::object(), {Type::object()}, ValueSTD::copy}
	});
	Type map_fun_type = Type::fun(Type::any(), {Type::any()});
	auto map_fun = &LSObject::ls_map<LSFunction*>;
	method("map", {
		{Type::object(), {Type::object(), map_fun_type}, (void*) map_fun}
	});
	method("keys", {
		{Type::array(Type::string()), {Type::object()}, (void*) &LSObject::ls_get_keys}
	});
	method("values", {
		{Type::array(Type::any()), {Type::object()}, (void*) &LSObject::ls_get_values}
	});

	/** Internal **/
	method("add_field", {
		{{}, {Type::object(), Type::i8().pointer(), Type::any()}, (void*) &LSObject::addField}
	});
	method("set_field_default_value", {
		{{}, {Type::clazz(), Type::i8().pointer(), Type::any()}, (void*) ObjectSTD::set_field_default_value}
	});
}

Compiler::value ObjectSTD::in_any(Compiler& c, std::vector<Compiler::value> args) {
	return c.insn_call(Type::any(), {args[0], c.insn_to_any(args[1])}, "Value.operatorin");
}

LSValue* ObjectSTD::object_new(LSClass* clazz) {
	return new LSObject(clazz);
}
void ObjectSTD::set_field_default_value(LSClass* clazz, char* field_name, LSValue* default_value) {
	clazz->fields.at(field_name).default_value = default_value;
}

}
