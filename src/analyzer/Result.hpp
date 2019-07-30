#ifndef RESULT_HPP
#define RESULT_HPP

#include <vector>
#include <string>
#include "error/Error.hpp"

namespace ls {

class Type;

struct Result {
	bool analyzed = false;
	bool compilation_success = false;
	bool execution_success = false;
	std::vector<Error> errors;
	std::string program = "";
	std::string value = "";
	double parse_time = 0;
	double compilation_time = 0;
	double execution_time = 0;
	long operations = 0;
	int objects_created = 0;
	int objects_deleted = 0;
	int mpz_objects_created = 0;
	int mpz_objects_deleted = 0;
	std::string assembly;
	std::string pseudo_code;
	const Type* type = nullptr;
	#if COMPILER
	vm::ExceptionObj exception;
	#endif
};

}

#endif