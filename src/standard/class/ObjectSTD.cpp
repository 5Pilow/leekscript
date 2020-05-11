#include "ObjectSTD.hpp"
#include "ValueSTD.hpp"
#include "../../type/Type.hpp"
#include "../../environment/Environment.hpp"
#if COMPILER
#include "../../vm/value/LSObject.hpp"
#include "../../vm/value/LSNumber.hpp"
#endif

namespace ls {

ObjectSTD::ObjectSTD(Environment& env) : Module(env, "Object") {

	#if COMPILER
	env.object_class = std::make_unique<LSClass>(clazz.get());
	lsclass = env.object_class.get();

	readonly = std::make_unique<LSObject>();
	readonly_value = std::unique_ptr<LSNumber>(LSNumber::get(12));
	readonly->addField("v", readonly_value.get());
	readonly->native = true;
	readonly->readonly = true;
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
		{env.object, env.any, env.boolean, ADDR((void*) LSObject::ls_in)},
		{env.object, env.number, env.boolean, ADDR(in_any)}
	});

	/*
	 * Methods
	 */
	method("copy", {
		{env.object, {env.object}, ADDR(ValueSTD::copy)}
	});
	method("map", {
		{env.tmp_object, {env.object, Type::fun_object(env.any, {env.any})}, ADDR((void*) LSObject::ls_map<LSFunction*>)}
	});
	method("keys", {
		{Type::array(env.string), {env.object}, ADDR((void*) LSObject::ls_get_keys)}
	});
	method("values", {
		{Type::array(env.any), {env.object}, ADDR((void*) LSObject::ls_get_values)}
	});

	/** Internal **/
	method("add_field", {
		{env.void_, {env.object, env.i8_ptr, env.any}, ADDR((void*) LSObject::std_add_field)}
	}, PRIVATE);
}

ObjectSTD::~ObjectSTD() {
	#if COMPILER
	readonly->values.clear();
	#endif
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
