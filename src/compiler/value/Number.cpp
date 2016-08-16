#include "../../compiler/value/Number.hpp"

#include "../../vm/VM.hpp"
#include <limits.h>
#include "../../vm/value/LSNumber.hpp"

using namespace std;

namespace ls {

Number::Number(double value, Token* token) {
	this->value = value;
	this->token = token;
	constant = true;
}

Number::~Number() {
}

void Number::print(ostream& os, int, bool debug) const {
	os << value;
	if (debug) {
		os << " " << type;
	}
}

unsigned Number::line() const {
	return token->line;
}

void Number::analyse(SemanticAnalyser*, const Type& req_type) {

	if (value != (int) value or req_type.raw_type == RawType::FLOAT) {
		type = Type::FLOAT;
	} else {
		type = Type::INTEGER;
	}

	if (req_type.nature != Nature::UNKNOWN) {
		type.nature = req_type.nature;
	}
}

jit_value_t Number::compile(Compiler& c) const {

	if (type.nature == Nature::POINTER) {

		jit_value_t val = JIT_CREATE_CONST_FLOAT(c.F, JIT_FLOAT, value);
		return VM::value_to_pointer(c.F, val, Type::FLOAT);

	} else {

		bool isfloat = type.raw_type == RawType::FLOAT;
		jit_type_t type = isfloat ? JIT_FLOAT : JIT_INTEGER;

		if (isfloat) {
			return JIT_CREATE_CONST_FLOAT(c.F, type, value);
		} else {
			return JIT_CREATE_CONST(c.F, type, value);
		}
	}
}

}
