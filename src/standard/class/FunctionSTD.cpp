#include "FunctionSTD.hpp"
#include "ValueSTD.hpp"
#include "../../type/Type.hpp"
#include "../../environment/Environment.hpp"
#if COMPILER
#include "../../vm/value/LSFunction.hpp"
#include "../../vm/value/LSClosure.hpp"
#endif

namespace ls {

FunctionSTD::FunctionSTD(Environment& env) : Module(env, "Function") {

	#if COMPILER
	env.function_class = std::make_unique<LSClass>(clazz.get());
	lsclass = env.function_class.get();
	#endif

	field("return", env.clazz(), ADDR(field_return));
	field("args", Type::array(env.clazz()), ADDR(field_args));

	constructor_({
		{Type::fun_object(env.void_, {}), {env.i8_ptr, env.i8_ptr}, ADDR((void*) LSFunction::constructor)},
		{Type::closure(env.void_, {}), {env.i8_ptr, env.i8_ptr}, ADDR((void*) LSClosure::constructor)},
	});

	/** Internal **/
	method("call", {
		{env.any, {Type::fun(env.void_, {})}, ADDR((void*) &LSFunction::call)}
	}, PRIVATE);
	method("get_capture", {
		{env.any, {Type::closure(env.void_, {})}, ADDR((void*) &LSClosure::get_capture)}
	}, PRIVATE);
	method("get_capture_l", {
		{env.any, {Type::closure(env.void_, {})}, ADDR((void*) &LSClosure::get_capture_l)}
	}, PRIVATE);
	method("add_capture", {
		{env.void_, {Type::closure(env.void_, {}), env.any}, ADDR((void*) &LSClosure::add_capture)}
	}, PRIVATE);
}

#if COMPILER

Compiler::value FunctionSTD::field_return(Compiler& c, Compiler::value function) {
	auto class_name = function.t->return_type()->class_name();
	if (!class_name.size()) class_name = "Value";
	return c.get_symbol(class_name, c.env.clazz(class_name));
}

Compiler::value FunctionSTD::field_args(Compiler& c, Compiler::value function) {
	std::vector<Compiler::value> args;
	for (const auto& arg : function.t->arguments()) {
		auto class_name = arg->class_name();
		if (!class_name.size()) class_name = "Value";
		args.push_back(c.get_symbol(class_name, c.env.clazz(class_name)));
	}
	return c.new_array(Type::array(c.env.clazz()), args);
}

#endif

}
