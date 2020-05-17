#ifndef FILE_HPP
#define FILE_HPP

#include "FileContext.hpp"
#include "../error/Error.hpp"
#include <unordered_map>
#include <unordered_set>
#include "../lexical/Todo.hpp"

namespace ls {

class Program;

class File {
public:
	std::string path;
	std::string code;
	FileContext context;
	std::vector<Error> errors;
	std::vector<Token> tokens;
	Token finished_token;
	std::vector<File*> included_files;
	std::unordered_map<std::string, File*> includers_files;
	Program* program;
	bool entrypoint = false;
	File* waiting = nullptr;
	bool tokens_read = false;
	std::vector<File*> waiters;
	std::vector<Todo> todos;
	std::unordered_set<File*> entrypoints;

	File(std::string path, std::string code, FileContext context, Program* program) : finished_token({ TokenType::FINISHED, this, 0, 0, 0, "" }) {
		this->path = path;
		this->code = code;
		this->context = context;
		this->program = program;
	}
};

}

#endif