#include "AbsoluteValue.hpp"
#include "../../vm/LSValue.hpp"
#include "../../type/Type.hpp"

namespace ls {

AbsoluteValue::AbsoluteValue() {
	type = Type::integer;
	throws = true;
}

void AbsoluteValue::print(std::ostream& os, int indent, PrintOptions options) const {
	os << "|";
	expression->print(os, indent, options);
	os << "|";
	if (options.debug) {
		os << " " << type;
	}
}

Location AbsoluteValue::location() const {
	return {open_pipe->location.file, open_pipe->location.start, close_pipe->location.end};
}

void AbsoluteValue::pre_analyze(SemanticAnalyzer* analyzer) {
	expression->pre_analyze(analyzer);
}
void AbsoluteValue::analyze(SemanticAnalyzer* analyzer) {
	expression->analyze(analyzer);
	constant = expression->constant;
}

#if COMPILER
Compiler::value AbsoluteValue::compile(Compiler& c) const {
	auto ex = c.insn_to_any(expression->compile(c));
	c.mark_offset(location().start.line);
	auto abso = c.insn_invoke(Type::integer, {ex}, "Value.absolute");
	c.insn_delete_temporary(ex);
	return abso;
}
#endif

std::unique_ptr<Value> AbsoluteValue::clone() const {
	auto abs = std::make_unique<AbsoluteValue>();
	abs->expression = expression->clone();
	abs->open_pipe = open_pipe;
	abs->close_pipe = close_pipe;
	return abs;
}

}
