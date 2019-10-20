#ifndef CALLABLE_HPP_
#define CALLABLE_HPP_

#include <vector>
#include <iostream>
#include <memory>
#include "CallableVersion.hpp"

namespace ls {

class Value;
class SemanticAnalyzer;
class Type;

class Callable {
public:
	std::vector<CallableVersion> versions;

	Callable() {}
	Callable(std::initializer_list<CallableVersion> versions) : versions(versions) {}
	// Callable(std::initializer_list<std::unique_ptr<const CallableVersion>> versions) : versions(versions) {}

	const CallableVersion* resolve(SemanticAnalyzer* analyzer, std::vector<const Type*> arguments) const;
	bool is_compatible(int argument_count);
	void add_version(CallableVersion version);
};

}

namespace std {
	std::ostream& operator << (std::ostream&, const ls::Callable*);
}

#endif