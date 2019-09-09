#ifndef LEXICALanalyzer_H_
#define LEXICALanalyzer_H_

#include <vector>
#include <string>
#include "Token.hpp"
#include "../resolver/File.hpp"

namespace ls {

enum class LetterType {
	LETTER, NUMBER, QUOTE, DOUBLE_QUOTE, WHITE, OTHER
};

class LexicalAnalyzer {
public:

	File* file;
	static std::unordered_map<std::string, TokenType> token_map;
	
	LexicalAnalyzer();

	LetterType getLetterType(unsigned char c, unsigned char nc);
	bool isToken(const std::string& word);
	TokenType getTokenType(const std::string& word, TokenType by_default);
	std::vector<Token> parseTokens(const std::string& code);

	std::vector<Token> analyze(File* file);
};

}

#endif
