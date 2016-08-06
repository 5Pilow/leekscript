#include "SemanticException.hpp"
#include "../../util/Util.hpp"

using namespace std;

namespace ls {

bool SemanticException::translation_loaded = false;
Json SemanticException::translation;

SemanticException::SemanticException(Type type, unsigned line) {
	this->type = type;
	this->line = line;
	this->content = "";
}

SemanticException::SemanticException(Type type, unsigned line, const string& content) {
	this->type = type;
	this->line = line;
	this->content = content;
}

SemanticException::~SemanticException() {}

std::string SemanticException::message() const {

	return build_message(type, content);
}

std::string SemanticException::build_message(Type type, std::string token) {

	if (!translation_loaded) {
		translation = Json::parse(Util::read_file("src/doc/semantic_exception_fr.json"));
		translation_loaded = true;
	}

	std::string m = translation[type_to_string(type)];
	if (m.find("%s") != std::string::npos) {
		m = m.replace(m.find("%s"), 2, token);
	}
	return m;
}

std::string SemanticException::type_to_string(Type type) {
	switch (type) {
		case Type::UNDEFINED_VARIABLE: return "UNDEFINED_VARIABLE";
		case Type::VARIABLE_ALREADY_DEFINED: return "VARIABLE_ALREADY_DEFINED";
		case Type::METHOD_NOT_FOUND: return "METHOD_NOT_FOUND";
		case Type::STATIC_METHOD_NOT_FOUND: return "STATIC_METHOD_NOT_FOUND";
		case Type::CANT_ASSIGN_VOID: return "CANT_ASSIGN_VOID";
		case Type::CANNOT_CALL_VALUE: return "CANNOT_CALL_VALUE";
		case Type::BREAK_MUST_BE_IN_LOOP: return "BREAK_MUST_BE_IN_LOOP";
		case Type::CONTINUE_MUST_BE_IN_LOOP: return "CONTINUE_MUST_BE_IN_LOOP";
		case Type::ARRAY_ACCESS_KEY_MUST_BE_NUMBER: return "ARRAY_ACCESS_KEY_MUST_BE_NUMBER";
		case Type::ARRAY_ACCESS_RANGE_KEY_MUST_BE_NUMBER: return "ARRAY_ACCESS_RANGE_KEY_MUST_BE_NUMBER";
		case Type::INVALID_MAP_KEY: return "INVALID_MAP_KEY";
		case Type::VALUE_MUST_BE_A_LVALUE: return "VALUE_MUST_BE_A_LVALUE";
		case Type::MUST_RETURN_A_VALUE: return "MUST_RETURN_A_VALUE";

		default:
			return "UNKNOWN_ERROR";
	}
}

}

