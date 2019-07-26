#include "LeftValue.hpp"

namespace ls {

LeftValue::LeftValue(Environment& env) : Value(env) {}

bool LeftValue::isLeftValue() const {
	return true;
}
void LeftValue::change_value(SemanticAnalyzer*, Value* value) {}

}
