#include "Callable.hpp"
#include "../semantic/SemanticAnalyzer.hpp"
#include "../../standard/Module.hpp"
#include "CallableVersion.hpp"
#include "../../type/Type.hpp"

namespace ls {

void Callable::add_version(CallableVersionTemplate v) {
	versions.push_back(v);
}

const CallableVersion* Callable::resolve(SemanticAnalyzer* analyzer, std::vector<const Type*> arguments) const {
	return nullptr;
}

bool Callable::is_compatible(int argument_count) {
	for (const auto& version : versions) {
		if (version.type->arguments().size() == (size_t) argument_count) return true;
	}
	return false;
}

}

namespace std {
	std::ostream& operator << (std::ostream& os, const ls::Callable* callable) {
		os << "[" << std::endl;
		for (const auto& v : callable->versions) {
			os << "    " << v << std::endl;
		}
		os << "]";
		return os;
	}
}