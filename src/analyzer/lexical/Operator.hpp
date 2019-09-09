#ifndef OPERATOR_HPP
#define OPERATOR_HPP

#include <memory>
#include "Token.hpp"

namespace ls {

class Operator {
public:

	Token* token;
	TokenType type;
	std::string character;
	int priority;
	bool reversed;

	Operator(Token* token);

	void print(std::ostream&);
};

}

#endif
