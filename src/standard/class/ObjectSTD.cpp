#include "ObjectSTD.hpp"
#include "ValueSTD.hpp"
#include "../../type/Type.hpp"
#if COMPILER
#include "../../vm/value/LSObject.hpp"
#include "../../vm/value/LSNumber.hpp"
#endif

namespace ls {

#if COMPILER
LSObject* ObjectSTD::readonly = new LSObject();
LSNumber* ObjectSTD::readonly_value = LSNumber::get(12);
#endif

ObjectSTD::ObjectSTD(StandardLibrary* stdLib) : Module(stdLib, "Object") {

	#if COMPILER
	LSObject::object_class = lsclass.get();

	readonly->addField("v", readonly_value);
	readonly->readonly = true;
	readonly->native = true;
	#endif

	static_field("readonly", Type::object, ADDR((void*) &readonly));

	/*
	 * Constructor
	 */
	constructor_({
		{Type::tmp_object, {}, ADDR((void*) &LSObject::constructor)},
		{Type::tmp_object, {Type::clazz()}, ADDR((void*) object_new)},
	});

	/*
	 * Operators
	 */
	operator_("in", {
		{Type::object, Type::any, Type::boolean, ADDR((void*) &LSObject::in)},
		{Type::object, Type::number, Type::boolean, ADDR(in_any)}
	});

	/*
	 * Methods
	 */
	method("copy", {
		{Type::object, {Type::object}, ADDR(ValueSTD::copy)}
	});
	auto map_fun_type = Type::fun_object(Type::any, {Type::any});
	auto map_fun = ADDR(&LSObject::ls_map<LSFunction*>);
	method("map", {
		{Type::tmp_object, {Type::object, map_fun_type}, (void*) map_fun}
	});
	method("keys", {
		{Type::array(Type::string), {Type::object}, ADDR((void*) &LSObject::ls_get_keys)}
	});
	method("values", {
		{Type::array(Type::any), {Type::object}, ADDR((void*) &LSObject::ls_get_values)}
	});

	/** Internal **/
	method("add_field", {
		{Type::void_, {Type::object, Type::i8_ptr, Type::any}, ADDR((void*) &LSObject::addField)}
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
