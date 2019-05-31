#include "Call.hpp"
#include "../value/Value.hpp"
#include "Callable.hpp"
#include "../../vm/Module.hpp"
#include "../semantic/SemanticAnalyzer.hpp"
#include "../../type/Function_type.hpp"
#include "../value/LeftValue.hpp"

namespace ls {

Call::Call() : callable(new Callable()) {}
Call::Call(std::vector<const CallableVersion*> versions) : callable(new Callable(versions)) {}
Call::Call(std::initializer_list<const CallableVersion*> versions) : callable(new Callable(versions)) {}

void Call::add_version(const CallableVersion* v) {
	callable->add_version(v);
}

const CallableVersion* Call::resolve(SemanticAnalyzer* analyzer, std::vector<Type> arguments) const {
	// std::cout << "Call::resolve(" << arguments << ") object = " << (object ? object->type : Type()) << std::endl;
	if (object) {
		arguments.insert(arguments.begin(), object->type);
	}
	return callable->resolve(analyzer, arguments);
}

void Call::apply_mutators(SemanticAnalyzer* analyzer, const CallableVersion* version, std::vector<Value*> values) const {
	// std::cout << "Call::apply_mutators " << values.size() << std::endl;
	if (object) values.insert(values.begin(), object);
	version->apply_mutators(analyzer, values);
}

void Call::pre_compile_call(Compiler& c) const {
	if (object) {
		((Call*) this)->compiled_object = [&]() { if (object->isLeftValue()) {
			if (object->type.is_mpz_ptr()) {
				return ((LeftValue*) object)->compile_l(c);
			} else {
				return c.insn_load(((LeftValue*) object)->compile_l(c));
			}
		} else {
			return object->compile(c);
		}}();
	}
}

Compiler::value Call::compile_call(Compiler& c, const CallableVersion* version, std::vector<Compiler::value> args, bool no_return) const {
	// std::cout << "Call::compile_call(" << args << ")" << std::endl;
	// Add the object if it's a method call
	if (object) {
		args.insert(args.begin(), compiled_object);
	}
	// Do the call
	auto r = version->compile_call(c, args, no_return);
	if (object) {
		object->compile_end(c);
	}
	return r;
}

}