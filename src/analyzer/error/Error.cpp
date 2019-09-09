#include "Error.hpp"
#include "../../util/Util.hpp"

namespace ls {

bool Error::translation_loaded = false;
Json Error::translation;

Error::Error(Type type, File* file, int line, int character) : type(type), location(file, Position(line, character, 0), Position(line, character + 1, 0)), focus(file, Position(line, character, 0), Position(line, character + 1, 0)) {}

Error::Error(Type type, Token* token, std::vector<std::string> parameters) : type(type), location(token->location), focus(token->location), parameters(parameters) {}

Error::Error(Type type, Location location, Location focus) : type(type), location(location), focus(focus) {}

Error::Error(Type type, Location location, Location focus, std::vector<std::string> parameters) : type(type), location(location), focus(focus), parameters(parameters) {}

Error::~Error() {}

std::string Error::message() const {
	return build_message(type, parameters);
}

Json Error::json() const {
	return {focus.start.line, focus.start.column, focus.end.line, focus.end.column, build_message(type, parameters)};
}

std::string Error::build_message(Type type, std::vector<std::string> parameters) {

	if (!translation_loaded) {
		try {
			translation = Json::parse(Util::read_file("src/doc/error_fr.json"));
		} catch (std::exception&) {} // LCOV_EXCL_LINE
		translation_loaded = true;
	}

	try {
		std::string m = translation[type_to_string(type)];
		size_t pos = 0;
		size_t i = 0;
		while ((pos = m.find("%", pos + 1)) != std::string::npos) {
			if (i < parameters.size()) {
				m = m.replace(pos, 2, parameters[i]);
			}
			i++;
		}
		return m;
	} catch (std::exception&) { // LCOV_EXCL_LINE
		return type_to_string(type); // LCOV_EXCL_LINE
	}
}

std::string Error::type_to_string(Type type) {
	switch (type) {
		// Lexical
		case Type::UNTERMINATED_STRING: return "UNTERMINATED_STRING";
		case Type::UNKNOWN_ESCAPE_SEQUENCE: return "UNKNOWN_ESCAPE_SEQUENCE";
		case Type::NUMBER_INVALID_REPRESENTATION: return "NUMBER_INVALID_REPRESENTATION";
		// Syntaxic
		case Type::BLOCK_NOT_CLOSED: return "BLOCK_NOT_CLOSED";
		case Type::BREAK_LEVEL_ZERO: return "BREAK_LEVEL_ZERO";
		case Type::CONTINUE_LEVEL_ZERO: return "CONTINUE_LEVEL_ZERO";
		case Type::EXPECTED_VALUE: return "EXPECTED_VALUE";
		case Type::UNEXPECTED_TOKEN: return "UNEXPECTED_TOKEN";
		// Semantic
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
		case Type::WRONG_ARGUMENT_COUNT: return "WRONG_ARGUMENT_COUNT";
		case Type::NO_SUCH_OPERATOR: return "NO_SUCH_OPERATOR";
		case Type::CANT_MODIFY_CONSTANT_VALUE: return "CANT_MODIFY_CONSTANT_VALUE";
		case Type::VALUE_NOT_ITERABLE: return "VALUE_NOT_ITERABLE";
		case Type::NO_SUCH_ATTRIBUTE: return "NO_SUCH_ATTRIBUTE";
		case Type::VALUE_MUST_BE_A_CONTAINER: return "VALUE_MUST_BE_A_CONTAINER";
		default:
			return "UNKNOWN_ERROR";
	}
}

}
