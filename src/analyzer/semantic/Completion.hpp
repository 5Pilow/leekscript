#ifndef COMPLETION_HPP
#define COMPLETION_HPP

#include "../lexical/Location.hpp"

namespace ls {

class Environment;
class Type;

enum class CompletionType {
	METHOD,
	FIELD,
	VARIABLE
};

class CompletionItem {
public:
	std::string name;
	CompletionType type;
	const Type* lstype;
	Location location;
};

class Completion {
public:
	const Type* type;
	std::vector<CompletionItem> items;

	Completion(Environment&);
	Completion(const Type*);
};

}

#endif