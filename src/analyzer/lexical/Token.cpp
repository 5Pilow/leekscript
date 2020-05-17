#include "Token.hpp"
#include <iostream>
#include "LexicalAnalyzer.hpp"
#include "../../util/Util.hpp"

namespace ls {

Token::Token(TokenType type, File* file, size_t raw, size_t line, size_t character, const std::string& content)
 : type(type), content(content), location(file, {line, character - content.size() - 1, raw - content.size() - 1}, {line, character - 2, raw - 2}) {

	// TODO legacy only
	auto word_lower = Util::tolower(content);
	bool ignore_case = LexicalAnalyzer::ignored_case_legacy.find(word_lower) != LexicalAnalyzer::ignored_case_legacy.end();
	if (ignore_case) {
		this->content = word_lower;
	}

	if (type == TokenType::STRING) {
		this->location.start.column--;
		this->location.start.raw--;
		this->location.end.raw++;
		this->location.end.column++;
		this->size = content.size() + 2;
	} else {
		this->size = content.size();
	}
}

}
