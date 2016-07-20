#include "../../compiler/lexical/LexicalAnalyser.hpp"

#include <iostream>
#include "../../../lib/utf8.h"
#include <string.h>

using namespace std;

namespace ls {

static vector<vector<string>> type_literals = {

	{}, // Unknown
	{}, // Finished
	{}, // Don't care

	{}, // Number
	{}, // Ident
	{}, // String

	{ "let", "var" }, { "global" }, { "fn" }, { "return" },
	{ "break" }, { "continue" },

	{ "!=", "is not", "≠" }, { "=" },

	{ "!", "not" }, { "null" }, { "true" }, { "false" }, { "/*" },
	{ "*/" }, { "(" }, { ")" }, { "[" }, { "]" }, { "{" }, { "}" },

	{ "<=>", "<->" },

	{ "@" },

	{ "===" }, { "==", "is" }, { "!==" },
	{ "+" }, { "-" }, { "*", "×" }, { "/", "÷" },

	{ "++" }, { "--" },

	{ "+=" }, {"-=" }, { "/=", "÷=" }, { "*=", "×=" },

	{ ";" },

	{ ">" }, {	">=", "≥" }, { "<" }, { "<=", "≤" },

	{ "->", "=>", "→" }, { "," }, { ":" }, { "if" }, { "else" }, { "for" }, { "in" },
	{ "while" }, { "do" }, { "?" },

	{ "or", "||" }, { "and", "&&" }, { "xor" },

	{ "." }, { "|" },

	{ "**" }, { "**=" }, { "%=" }, {"%" },

	{ "class" }, { "new" }, { "parent" }, { "then" },
	{ "end" }, { "?:" }, { "!!" },

	{ "~" }, { "~=" }, { "~~" }, { "~~=" },

	{ "π" },

	{ ".." }, { "..." },

	{ "instanceof" },

	{ "&" }, { "&=" },
	{ "|=" },
	{ "^" }, { "^=" },
	{ "<<" }, { "<<=" },
	{ ">>" }, { ">>=" },
	{ ">>>" }, { ">>>=" },
	{ "^<" }, { "^<=" },
	{ "^>" }, { "^>=" },

	{ "??" }
};

LexicalAnalyser::LexicalAnalyser() {}

LetterType LexicalAnalyser::getLetterType(char32_t c) {

	if (c == '\'') {
		return LetterType::QUOTE;
	}
	if (c == '"') {
		return LetterType::DOUBLE_QUOTE;
	}
	if (c >= '0' && c <= '9') {
		return LetterType::NUMBER;
	}
	if (c == ' ' || c == '	' || c == '\n') {
		return LetterType::WHITE;
	}
	if (c == '_' || (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c >= 0xc0) {
		return LetterType::LETTER;
	}

	return LetterType::OTHER;
}

TokenType LexicalAnalyser::getTokenType(string word, TokenType by_default)
{
	for (size_t j = 0; j < type_literals.size(); ++j) {
		for (string text : type_literals[j]) {
			if (word == text) return (TokenType) j;
		}
	}
	return by_default;
}

vector<Token> LexicalAnalyser::analyse(std::string code) {

	vector<Token> tokens = LexicalAnalyser::parseTokens(code + " ");

	tokens.push_back(Token(TokenType::FINISHED, 0, 1, ""));

	for (size_t i = 0; i < tokens.size(); ++i) {
		if (i + 1 < tokens.size() && tokens[i].content == "is" && tokens[i+1].content == "not") {
			tokens[i].type = TokenType::DIFFERENT;
			tokens[i].content = "is not";
			tokens.erase(tokens.begin() + i + 1);
		}
	}

	return tokens;
}

vector<Token> LexicalAnalyser::parseTokens(string code) {

	char buff[5];
	char* string_chars = (char*) code.c_str();

	vector<Token> tokens;

	int line = 1;
	int character = 1;
	string word = "";
	bool ident = false;
	bool number = false;
	bool string1 = false;
	bool string2 = false;
	bool other = false;
	int comment = 0;
	bool lineComment = false;

	int i = 0;
	int l = strlen(string_chars);

	while (i < l) {

		character++;

		u_int32_t c = u8_nextchar(string_chars, &i);
		int oldi = i;
		u_int32_t nc = u8_nextchar(string_chars, &i);
		i = oldi;

		LetterType type = getLetterType(c);

		if (lineComment) {
			if (c == '\n') {
				lineComment = false;
			}
		} else {

			if (c == '/' && nc == '/') {
				lineComment = true;
			} else if (c == '/' && nc == '*') {
				comment++;
				i++;
			} else if (c == '*' && nc == '/') {
				comment--;
				i++;

			} else if (comment == 0) {

				if (type == LetterType::WHITE) {

					if (ident) {
						tokens.push_back(Token(getTokenType(word, TokenType::IDENT), line, character, word));
						ident = false;
					} else if (number) {
						tokens.push_back(Token(TokenType::NUMBER, line, character, word));
						number = false;
					} else if (string1 || string2) {
						u8_toutf8(buff, 5, &c, 1);
						word += buff;
					} else if (other) {
						tokens.push_back(Token(getTokenType(word, TokenType::UNKNOW), line, character, word));
						other = false;
					}

				} else if (type == LetterType::LETTER) {

					if (ident || string1 || string2) {
						u8_toutf8(buff, 5, &c, 1);
						word += buff;
					} else if (number) {
						tokens.push_back(Token(TokenType::NUMBER, line, character, word));
						number = false;
					} else if (other) {
						tokens.push_back(Token(getTokenType(word, TokenType::UNKNOW), line, character, word));
						other = false;
						ident = true;
						u8_toutf8(buff, 5, &c, 1);
						word = buff;
					} else {
						ident = true;
						u8_toutf8(buff, 5, &c, 1);
						word = buff;
					}

				} else if (type == LetterType::NUMBER) {

					if (number || ident || string1 || string2) {
						u8_toutf8(buff, 5, &c, 1);
						word += buff;
					} else if (other) {
						tokens.push_back(Token(getTokenType(word, TokenType::UNKNOW), line, character, word));
						other = false;
						number = true;
						u8_toutf8(buff, 5, &c, 1);
						word = buff;
					} else {
						number = true;
						u8_toutf8(buff, 5, &c, 1);
						word = buff;
					}

				} else if (type == LetterType::QUOTE) {

					if (ident) {
						tokens.push_back(Token(getTokenType(word, TokenType::IDENT), line, character, word));
						ident = false;
						string1 = true;
						word = "";
					} else if (number) {
						tokens.push_back(Token(TokenType::NUMBER, line, character, word));
						number = false;
						string1 = true;
						word = "";
					} else if (string1) {
						tokens.push_back(Token(TokenType::STRING, line, character, word));
						string1 = false;
					} else if (string2) {
						u8_toutf8(buff, 5, &c, 1);
						word += buff;
					} else if (other) {
						tokens.push_back(Token(getTokenType(word, TokenType::UNKNOW), line, character, word));
						other = false;
						string1 = true;
						word = "";
					} else {
						string1 = true;
						word = "";
					}

				} else if (type == LetterType::DOUBLE_QUOTE) {

					if (ident) {
						tokens.push_back(Token(getTokenType(word, TokenType::IDENT), line, character, word));
						ident = false;
						string2 = true;
						word = "";
					} else if (number) {
						tokens.push_back(Token(TokenType::NUMBER, line, character, word));
						number = false;
						string2 = true;
						word = "";
					} else if (string1) {
						u8_toutf8(buff, 5, &c, 1);
						word += buff;
					} else if (string2) {
						tokens.push_back(Token(TokenType::STRING, line, character, word));
						string2 = false;
					} else if (other) {
						tokens.push_back(Token(getTokenType(word, TokenType::UNKNOW), line, character, word));
						other = false;
						string2 = true;
						word = "";
					} else {
						string2 = true;
						word = "";
					}

				} else if (type == LetterType::OTHER) {

					if (ident) {
						tokens.push_back(Token(getTokenType(word, TokenType::IDENT), line, character, word));
						ident = false;
						other = true;
						u8_toutf8(buff, 5, &c, 1);
						word = buff;
					} else if (number) {
						if (c == '.' && word.find('.') == string::npos && getLetterType(nc) == LetterType::NUMBER) {
							u8_toutf8(buff, 5, &c, 1);
							word += buff;
						} else {
							tokens.push_back(Token(TokenType::NUMBER, line, character, word));
							number = false;
							other = true;
							u8_toutf8(buff, 5, &c, 1);
							word = buff;
						}
					} else if (string1 || string2) {
						u8_toutf8(buff, 5, &c, 1);
						word += buff;
					} else if (other) {
						bool is_longer = false;
						for (size_t j = 0; j < type_literals.size() && !is_longer; ++j) {
							for (string text : type_literals[j]) {
								if (text.size() > word.size() && text.compare(0, word.size(), word) == 0 && text[word.size()] == (char) c) {
									is_longer = true;
									break;
								}
							}
						}

						if (!is_longer) {
							tokens.push_back(Token(getTokenType(word, TokenType::UNKNOW), line, character, word));
							u8_toutf8(buff, 5, &c, 1);
							word = buff;
						} else {
							u8_toutf8(buff, 5, &c, 1);
							word += buff;
						}

					} else {
						u8_toutf8(buff, 5, &c, 1);
						word = buff;
						other = true;
					}

				}
			}
		}
		if (c == '\n') {
			line++;
			character = 0;
		}
	}
	return tokens;
}

}
