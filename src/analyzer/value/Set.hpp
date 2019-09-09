#ifndef SET_HPP
#define SET_HPP

#include <vector>
#include "Value.hpp"

namespace ls {

class Set : public Value {
public:
	std::vector<std::unique_ptr<Value>> expressions;

	virtual void print(std::ostream&, int indent, bool debug, bool condensed) const override;
	virtual Location location() const override;
	virtual void pre_analyze(SemanticAnalyzer*) override;
	virtual void analyze(SemanticAnalyzer*) override;
	virtual bool will_store(SemanticAnalyzer* analyzer, const Type* type) override;
	
	virtual Compiler::value compile(Compiler&) const override;

	virtual std::unique_ptr<Value> clone() const override;
};

}

#endif
