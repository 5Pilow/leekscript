#include "ObjectSTD.hpp"
#include "ValueSTD.hpp"
#include "../../type/Type.hpp"
#include "../../environment/Environment.hpp"
#if COMPILER
#include "../../vm/value/LSObject.hpp"
#include "../../vm/value/LSNumber.hpp"
#endif

namespace ls {

#if COMPILER
LSObject* ObjectSTD::readonly = new LSObject();
LSNumber* ObjectSTD::readonly_value = LSNumber::get(12);
#endif

ObjectSTD::ObjectSTD(Environment& env) : Module(env, "Object") {

	#if COMPILER
	env.object_class = std::make_unique<LSClass>(clazz.get());
	lsclass = env.object_class.get();

	readonly->addField("v", readonly_value);
	readonly->readonly = true;
	readonly->native = true;
	#endif

	static_field("readonly", env.object, ADDR((void*) &readonly), PRIVATE);

	/*
	 * Constructor
	 */
	constructor_({
		{env.tmp_object, {}, ADDR((void*) &LSObject::constructor)},
		{env.tmp_object, {env.clazz()}, ADDR((void*) object_new)},
	});

	/*
	 * Operators
	 */
	operator_("in", {
		{env.object, env.any, env.boolean, ADDR((void*) &LSObject::in)},
		{env.object, env.number, env.boolean, ADDR(in_any)}
	});

	/*
	 * Methods
	 */
	method("copy", {
		{env.object, {env.object}, ADDR(ValueSTD::copy)}
	});
	auto map_fun_type = Type::fun_object(env.any, {env.any});
	auto map_fun = ADDR(&LSObject::ls_map<LSFunction*>);
	method("map", {
		{env.tmp_object, {env.object, map_fun_type}, (void*) map_fun}
	});
	method("keys", {
		{Type::array(env.string), {env.object}, ADDR((void*) &LSObject::ls_get_keys)}
	});
	method("values", {
		{Type::array(env.any), {env.object}, ADDR((void*) &LSObject::ls_get_values)}
	});

	/** Internal **/
	method("add_field", {
		{env.void_, {env.object, env.i8_ptr, env.any}, ADDR((void*) &LSObject::addField)}
	}, PRIVATE);
}

#if COMPILER

Compiler::value ObjectSTD::in_any(Compiler& c, std::vector<Compiler::value> args, int) {
	return c.insn_call(c.env.any, {args[0], c.insn_to_any(args[1])}, "Value.operatorin");
}

LSValue* ObjectSTD::object_new(LSClass* clazz) {
	return new LSObject(clazz);
}

#endif

}
