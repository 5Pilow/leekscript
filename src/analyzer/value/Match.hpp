#ifndef MATCH_HPP_
#define MATCH_HPP_

#include "Value.hpp"
#include "Block.hpp"
#include <vector>

namespace ls {

class Match : public Value {
public:
	class Pattern {
	public:
		bool interval;
		std::unique_ptr<Value> begin;
		std::unique_ptr<Value> end;

		Pattern(Pattern&& p) : interval(p.interval), begin(std::move(p.begin)), end(std::move(p.end)) {}
		Pattern(std::unique_ptr<Value> value);
		Pattern(std::unique_ptr<Value> begin, std::unique_ptr<Value> end);
		~Pattern();

		inline bool is_default() const { return !begin && !end; }

		void print(std::ostream&, int indent, bool debug) const;
		Compiler::value match(Compiler &c, Compiler::value v) const;

		Pattern&& clone() const {
			Pattern p { begin->clone(), end->clone() };
			p.interval = interval;
			return std::move(p);
		}
	};

	std::unique_ptr<Value> value;
	std::vector<std::vector<Pattern>> pattern_list;
	std::vector<std::unique_ptr<Value>> returns;

	virtual void print(std::ostream&, int indent, bool debug, bool condensed) const override;
	virtual Location location() const override;

	virtual void pre_analyze(SemanticAnalyzer*) override;
	virtual void analyze(SemanticAnalyzer*) override;
	Compiler::value construct_branch(Compiler& c, Compiler::value v, size_t i) const;
	Compiler::value get_pattern_condition(Compiler& c, Compiler::value v, const std::vector<Pattern>&) const;
	virtual Compiler::value compile(Compiler&) const override;

	virtual std::unique_ptr<Value> clone() const override;
};

}

#endif
