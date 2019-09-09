#ifndef CALLABLE_HPP_
#define CALLABLE_HPP_

#include <vector>
#include <iostream>

namespace ls {

class Value;
class CallableVersion;
class SemanticAnalyzer;
class Type;

class Callable {
public:
	std::vector<const CallableVersion*> versions;

	Callable() {}
	Callable(std::vector<const CallableVersion*> versions) : versions(versions) {}
	Callable(std::initializer_list<const CallableVersion*> versions) : versions(versions) {}

	const CallableVersion* resolve(SemanticAnalyzer* analyzer, std::vector<const Type*> arguments) const;
	bool is_compatible(int argument_count);
	void add_version(const CallableVersion* version);
};

}

namespace std {
	std::ostream& operator << (std::ostream&, const ls::Callable*);
}

#endif