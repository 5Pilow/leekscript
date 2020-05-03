#include "Exception.hpp"
#include "../colors.h"

namespace ls {
namespace vm {

ExceptionObj::~ExceptionObj() {
}

std::string ExceptionObj::to_string(bool colors) const {
	auto pad = [](std::string s, int l) {
		l -= s.size();
		while (l-- > 0) s = " " + s;
		return s;
	};
	size_t padding = 0;
	for (auto& f : frames) {
		padding = fmax(padding, f.function.size() + 2);
	}
	std::ostringstream oss;
	oss << "Exception " << (colors ? BOLD : "") << exception_message(type) << (colors ? END_COLOR : "") << std::endl;
	for (const auto& f : frames) {
		oss << (colors ? BOLD : "") << "    > " << (colors ? END_COLOR : "") << pad(f.function + "()", padding) << " @ " << (colors ? BOLD : "") << f.file << ":" << f.line << (colors ? END_COLOR : "");
		#if STACKTRACE_DETAILS
			oss << " (frame: " << f.frame << ", pc: " << f.pc << ")";
		#endif
		oss << std::endl;
	}
	return oss.str();
}

std::string ExceptionObj::exception_message(vm::Exception expected) {
	switch (expected) {
	case vm::Exception::EXCEPTION: return "exception";
	case vm::Exception::DIVISION_BY_ZERO: return "division_by_zero";
	case vm::Exception::OPERATION_LIMIT_EXCEEDED: return "too_much_operations";
	case vm::Exception::NUMBER_OVERFLOW: return "number_overflow";
	case vm::Exception::NO_EXCEPTION: return "no_exception";
	case vm::Exception::NO_SUCH_OPERATOR: return "no_such_operator";
	case vm::Exception::ARRAY_OUT_OF_BOUNDS: return "array_out_of_bounds";
	case vm::Exception::ARRAY_KEY_IS_NOT_NUMBER: return "array_key_is_not_a_number";
	case vm::Exception::CANT_MODIFY_READONLY_OBJECT: return "cant_modify_readonly_object";
	case vm::Exception::NO_SUCH_ATTRIBUTE: return "no_such_attribute";
	case vm::Exception::WRONG_ARGUMENT_TYPE: return "wrong_argument_type";
	}
	return "??" + std::to_string((int) expected) + "??";
}

}
}
