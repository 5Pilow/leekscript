#ifndef FILE_HPP
#define FILE_HPP

#include "FileContext.hpp"
#include "../error/Error.hpp"

namespace ls {

class File {
public:
	std::string path;
	std::string code;
	FileContext context;
	std::vector<Error> errors;
	std::vector<Token> tokens;
	Token finished_token;
	std::vector<std::unique_ptr<File>> included_files;

	File(std::string path, std::string code, FileContext context) : finished_token({ TokenType::FINISHED, this, 0, 0, 0, "" }) {
		this->path = path;
		this->code = code;
		this->context = context;
	}
};

}

#endif