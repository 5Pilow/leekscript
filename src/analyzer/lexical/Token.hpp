#ifndef TOKEN_H_
#define TOKEN_H_

#include <string>
#include "Location.hpp"
#include "TokenType.hpp"

namespace ls {

class File;

class Token {

public:

	TokenType type;
	std::string content;
	Location location;
	unsigned size;

	Token(TokenType type, File* file, size_t raw, size_t line, size_t character, const std::string& content);
};

std::ostream& operator << (std::ostream& os, Token& var);

}

#endif
