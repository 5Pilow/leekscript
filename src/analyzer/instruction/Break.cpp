#include "Break.hpp"
#include "../semantic/SemanticAnalyzer.hpp"
#include "../error/Error.hpp"

namespace ls {

Break::Break(Environment& env) : Instruction(env) {
	deepness = 1;
	jumping = true;
	jump_to_existing_section = true;
	breaking = true;
}

void Break::print(std::ostream& os, int, PrintOptions) const {
	os << "break";
	if (deepness > 1) {
		os << " " << deepness;
	}
}

Location Break::location() const {
	return token->location;
}

void Break::analyze(SemanticAnalyzer* analyzer, const Type*) {

	// break must be in a loop
	if (!analyzer->in_loop(deepness)) {
		analyzer->add_error({Error::Type::BREAK_MUST_BE_IN_LOOP, token->location, token->location});
	}
}

#if COMPILER
Compiler::value Break::compile(Compiler& c) const {
	/*	{ for {
	 *		let x = ...
	 *		{
	 *			let y = ...
	 *			if ... break => delete y, delete x, goto end
	 *			let z = ...
	 *		}
	 *		let w = ...
	 *	}
	 *		label end
	 *	}
	 */
	c.delete_variables_block(c.get_current_loop_blocks(deepness));
	return { c.env };
}
#endif

std::unique_ptr<Instruction> Break::clone(Block* parent) const {
	auto b = std::make_unique<Break>(type->env);
	b->token = token;
	b->deepness = deepness;
	return b;
}

}
