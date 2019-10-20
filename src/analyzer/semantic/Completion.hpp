#ifndef COMPLETION_HPP
#define COMPLETION_HPP

namespace ls {

class Type;

enum class CompletionType {
	METHOD,
	FIELD
};

class Completion {
public:
	std::string name;
	CompletionType type;
	const Type* lstype;
};

}

#endif