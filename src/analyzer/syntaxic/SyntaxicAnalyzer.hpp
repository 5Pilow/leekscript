#ifndef SYNTAXIC_ANALYZER_HPP
#define SYNTAXIC_ANALYZER_HPP

#include <vector>

#include "../value/Match.hpp"
#include "../resolver/Resolver.hpp"

namespace ls {

class Token;
enum class TokenType;
class Ident;
class Instruction;
class Value;
class ClassDeclaration;
class VariableDeclaration;
class If;
class For;
class Foreach;
class Program;
class Block;
class Object;
class Array;
class Set;
class Function;
class Break;
class Continue;

class SyntaxicAnalyzer {

	Resolver* resolver;
	Token* t;
	int last_character = 0;
	size_t last_line = 0;
	size_t last_size = 0;
	Token* nt;
	unsigned i;
	long time;
	std::vector<std::pair<unsigned,size_t>> stack;
	Token* finished_token;
	File* file;
	std::vector<Instruction*> loops;

public:

	Environment& env;

	SyntaxicAnalyzer(Environment& env, Resolver* resolver);

	Block* analyze(File* file);

	Block* eatMain(File* file);
	Token* eatIdent();
	Value* eatExpression(Block* block, bool pipe_opened = false, bool set_opened = false, Value* initial = nullptr, bool comma_list = false);
	Value* eatSimpleExpression(Block* block, bool pipe_opened = false, bool set_opened = false, bool comma_list = false, Value* initial = nullptr);
	Value* eatValue(Block* block, bool comma_list = false);
	bool isObject();
	Value* eatBlockOrObject(Block* block, Section* before = nullptr, Section* after = nullptr);
	Block* eatBlock(Block* block, bool is_function_block = false, bool single_instruction = false, Section* before = nullptr, Section* after = nullptr);
	Block* newBlock(Block* block, Value* value, Section* after = nullptr);
	Block* newEmptyBlock(Block* block, Section* after = nullptr);
	Block* blockInit(Block* parent, Section* before, bool is_function_block);
	void blockEnd(Block* block, Section* after);
	Object* eatObject(Block* block);
	Value* eatArrayOrMap(Block* block);
	Value* eatSetOrLowerOperator(Block* block);
	Value* eatIf(Block* block);
	Match* eatMatch(Block* block, bool force_value);
	Match::Pattern eatMatchPattern(Block* block);
	Instruction* eatFor(Block* block);
	Instruction* eatWhile(Block* block);
	Break* eatBreak(Block* block);
	Continue* eatContinue(Block* block);
	ClassDeclaration* eatClassDeclaration(Block* block);
	VariableDeclaration* eatVariableDeclaration(Block* block);
	Function* eatFunction(Block* block, Token* token);
	VariableDeclaration* eatFunctionDeclaration(Block* block);
	Instruction* eatInstruction(Block* block);
	Value* eatLambdaContinue(Block* block, bool parenthesis, Ident ident, Value* expression, bool comma_list = false);
	Value* eatLambdaOrParenthesisExpression(Block* block, bool pipe_opened = false, bool set_opened = false, bool comma_list = false);
	void splitCurrentOrInTwoPipes();

	bool beginingOfExpression(TokenType type);
	int findNextClosingParenthesis();
	int findNextArrow();
	int findNextColon();
	bool isLambda();

	Token* eat_get();
	void eat();
	Token* eat_get(TokenType type);
	void eat(TokenType type);
	Token* nextTokenAt(int pos);
};

}

#endif
