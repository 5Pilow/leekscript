#include "Call.hpp"
#include "../value/Value.hpp"
#include "Callable.hpp"
#include "../../standard/Module.hpp"
#include "../semantic/SemanticAnalyzer.hpp"
#include "../value/LeftValue.hpp"
#include "CallableVersion.hpp"
#include "../../type/Type.hpp"

namespace ls {

void Call::add_callable(Callable* callable) {
	callables.push_back(callable);
}

CallableVersion Call::resolve(SemanticAnalyzer* analyzer, std::vector<const Type*> arguments) const {
	// std::cout << "Call::resolve(" << arguments << ") object = " << (object ? object->type : analyzer->env.void_) << std::endl;
	if (object) {
		arguments.insert(arguments.begin(), object->type);
	}
	CallableVersion best { analyzer->env };
	int best_score = std::numeric_limits<int>::max();
	for (const auto& callable : callables) {
		for (auto v = 0; v < callable->versions.size(); ++v) {
			auto& version = callable->versions[v];
			if ((version.flags & Module::DEFAULT) != 0) continue;
			auto result = version.get_score(analyzer, arguments, callable, v);
			// std::cout << "version " << version << " score " << result.first << std::endl;
			if ((not best or result.first <= best_score) and result.first != std::numeric_limits<int>::max()) {
				best_score = result.first;
				best = result.second;
			}
		}
	}
	return best;
}

void Call::apply_mutators(SemanticAnalyzer* analyzer, CallableVersion& version, std::vector<Value*> values) const {
	// std::cout << "Call::apply_mutators " << values.size() << std::endl;
	if (version.template_()->mutators.size()) {
		if (object) values.insert(values.begin(), object);
		version.apply_mutators(analyzer, values);
	}
}

#if COMPILER
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

Compiler::value Call::compile_call(Compiler& c, const CallableVersion& version, std::vector<Compiler::value> args, int flags) const {
	// std::cout << "Call::compile_call(" << args << ")" << std::endl;
	// Do the call
	auto r = version.compile_call(c, args, value, flags);
	if (object) {
		object->compile_end(c);
	}
	return r;
}
#endif

}

namespace std {
	std::ostream& operator << (std::ostream& os, const ls::Call& v) {
		os << "Call: [" << std::endl;
		for (const auto& callable : v.callables) {
			os << callable << std::endl;
		}
		os << "] ";
		if (v.value) os << "value=\"" << v.value << "\" ";
		if (v.object) os << "object=\"" << v.object << "\" ";
		os << std::endl;
		return os;
	}
	std::ostream& operator << (std::ostream& os, const ls::Call* v) {
		os << *v;
		return os;
	}
}