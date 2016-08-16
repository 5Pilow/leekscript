#include "Value.hpp"
#include "../../vm/Type.hpp"

namespace ls {

Value::Value() {
	type = Type::UNKNOWN;
	constant = false;
}

Value::~Value() {}

bool Value::will_take(SemanticAnalyser*, const std::vector<Type>& args_type) {
	return type.will_take(args_type);
}

bool Value::will_take_element(SemanticAnalyser*, const Type arg_type) {
	return type.will_take_element(arg_type);
}

bool Value::must_be_pointer(SemanticAnalyser*) {
	if (type.nature == Nature::POINTER) {
		return false;
	}
	type.nature = Nature::POINTER;
	return true;
}

void Value::must_return(SemanticAnalyser*, const Type& ret_type) {
	type.setReturnType(ret_type);
}

bool Value::isLeftValue() const {
	return false;
}

std::string Value::tabs(int indent) {
	return std::string(indent * 4, ' ');
}

}
