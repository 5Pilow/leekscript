#ifndef FOREACH_HPP
#define FOREACH_HPP

#include <vector>
#include "../value/Value.hpp"
#include "../value/Block.hpp"
#include "../semantic/SemanticAnalyzer.hpp"
#include "../semantic/Mutation.hpp"

namespace ls {

enum class ForeachMode {
	/**
	  * Default mode for containers with real elements : arrays, sets and maps
	  * It will get the address of each element, so modification is possible even for primitive elements, like array<int>.
	  * Fast: the address is computed and directly used
	  */
	ADDRESS,
	/**
	  * It will provide a constant value for each element
	  * Default mode for containers with computed elements : numbers, strings, intervals
	  * Fast: the value is computed and directly used
	  */
	VALUE,
	/**
	  * Mode activated with `for (var @x in container)`
	  * It will create a reference to each element
	  * Compatible with containers with real elements : arrays, sets, maps
	  * Not compatible with containers with computed elements : numbers, strings, intervals
	  * Fast for wrapped elements (array<string>, set<object>) but slow for primitive types because the container will be converted to an container of wrapped values before the foreach.
	  */
	REFERENCE,
	/**
	  * Mode activated with `for (var +x in container)`, never activated by default
	  * It will create a separated variable to store each value
	  * Compatible with every container
	  * Slow: each element is copied, but can be useful
	  */
	COPY,
};

class Foreach : public Instruction {
public:
	std::unique_ptr<Block> wrapper_block;
	Token* key = nullptr;
	Token* value = nullptr;
	std::unique_ptr<Value> container;
	std::unique_ptr<Block> body;
	std::vector<Mutation> mutations;
	std::vector<std::tuple<Variable*, Variable*, const Section*>> conversions;
	Section* condition_section = nullptr;
	Section* increment_section = nullptr;

	const Type* key_type;
	const Type* value_type;
	Variable* value_var;
	Variable* key_var;

	ForeachMode mode;

	Foreach(Environment& env);

	virtual void print(std::ostream&, int indent, PrintOptions options) const override;
	virtual Location location() const override;

	virtual void pre_analyze(SemanticAnalyzer* analyzer) override;
	virtual void analyze(SemanticAnalyzer*, const Type* req_type) override;

	#if COMPILER
	virtual Compiler::value compile(Compiler&) const override;
	#endif

	virtual std::unique_ptr<Instruction> clone() const override;
};

}

#endif
