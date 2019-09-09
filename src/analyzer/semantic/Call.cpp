#include "Call.hpp"
#include "../value/Value.hpp"
#include "Callable.hpp"
#include "../../vm/Module.hpp"
#include "../semantic/SemanticAnalyzer.hpp"
#include "../value/LeftValue.hpp"
#include "CallableVersion.hpp"
#include "../../type/Type.hpp"

namespace ls {

Call::Call(std::vector<const CallableVersion*> versions) : callable(new Callable(versions)) {}
Call::Call(std::initializer_list<const CallableVersion*> versions) : callable(new Callable(versions)) {}

void Call::add_version(const CallableVersion* v) {
	callable->add_version(v);
}

const CallableVersion* Call::resolve(SemanticAnalyzer* analyzer, std::vector<const Type*> arguments) const {
	// std::cout << "Call::resolve(" << arguments << ") object = " << (object ? object->type : Type::void_) << std::endl;
	if (object) {
		arguments.insert(arguments.begin(), object->type);
	}
	return callable->resolve(analyzer, arguments);
}

void Call::apply_mutators(SemanticAnalyzer* analyzer, const CallableVersion* version, std::vector<Value*> values) const {
	// std::cout << "Call::apply_mutators " << values.size() << std::endl;
	if (version->mutators.size()) {
		if (object) values.insert(values.begin(), object);
		version->apply_mutators(analyzer, values);
	}
}

Compiler::value Call::pre_compile_call(Compiler& c) const {
	assert(object != nullptr);
	if (object->isLeftValue()) {
		if (object->type->is_mpz_ptr()) {
			return ((LeftValue*) object)->compile_l(c);
		} else {
			return c.insn_load(((LeftValue*) object)->compile_l(c));
		}
	} else {
		return object->compile(c);
	}
}

Compiler::value Call::compile_call(Compiler& c, const CallableVersion* version, std::vector<Compiler::value> args, int flags) const {
	// std::cout << "Call::compile_call(" << args << ")" << std::endl;
	// Do the call
	auto r = version->compile_call(c, args, flags);
	if (object) {
		object->compile_end(c);
	}
	return r;
}

}