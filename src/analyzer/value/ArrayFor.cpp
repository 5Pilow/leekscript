#include "ArrayFor.hpp"

namespace ls {

void ArrayFor::print(std::ostream& os, int indent, bool debug, bool condensed) const {
	os << "[";
	forr->print(os, indent, debug, condensed);
	os << "]";

	if (debug) {
		os << " " << type;
	}
}

Location ArrayFor::location() const {
	return {nullptr, {0, 0, 0}, {0, 0, 0}}; // TODO
}

void ArrayFor::pre_analyze(SemanticAnalyzer* analyzer) {
	forr->pre_analyze(analyzer);
}

void ArrayFor::analyze(SemanticAnalyzer* analyzer) {
	forr->analyze(analyzer, Type::array());
	type = forr->type;
	throws = forr->throws;
}

Compiler::value ArrayFor::compile(Compiler& c) const {
	return forr->compile(c);
}

std::unique_ptr<Value> ArrayFor::clone() const {
	auto af = std::make_unique<ArrayFor>();
	af->forr = forr->clone();
	return af;
}

}
