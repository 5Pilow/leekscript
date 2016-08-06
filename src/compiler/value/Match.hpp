#ifndef MATCH_HPP_
#define MATCH_HPP_

#include "../../compiler/value/Value.hpp"
#include "../../compiler/value/Block.hpp"
#include <vector>

namespace ls {

class Match : public Value {
public:
	class Pattern {
	public:
		bool interval;
		Value* begin;
		Value* end;

		Pattern(Value* value);
		Pattern(Value* begin, Value* end);
		~Pattern();

		inline bool is_default() const {
			return !begin && !end;
		}

		void print(std::ostream&, int indent, bool debug) const;
		jit_value_t match(Compiler &c, jit_value_t v) const;
	};

	Value* value;
	std::vector<std::vector<Pattern>> pattern_list;
	std::vector<Value*> returns;

	Match();
	virtual ~Match();

	virtual void print(std::ostream&, int indent, bool debug = false) const override;
	virtual unsigned line() const override;

	virtual void analyse(SemanticAnalyser*, const Type&) override;
	virtual jit_value_t compile(Compiler&) const override;
};

}

#endif
