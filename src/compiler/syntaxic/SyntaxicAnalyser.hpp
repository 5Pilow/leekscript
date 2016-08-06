#ifndef SYNTAXICANALYSER_HPP
#define SYNTAXICANALYSER_HPP

#include <vector>

#include "SyntaxicalError.hpp"
#include "../value/Match.hpp"

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
class Function;

class SyntaxicAnalyser {

	std::vector<Token> tokens;
	Token* t;
	Token* lt;
	Token* nt;
	unsigned i;
	std::vector<SyntaxicalError*> errors;
	long time;

public:

	SyntaxicAnalyser();
	~SyntaxicAnalyser();

	Program* analyse(std::vector<Token>&);

	Token* eat();
	Token* eat(TokenType type);
	Token* nextTokenAt(int pos);

	Block* eatMain();
	Token* eatIdent();
	Value* eatExpression(bool pipe_opened = false);
	Value* eatSimpleExpression(bool pipe_opened = false);
	Value* eatValue();
	bool isObject();
	Value* eatBlockOrObject();
	Block* eatBlock();
	Object* eatObject();
	Value* eatArrayOrMap();
	If* eatIf();
	Match* eatMatch();
	Match::Pattern eatMatchPattern();
	Instruction* eatFor();
	Instruction* eatWhile();
	Foreach* eatForeach();
	ClassDeclaration* eatClassDeclaration();
	VariableDeclaration* eatVariableDeclaration();
	Function* eatFunction();
	VariableDeclaration *eatFunctionDeclaration();
	Instruction* eatInstruction();

	long getTime();
	std::vector<SyntaxicalError*> getErrors();
};

}

#endif
