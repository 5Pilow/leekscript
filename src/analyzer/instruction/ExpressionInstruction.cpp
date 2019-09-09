#include "ExpressionInstruction.hpp"
#include "../value/Block.hpp"
#include "../value/If.hpp"

namespace ls {

ExpressionInstruction::ExpressionInstruction(std::unique_ptr<Value> value) : value(std::move(value)) {}

void ExpressionInstruction::print(std::ostream& os, int indent, bool debug, bool condensed) const {
	value->print(os, indent, debug, condensed);
}

Location ExpressionInstruction::location() const {
	return value->location();
}

void ExpressionInstruction::pre_analyze(SemanticAnalyzer* analyzer) {
	value->pre_analyze(analyzer);
}

void ExpressionInstruction::analyze(SemanticAnalyzer* analyzer, const Type* req_type) {
	// std::cout << "ExpressionInstruction::analyze() " << is_void << std::endl;
	value->is_void = is_void;
	value->analyze(analyzer);
	if (req_type->is_void()) {
		type = Type::void_;
	} else {
		type = value->type;
	}
	returning = value->returning;
	may_return = value->may_return;
	return_type = value->return_type;
	throws = value->throws;
}

Compiler::value ExpressionInstruction::compile(Compiler& c) const {
	auto v = value->compile(c);
	assert(!v.v or v.t->llvm(c) == v.v->getType());
	return v;
}

Compiler::value ExpressionInstruction::compile_end(Compiler& c) const {
	value->compile_end(c);
}

std::unique_ptr<Instruction> ExpressionInstruction::clone() const {
	return std::make_unique<ExpressionInstruction>(value->clone());
}

}
