#include "Match.hpp"
#include "../../vm/LSValue.hpp"

namespace ls {

void Match::print(std::ostream& os, int indent, bool debug, bool condensed) const {
	os << "match ";
	value->print(os, indent, debug);
	os << " {";
	for (size_t i = 0; i < pattern_list.size(); ++i) {

		os << std::endl << tabs(indent + 1);

		const auto& list = pattern_list[i];
		for (size_t j = 0; j < list.size(); ++j) {
			if (j > 0) {
				os << "|";
			}
			list[j].print(os, indent + 1, debug);
		}
		os << " : ";
		returns[i]->print(os, indent + 1, debug);
	}
	os << std::endl << tabs(indent) << "}";
	if (debug) {
		os << " " << type;
	}
}

Location Match::location() const {
	return {nullptr, {0, 0, 0}, {0, 0, 0}}; // TODO
}

void Match::pre_analyze(ls::SemanticAnalyzer* analyzer) {
	value->pre_analyze(analyzer);
	for (auto& ps : pattern_list) {
		for (Pattern& p : ps) {
			if (p.begin) p.begin->pre_analyze(analyzer);
			if (p.end) p.end->pre_analyze(analyzer);
		}
	}
}

void Match::analyze(ls::SemanticAnalyzer* analyzer) {

	bool has_default = false;

	value->analyze(analyzer);
	throws = value->throws;

	for (auto& ps : pattern_list) {
		for (Pattern& p : ps) {
			if (p.begin) {
				p.begin->analyze(analyzer);
				throws |= p.begin->throws;
			}
			if (p.end) {
				p.end->analyze(analyzer);
				throws |= p.end->throws;
			}
			has_default = has_default || p.is_default();
		}
	}
	if (!has_default) {
		// Return type is always pointer because in the default case, null is return
		type = Type::any;
		for (const auto& r : returns) {
			r->analyze(analyzer);
			throws |= r->throws;
		}
	} else {
		type = Type::void_;
		for (const auto& ret : returns) {
			ret->analyze(analyzer);
			throws |= ret->throws;
			type = type->operator * (ret->type);
		}
	}
}

Compiler::value Match::get_pattern_condition(Compiler& c, Compiler::value v, const std::vector<Pattern>& patterns) const {
	auto cond = patterns[0].match(c, v);
	for (size_t i = 1; i < patterns.size(); ++i) {
		cond = c.insn_or(cond, patterns[i].match(c, v));
	}
	return cond;
}

Compiler::value Match::construct_branch(Compiler& c, Compiler::value v, size_t i) const {
	if (i == pattern_list.size()) {
		return c.insn_convert(c.new_null(), type);
	}
	for (const auto& pattern : pattern_list[i]) {
		if (pattern.is_default()) {
			return c.insn_convert(returns[i]->compile(c), type);
		}
	}
	auto label_then = c.insn_init_label("then");
	auto label_else = c.insn_init_label("else");
	auto label_end = c.insn_init_label("end");

	auto cond = get_pattern_condition(c, v, pattern_list[i]);
	c.insn_if_new(cond, &label_then, &label_else);
	
	c.insn_label(&label_then);
	auto value = c.insn_convert(returns[i]->compile(c), type);
	returns[i]->compile_end(c);
	c.insn_branch(&label_end);
	label_then.block = c.builder.GetInsertBlock();

	c.insn_label(&label_else);
	auto new_branch = construct_branch(c, v, i + 1);
	c.insn_branch(&label_end);
	label_else.block = c.builder.GetInsertBlock();

	c.insn_label(&label_end);
	return c.insn_phi(type, value, label_then, new_branch, label_else);
}

Compiler::value Match::compile(Compiler& c) const {
	auto v = value->compile(c);
	v.t = v.t->not_temporary();
	c.insn_inc_refs(v);
	auto res = construct_branch(c, v, 0);
	c.insn_delete(v);
	return res;
}

Match::Pattern::Pattern(std::unique_ptr<Value> value) : interval(false), begin(std::move(value)), end(nullptr) {}

Match::Pattern::Pattern(std::unique_ptr<Value> begin, std::unique_ptr<Value> end) : interval(true), begin(std::move(begin)), end(std::move(end)) {}

Match::Pattern::~Pattern() {}

void Match::Pattern::print(std::ostream &os, int indent, bool debug) const {
	if (interval) {
		if (begin) begin->print(os, indent, debug);
		os << "..";
		if (end) end->print(os, indent, debug);
	} else {
		begin->print(os, indent, debug);
	}
}

Compiler::value Match::Pattern::match(Compiler &c, Compiler::value v) const {
	if (interval) {
		Compiler::value ge;
		if (begin) {
			auto b = begin->compile(c);
			ge = c.insn_ge(v, b);
		}
		Compiler::value lt;
		if (end) {
			auto e = end->compile(c);
			lt = c.insn_lt(v, e);
		}
		if (ge.v) {
			if (lt.v) {
				return c.insn_and(ge, lt);
			} else {
				return ge;
			}
		} else {
			return lt;
		}
	} else {
		auto p = begin->compile(c);
		return c.insn_eq(p, v);
	}
}

std::unique_ptr<Value> Match::clone() const {
	auto match = std::make_unique<Match>();
	match->value = value->clone();
	for (const auto& pl : pattern_list) {
		match->pattern_list.push_back({});
		for (const auto& p : pl) {
			match->pattern_list.back().emplace_back(p.clone());
		}
	}
	return match;
}

}
