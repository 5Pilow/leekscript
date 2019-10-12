#include "Interval.hpp"
#include "../../type/Type.hpp"
#include <math.h>
#include "../semantic/SemanticAnalyzer.hpp"

namespace ls {

Interval::Interval(Environment& env) : Value(env) {
	type = env.interval;
}

void Interval::print(std::ostream& os, int indent, PrintOptions options) const {
	os << "[";
	start->print(os, indent, options);
	os << "..";
	end->print(os, indent, options);
	os << "]";
	if (options.debug) {
		os << " " << type;
	}
}

Location Interval::location() const {
	return {opening_bracket->location.file, opening_bracket->location.start, closing_bracket->location.end};
}

void Interval::pre_analyze(SemanticAnalyzer* analyzer) {
	start->pre_analyze(analyzer);
	end->pre_analyze(analyzer);
}

void Interval::analyze(SemanticAnalyzer* analyzer) {
	constant = true;
	type = analyzer->env.tmp_interval;
	start->analyze(analyzer);
	end->analyze(analyzer);
}

#if COMPILER
Compiler::value Interval::compile(Compiler& c) const {
	auto a = start->compile(c);
	auto b = end->compile(c);
	auto int_a = c.to_int(a);
	auto int_b = c.to_int(b);
	auto interval = c.insn_call(c.env.tmp_interval, {int_a, int_b}, "Interval.new");
	c.insn_delete_temporary(a);
	c.insn_delete_temporary(b);
	return interval;
}
#endif

std::unique_ptr<Value> Interval::clone(Block* parent) const {
	auto interval = std::make_unique<Interval>(type->env);
	interval->opening_bracket = opening_bracket;
	interval->closing_bracket = closing_bracket;
	interval->start = start->clone(parent);
	interval->end = end->clone(parent);
	return interval;
}

}
