#include "ObjectSTD.hpp"
#include "ValueSTD.hpp"
#include "../value/LSObject.hpp"
#include "../value/LSNumber.hpp"
#include "../../type/Type.hpp"

namespace ls {

LSObject* ObjectSTD::readonly = new LSObject();
LSNumber* ObjectSTD::readonly_value = LSNumber::get(12);

ObjectSTD::ObjectSTD(VM* vm) : Module(vm, "Object") {

	#if COMPILER
	LSObject::object_class = lsclass.get();
	#endif

	readonly->addField("v", readonly_value);
	readonly->readonly = true;
	readonly->native = true;

	static_field("readonly", Type::object, (void*) &readonly);

	/*
	 * Constructor
	 */
	constructor_({
		{Type::tmp_object, {}, (void*) &LSObject::constructor},
		{Type::tmp_object, {Type::clazz()}, (void*) object_new},
	});

	/*
	 * Operators
	 */
	operator_("in", {
		{Type::object, Type::any, Type::boolean, (void*) &LSObject::in},
		{Type::object, Type::number, Type::boolean, ADDR(in_any)}
	});

	/*
	 * Methods
	 */
	method("copy", {
		{Type::object, {Type::object}, ADDR(ValueSTD::copy)}
	});
	auto map_fun_type = Type::fun_object(Type::any, {Type::any});
	auto map_fun = &LSObject::ls_map<LSFunction*>;
	method("map", {
		{Type::tmp_object, {Type::object, map_fun_type}, (void*) map_fun}
	});
	method("keys", {
		{Type::array(Type::string), {Type::object}, (void*) &LSObject::ls_get_keys}
	});
	method("values", {
		{Type::array(Type::any), {Type::object}, (void*) &LSObject::ls_get_values}
	});

	/** Internal **/
	method("add_field", {
		{Type::void_, {Type::object, Type::i8_ptr, Type::any}, (void*) &LSObject::addField}
	});
}

#if COMPILER

Compiler::value ObjectSTD::in_any(Compiler& c, std::vector<Compiler::value> args, int) {
	return c.insn_call(Type::any, {args[0], c.insn_to_any(args[1])}, "Value.operatorin");
}

LSValue* ObjectSTD::object_new(LSClass* clazz) {
	return new LSObject(clazz);
}

#endif

}
