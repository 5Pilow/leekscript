#include "LeftValue.hpp"

namespace ls {

bool LeftValue::isLeftValue() const {
	return true;
}
void LeftValue::change_value(SemanticAnalyzer*, Value* value) {}

}
