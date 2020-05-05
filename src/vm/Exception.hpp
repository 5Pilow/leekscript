#ifndef EXCEPTION_HPP
#define EXCEPTION_HPP

#include <cmath>
#include <iostream>
#include <sstream>
#include <vector>

namespace ls {

class VM;

namespace vm {

enum Exception {
	DIVISION_BY_ZERO = -2,
	NO_EXCEPTION = 0,
	EXCEPTION = 1,
	OPERATION_LIMIT_EXCEEDED = 2,
	NUMBER_OVERFLOW = 3,
	NO_SUCH_OPERATOR = 4,
	ARRAY_OUT_OF_BOUNDS = 5,
	ARRAY_KEY_IS_NOT_NUMBER = 6,
	CANT_MODIFY_READONLY_OBJECT = 7,
	NO_SUCH_ATTRIBUTE = 8,
	WRONG_ARGUMENT_TYPE = 9
};

struct exception_frame {
	size_t line;
	std::string file;
	std::string function;
	void* frame;
	void* pc;
	exception_frame() {}
	exception_frame(std::string file, std::string function, size_t line) : line(line), file(file), function(function) {}
	bool operator == (const exception_frame& o) const {
		return file == o.file and line == o.line and function == o.function;
	}
};

struct ExceptionObj : public std::exception {
	Exception type;
	std::vector<exception_frame> frames;
	ExceptionObj() : ExceptionObj(NO_EXCEPTION) {}
	ExceptionObj(Exception type);

	~ExceptionObj();

	std::string to_string(bool colors = true) const;

	static std::string exception_message(vm::Exception expected);
};

}
}

#endif
