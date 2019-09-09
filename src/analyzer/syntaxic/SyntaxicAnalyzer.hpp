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

public:

	SyntaxicAnalyzer(Resolver* resolver);

	Block* analyze(File* file);

	Block* eatMain(File* file);
	Token* eatIdent();
	Value* eatExpression(bool pipe_opened = false, bool set_opened = false, Value* initial = nullptr, bool comma_list = false);
	Value* eatSimpleExpression(bool pipe_opened = false, bool set_opened = false, bool comma_list = false, Value* initial = nullptr);
	Value* eatValue(bool comma_list = false);
	bool isObject();
	Value* eatBlockOrObject();
	Block* eatBlock(bool is_function_block = false);
	Object* eatObject();
	Value* eatArrayOrMap();
	Value* eatSetOrLowerOperator();
	If* eatIf();
	Match* eatMatch(bool force_value);
	Match::Pattern eatMatchPattern();
	Instruction* eatFor();
	Instruction* eatWhile();
	Break* eatBreak();
	Continue* eatContinue();
	ClassDeclaration* eatClassDeclaration();
	VariableDeclaration* eatVariableDeclaration();
	Function* eatFunction(Token* token);
	VariableDeclaration* eatFunctionDeclaration();
	Instruction* eatInstruction();
	Value* eatLambdaContinue(bool parenthesis, Ident ident, Value* expression, bool comma_list = false);
	Value* eatLambdaOrParenthesisExpression(bool pipe_opened = false, bool set_opened = false, bool comma_list = false);
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
