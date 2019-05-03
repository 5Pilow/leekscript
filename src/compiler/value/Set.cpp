#include "Set.hpp"
#include "../../vm/value/LSSet.hpp"

namespace ls {

Set::Set() {}

Set::~Set() {
	for (auto ex : expressions) delete ex;
}

void Set::print(std::ostream& os, int indent, bool debug, bool condensed) const {
	os << "<";
	for (size_t i = 0; i < expressions.size(); ++i) {
		if (i > 0) os << ", ";
		expressions[i]->print(os, indent + 1, debug);
	}
	os << ">";
	if (debug) os << " " << type;
}

Location Set::location() const {
	return {{0, 0, 0}, {0, 0, 0}}; // TODO
}

void Set::analyse(SemanticAnalyser* analyser) {

	Type element_type = {};

	constant = true;
	for (auto& ex : expressions) {
		ex->analyse(analyser);
		element_type = element_type * ex->type;
		constant = constant && ex->constant;
	}
	if (element_type.is_primitive()) {
		if (element_type != Type::integer() && element_type != Type::real()) {
			element_type = Type::any();
		}
	} else if (!element_type.is_primitive()) {
		element_type = Type::any();
	}
	if (element_type._types.size() == 0) {
		element_type = Type::any();
	}
	type = Type::tmp_set(element_type);
}

bool Set::will_store(SemanticAnalyser* analyser, const Type& type) {

	Type added_type = type;
	if (added_type.is_array() or added_type.is_set()) {
		added_type = added_type.element();
	}
	Type current_type = this->type.element();
	if (expressions.size() == 0) {
		this->type = Type::tmp_set(added_type);
	} else {
		this->type = Type::tmp_set(current_type * added_type);
	}
	return false;
}

Compiler::value Set::compile(Compiler& c) const {
	auto create = type.element().is_integer() ? "Set.new.2" : type.element().is_real() ? "Set.new.1" : "Set.new";
	auto insert = type.element().is_integer() ? "Set.vinsert.2" : type.element().is_real() ? "Set.vinsert.1" : "Set.vinsert";

	unsigned ops = 1;
	auto s = c.insn_call(type, {}, create);

	double i = 0;
	for (Value* ex : expressions) {
		auto v = c.insn_convert(ex->compile(c), type.element());
		ex->compile_end(c);
		c.insn_call({}, {s, v}, insert);
		ops += std::log2(++i);
	}
	c.inc_ops(ops);
	return s;
}

Value* Set::clone() const {
	auto s = new Set();
	for (const auto& v : expressions) {
		s->expressions.push_back(v->clone());
	}
	return s;
}

}
