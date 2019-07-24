#include "FunctionSTD.hpp"
#include "ValueSTD.hpp"
#include "../../type/Type.hpp"
#if COMPILER
#include "../../vm/value/LSFunction.hpp"
#include "../../vm/value/LSClosure.hpp"
#endif

namespace ls {

FunctionSTD::FunctionSTD(StandardLibrary* stdLib) : Module(stdLib, "Function") {

	#if COMPILER
	LSFunction::clazz = lsclass.get();
	#endif

	field("return", Type::clazz(), ADDR(field_return));
	field("args", Type::array(Type::clazz()), ADDR(field_args));

	constructor_({
		{Type::fun_object(Type::void_, {}), {Type::i8_ptr, Type::i8_ptr}, ADDR((void*) LSFunction::constructor)},
		{Type::closure(Type::void_, {}), {Type::i8_ptr, Type::i8_ptr}, ADDR((void*) LSClosure::constructor)},
	});

	/** Internal **/
	method("call", {
		{Type::any, {Type::fun(Type::void_, {})}, ADDR((void*) &LSFunction::call)}
	});
	method("get_capture", {
		{Type::any, {Type::closure(Type::void_, {})}, ADDR((void*) &LSClosure::get_capture)}
	});
	method("get_capture_l", {
		{Type::any, {Type::closure(Type::void_, {})}, ADDR((void*) &LSClosure::get_capture_l)}
	});
	method("add_capture", {
		{Type::void_, {Type::closure(Type::void_, {}), Type::any}, ADDR((void*) &LSClosure::add_capture)}
	});
}

#if COMPILER

Compiler::value FunctionSTD::field_return(Compiler& c, Compiler::value function) {
	auto class_name = function.t->return_type()->class_name();
	if (!class_name.size()) class_name = "Value";
	return c.get_symbol(class_name, Type::clazz(class_name));
}

Compiler::value FunctionSTD::field_args(Compiler& c, Compiler::value function) {
	std::vector<Compiler::value> args;
	for (const auto& arg : function.t->arguments()) {
		auto class_name = arg->class_name();
		if (!class_name.size()) class_name = "Value";
		args.push_back(c.get_symbol(class_name, Type::clazz(class_name)));
	}
	return c.new_array(Type::array(Type::clazz()), args);
}

#endif

}
