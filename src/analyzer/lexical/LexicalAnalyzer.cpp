#include "LexicalAnalyzer.hpp"
#include <iostream>
#include "../../util/utf8.h"
#include <string.h>
#include "../error/Error.hpp"

namespace ls {

std::unordered_map<std::string, TokenType> LexicalAnalyzer::token_map;

static std::vector<std::vector<std::string>> type_literals = {

	{}, // Unknown
	{}, // Finished
	{}, // Don't care

	{}, // Number
	{}, // Ident
	{}, // String

	{ "let" }, { "global" }, { "function" }, { "return" },
	{ "break" }, { "continue" },

	{ "!=", "is not", "≠" }, { "=" },

	{ "!", "not" }, { "null" }, { "true" }, { "false" }, { "/*" },
	{ "*/" }, { "(" }, { ")" }, { "[" }, { "]" }, { "{" }, { "}" },

	{ "<=>", "<->" },

	{ "@" },

	{ "===" }, { "==" }, { "!==" },
	{ "+" }, { "-" }, { "*", "×" }, { "/", "÷" },

	{ "++" }, { "--" },

	{ "+=" }, {"-=" }, { "/=", "÷=" }, { "*=", "×=" },

	{ ";" },

	{ ">" }, {	">=", "≥" }, { "<" }, { "<=", "≤" },

	{ "->", "=>", "→" }, { "," }, { ":" }, { "if" }, { "else" }, { "match" }, { "for" }, { "in" },
	{ "while" }, { "do" }, { "?" },

	{ "or", "||" }, { "and", "&&" }, { "xor" },

	{ "." }, { "|" },

	{ "**" }, { "**=" }, { "%=" }, {"%" },

	{ "class" }, { "new" }, { "parent" }, { "then" },
	{ "end" }, { "?:" }, { "!!" },

	{ "~" }, { "~=" }, { "~~" }, { "~~=" },

	{ "π" },

	{ ".." }, { "..." },

	{ "is" },

	{ "&" }, { "&=" },
	{ "|=" },
	{ "^" }, { "^=" },
	{ "<<" }, { "<<=" },
	{ ">>" }, { ">>=" },
	{ ">>>" }, { ">>>=" },
	{ "^<" }, { "^<=" },
	{ "^>" }, { "^>=" },
	{ "??" },
	{ "\\" },
	{ "throw" },
	{ "var" },
	{ "$" },
	{ "\\=" },
	{ "!?" },
	{ "%%" }, { "%%=" }
};

LexicalAnalyzer::LexicalAnalyzer() {
	// Build token cache map
	if (not token_map.size()) {
		for (size_t j = 0; j < type_literals.size(); ++j) {
			for (const auto& text : type_literals[j]) {
				token_map.insert({ text, (TokenType) j });
			}
		}
	}
}

LetterType LexicalAnalyzer::getLetterType(unsigned char c, unsigned char nc) {
	if (c == '\'') {
		return LetterType::QUOTE;
	}
	if (c == '"') {
		return LetterType::DOUBLE_QUOTE;
	}
	if (c >= '0' && c <= '9') {
		return LetterType::NUMBER;
	}
	if (c == ' ' || c == '	' || c == '\n' || (c == 0xC2 and nc == 0xA0) /* unbreakable space */) {
		return LetterType::WHITE;
	}
	if (c == '_' || (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c >= 0xc0) {
		return LetterType::LETTER;
	}
	return LetterType::OTHER;
}

TokenType LexicalAnalyzer::getTokenType(const std::string& word, TokenType by_default) {
	auto i = token_map.find(word);
	if (i != token_map.end()) return i->second;
	return by_default;
}
bool LexicalAnalyzer::isToken(const std::string& word) {
	return token_map.find(word) != token_map.end();
}

std::vector<Token> LexicalAnalyzer::analyze(File* file) {

	this->file = file;
	auto tokens = LexicalAnalyzer::parseTokens(file->code + " ");

	tokens.push_back({ TokenType::FINISHED, file, 0, 0, 1, "" });

	for (size_t i = 0; i < tokens.size(); ++i) {
		if (i + 1 < tokens.size() && tokens[i].content == "is" && tokens[i + 1].content == "not") {
			tokens[i].type = TokenType::DIFFERENT;
			tokens[i].content = "is not";
			tokens.erase(tokens.begin() + i + 1);
		}
	}

	return tokens;
}

std::vector<Token> LexicalAnalyzer::parseTokens(const std::string& code) {

	const char* string_chars = code.c_str();
	std::vector<Token> tokens;

	int line = 1;
	int character = 0;
	std::string word = "";
	bool ident = false;
	bool number = false;
	bool string1 = false;
	bool string2 = false;
	bool other = false;
	bool escape = false;
	bool hex = false;
	bool bin = false;
	int comment = 0;
	bool lineComment = false;

	int l = strlen(string_chars);
	int h = 0, i = 0, j = 0;
	char c, nc = code[j];
	u8_inc(string_chars, &j);

	while (i < l) {

		character++;
		c = nc;
		h = i;
		i = j;
		nc = code[j];
		u8_inc(string_chars, &j);
		auto type = getLetterType(c, code[h + 1]);
		// std::cout << "c = " << c << " nc = " << nc << " " << code.substr(h, i - h) << " " << (int)(unsigned char) c << " " << (int)(unsigned char) nc << " " << (int) type << std::endl;

		if (lineComment) {
			if (c == '\n') {
				lineComment = false;
			}
		} else {
			if (c == '/' && nc == '/') {
				lineComment = true;
			} else if (c == '/' && nc == '*') {
				comment++;
			} else if (c == '*' && nc == '/') {
				comment--;
			} else if (comment == 0) {
				if (type == LetterType::WHITE) {
					if (ident) {
						tokens.push_back({ getTokenType(word, TokenType::IDENT), file, i, line, character, word });
						ident = false;
					} else if (number) {
						if ((bin || hex) && word.size() == 2) {
							file->errors.push_back({Error::Type::NUMBER_INVALID_REPRESENTATION, file, line, character});
						}
						tokens.push_back({ TokenType::NUMBER, file, i, line, character, word });
						number = bin = hex = false;
					} else if (string1 || string2) {
						if (escape) {
							escape = false;
							file->errors.push_back({Error::Type::UNKNOWN_ESCAPE_SEQUENCE, file, line, character});
						}
						word += code.substr(h, i - h);
					} else if (other) {
						tokens.push_back({ getTokenType(word, TokenType::UNKNOW), file, i, line, character, word });
						other = false;
					}
				} else if (type == LetterType::LETTER) {
					if (ident || string1 || string2) {
						if (escape) {
							escape = false;
							if (c == 'b') {
								word += '\b'; // used either to erase the last character printed or to overprint it.
							} else if (c == 'f') {
								word += '\f'; // to cause a printer to eject paper to the top of the next page, or a video terminal to clear the screen.
							} else if (c == 'n') {
								word += '\n'; // used as the end of line marker in most UNIX systems and variants.
							} else if (c == 'r') {
								word += '\r'; // used as the end of line marker in Classic Mac OS, OS-9, FLEX (and variants). A carriage return/line feed pair is used by CP/M-80 and its derivatives including DOS and Windows, and by Application Layer protocols such as HTTP.
							} else if (c == 't') {
								word += '\t'; // moves the printing position some spaces to the right.
							} else {
								file->errors.push_back({Error::Type::UNKNOWN_ESCAPE_SEQUENCE, file, line, character});
							}
						} else {
							word += code.substr(h, i - h);
						}
					} else if (number) {
						if (word == "0" && (c == 'x' || c == 'b')) {
							hex = c == 'x';
							bin = c == 'b';
							word += code.substr(h, i - h);
						} else if (hex && (c <= 'F' || (c >= 'a' && c <= 'f'))) {
							word += code.substr(h, i - h);
						} else if (c == 'l' or c == 'L') {
							word += "l";
							tokens.push_back({ TokenType::NUMBER, file, i + 1, line, character + 1, word });
							number = bin = hex = false;
							word = "";
						} else if (c == 'm' or c == 'M') {
							word += "m";
							tokens.push_back({ TokenType::NUMBER, file, i + 1, line, character + 1, word });
							number = bin = hex = false;
							word = "";
						} else if (c == '$') {
							tokens.push_back({ TokenType::NUMBER, file, i, line, character, word });
							number = bin = hex = false;
							word = "";
							tokens.push_back({ TokenType::STAR, file, i, line, character, code.substr(h, i - h) });
						} else {
							file->errors.push_back({Error::Type::NUMBER_INVALID_REPRESENTATION, file, line, character});
							tokens.push_back({ TokenType::NUMBER, file, i, line, character, word });
							number = bin = hex = false;
						}
					} else if (other) {
						tokens.push_back({ getTokenType(word, TokenType::UNKNOW), file, i, line, character, word });
						other = false;
						ident = true;
						word = code.substr(h, i - h);
					} else {
						ident = true;
						word = code.substr(h, i - h);
					}
				} else if (type == LetterType::NUMBER) {
					if (number) {
						if (bin && c > '1') {
							file->errors.push_back({Error::Type::NUMBER_INVALID_REPRESENTATION, file, line, character});
						} else {
							word += code.substr(h, i - h);
						}
					} else if (ident || string1 || string2) {
						if (escape) {
							escape = false;
							file->errors.push_back({Error::Type::UNKNOWN_ESCAPE_SEQUENCE, file, line, character});
						}
						word += code.substr(h, i - h);
					} else if (other) {
						tokens.push_back({ getTokenType(word, TokenType::UNKNOW), file, i, line, character, word });
						other = false;
						number = true;
						word = code.substr(h, i - h);
					} else {
						number = true;
						word = code.substr(h, i - h);
					}
				} else if (type == LetterType::QUOTE) {
					if (ident) {
						tokens.push_back({ getTokenType(word, TokenType::IDENT), file, i, line, character, word });
						ident = false;
						string1 = true;
						word = "";
					} else if (number) {
						if ((bin || hex) && word.size() == 2) {
							file->errors.push_back({Error::Type::NUMBER_INVALID_REPRESENTATION, file, line, character});
						}
						tokens.push_back({ TokenType::NUMBER, file, i, line, character, word });
						number = bin = hex = false;
						string1 = true;
						word = "";
					} else if (string2 || (string1 && escape)) {
						escape = false;
						word += code.substr(h, i - h);
					} else if (string1) {
						tokens.push_back({ TokenType::STRING, file, i, line, character, word });
						string1 = false;
					} else if (other) {
						tokens.push_back({ getTokenType(word, TokenType::UNKNOW), file, i, line, character, word });
						other = false;
						string1 = true;
						word = "";
					} else {
						string1 = true;
						word = "";
					}
				} else if (type == LetterType::DOUBLE_QUOTE) {
					if (ident) {
						tokens.push_back({ getTokenType(word, TokenType::IDENT), file, i, line, character, word });
						ident = false;
						string2 = true;
						word = "";
					} else if (number) {
						if ((bin || hex) && word.size() == 2) {
							file->errors.push_back({Error::Type::NUMBER_INVALID_REPRESENTATION, file, line, character});
						}
						tokens.push_back({ TokenType::NUMBER, file, i, line, character, word });
						number = bin = hex = false;
						string2 = true;
						word = "";
					} else if (string1) {
						if (escape) {
							escape = false;
							word += '\\';
						}
						word += code.substr(h, i - h);
					} else if (string2 && escape) {
						escape = false;
						word += code.substr(h, i - h);
					} else if (string2) {
						tokens.push_back({ TokenType::STRING, file, i, line, character, word });
						string2 = false;
					} else if (other) {
						tokens.push_back({ getTokenType(word, TokenType::UNKNOW), file, i, line, character, word });
						other = false;
						string2 = true;
						word = "";
					} else {
						string2 = true;
						word = "";
					}
				} else if (type == LetterType::OTHER) {
					if (ident) {
						tokens.push_back({ getTokenType(word, TokenType::IDENT), file, i, line, character, word });
						ident = false;
						other = true;
						word = code.substr(h, i - h);
					} else if (number) {
						if (!hex && !bin && c == '.' && word.find('.') == std::string::npos && getLetterType(nc, 0) == LetterType::NUMBER) {
							word += code.substr(h, i - h);
						} else {
							if ((bin || hex) && word.size() == 2) {
								file->errors.push_back({Error::Type::NUMBER_INVALID_REPRESENTATION, file, line, character});
							}
							tokens.push_back({ TokenType::NUMBER, file, i, line, character, word });
							number = bin = hex = false;
							other = true;
							word = code.substr(h, i - h);
						}
					} else if (string1 || string2) {
						if (escape && c != '\\') {
							escape = false;
							file->errors.push_back({Error::Type::UNKNOWN_ESCAPE_SEQUENCE, file, line, character});
						}
						if (!escape && c == '\\') {
							escape = true;
						} else {
							escape = false;
							word += code.substr(h, i - h);
						}
					} else if (other) {
						auto buff = code.substr(h, i - h);
						if ((c == '!' and word == "!") or not isToken(word + buff)) {
							tokens.push_back({ getTokenType(word, TokenType::UNKNOW), file, i, line, character, word });
							word = buff;
						} else {
							word += buff;
						}
					} else {
						word = code.substr(h, i - h);
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
	if (string1 or string2) {
		file->errors.push_back({Error::Type::UNTERMINATED_STRING, file, line, character});
	}
	return tokens;
}

}
