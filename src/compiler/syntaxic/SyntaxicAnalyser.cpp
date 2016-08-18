#include "SyntaxicAnalyser.hpp"

#include <string>
#include <math.h>

#include "../value/Function.hpp"
#include "../instruction/Instruction.hpp"
#include "../value/Block.hpp"
#include "../instruction/Break.hpp"
#include "../instruction/Foreach.hpp"
#include "../instruction/For.hpp"
#include "../value/If.hpp"
#include "../value/Match.hpp"
#include "../instruction/Continue.hpp"
#include "../instruction/ExpressionInstruction.hpp"
#include "../instruction/ClassDeclaration.hpp"
#include "../instruction/VariableDeclaration.hpp"
#include "../instruction/Return.hpp"
#include "../instruction/While.hpp"
#include "../value/AbsoluteValue.hpp"
#include "../value/Array.hpp"
#include "../value/Map.hpp"
#include "../value/ArrayAccess.hpp"
#include "../value/Boolean.hpp"
#include "../value/FunctionCall.hpp"
#include "../value/Nulll.hpp"
#include "../value/Number.hpp"
#include "../value/Object.hpp"
#include "../value/ObjectAccess.hpp"
#include "../value/PostfixExpression.hpp"
#include "../value/PrefixExpression.hpp"
#include "../value/Reference.hpp"
#include "../value/String.hpp"
#include "../value/VariableValue.hpp"
#include "../value/ArrayFor.hpp"
#include "../../vm/Program.hpp"
#include "SyntaxicalError.hpp"
#include "../lexical/Token.hpp"

using namespace std;

namespace ls {

SyntaxicAnalyser::SyntaxicAnalyser() {
	time = 0;
	lt = nullptr;
	nt = nullptr;
	t = nullptr;
	i = 0;
}

SyntaxicAnalyser::~SyntaxicAnalyser() {
	for (SyntaxicalError* error : errors) {
		delete error;
	}
}

Program* SyntaxicAnalyser::analyse(vector<Token>& tokens) {

	this->tokens = tokens;
	this->lt = nullptr;
	this->t = &tokens.at(0);
	this->nt = tokens.size() > 1 ? &tokens.at(1) : nullptr;
	this->i = 0;

	Program* program = new Program();
	program->main = new Function();
	program->main->body = eatMain();

	return program;
}

Block* SyntaxicAnalyser::eatMain() {

	Block* block = new Block();

	while (true) {
		if (t->type == TokenType::FINISHED) {
			eat();
			break;
		} else if (t->type == TokenType::SEMICOLON) {
			eat();
		} else {
			Instruction* ins = eatInstruction();
			if (ins) {
				block->instructions.push_back(ins);
			}
		}
	}

	return block;
}

/*
 * Detects whether a opening brace is an object or a block
 *
 * {} => object
 * {a: 12} => object
 * { 12 } => block
 */
bool SyntaxicAnalyser::isObject() {

	if (nt != nullptr and nt->type == TokenType::CLOSING_BRACE) {
		return true;
	}

	auto nnt = nextTokenAt(2);
	if (nt != nullptr and nt->type == TokenType::IDENT
			and nnt != nullptr and nnt->type == TokenType::COLON) {
		return true;
	}
	return false;
}

Value* SyntaxicAnalyser::eatBlockOrObject() {
	if (isObject()) {
		return eatObject();
	} else {
		return eatBlock();
	}
}

Block* SyntaxicAnalyser::eatBlock() {

	Block* block = new Block();

	bool brace = false;
	if (t->type == TokenType::OPEN_BRACE) {
		brace = true;
		eat();
	}

	while (true) {
		if (t->type == TokenType::CLOSING_BRACE) {
			eat();
			if (not brace) {
				errors.push_back(new SyntaxicalError(t, "Unexpected closing brace, forgot to open it ?"));
			}
			break;
		} else if (t->type == TokenType::FINISHED || t->type == TokenType::ELSE || t->type == TokenType::END || t->type == TokenType::IN) {
			if (brace) {
				errors.push_back(new SyntaxicalError(t, "Expecting closing brace at end of the block"));
			}
			break;
		} else if (t->type == TokenType::SEMICOLON) {
			eat();
		} else {
			Instruction* ins = eatInstruction();
			if (ins) block->instructions.push_back(ins);
		}
	}

	return block;
}

Object* SyntaxicAnalyser::eatObject() {

	eat(TokenType::OPEN_BRACE);

	Object* o = new Object();

	while (t->type == TokenType::IDENT) {

		o->keys.push_back(new Ident(eatIdent()));
		eat(TokenType::COLON);
		o->values.push_back(eatExpression());

		if (t->type == TokenType::COMMA) {
			eat();
		}
	}
	eat(TokenType::CLOSING_BRACE);

	return o;
}

Instruction* SyntaxicAnalyser::eatInstruction() {

	switch (t->type) {

		case TokenType::LET:
		case TokenType::GLOBAL:
			return eatVariableDeclaration();

		case TokenType::NUMBER:
		case TokenType::PI:
		case TokenType::TRUE:
		case TokenType::FALSE:
		case TokenType::STRING:
		case TokenType::IDENT:
		case TokenType::OPEN_BRACKET:
		case TokenType::OPEN_PARENTHESIS:
		case TokenType::OPEN_BRACE:
		case TokenType::NULLL:
		case TokenType::ARROW:
		case TokenType::IF:
		case TokenType::MINUS:
		case TokenType::NOT:
		case TokenType::MINUS_MINUS:
		case TokenType::PLUS_PLUS:
		case TokenType::NEW:
//		case TokenType::AROBASE:
		case TokenType::PLUS:
		case TokenType::TIMES:
		case TokenType::DIVIDE:
		case TokenType::POWER:
		case TokenType::MODULO:
		case TokenType::PIPE:
		case TokenType::TILDE:
			return new ExpressionInstruction(eatExpression());

		case TokenType::MATCH:
			return new ExpressionInstruction(eatMatch(false));

		case TokenType::FUNCTION:
			return eatFunctionDeclaration();

		case TokenType::RETURN: {
			eat();
			if (t->type == TokenType::FINISHED or t->type == TokenType::CLOSING_BRACE
				or t->type == TokenType::ELSE or t->type == TokenType::END) {
				return new Return(new Nulll());
			} else {
				return new Return(eatExpression());
			}
		}
		case TokenType::BREAK:
			return eatBreak();

		case TokenType::CONTINUE:
			return eatContinue();

		case TokenType::CLASS:
			return eatClassDeclaration();

		case TokenType::FOR:
			return eatFor();

		case TokenType::WHILE:
			return eatWhile();

		default:
			errors.push_back(new SyntaxicalError(t, "Unexpected token <" + to_string((int)t->type) + "> (" + t->content + ")"));
			eat();
			return nullptr;
	}
}

VariableDeclaration* SyntaxicAnalyser::eatVariableDeclaration() {

	VariableDeclaration* vd = new VariableDeclaration();

	if (t->type == TokenType::GLOBAL) {
		vd->global = true;
		eat(TokenType::GLOBAL);
	} else {
		eat(TokenType::LET);
	}

	vd->variables.push_back(eatIdent());

	while (t->type == TokenType::COMMA) {
		eat(TokenType::COMMA);
		vd->variables.push_back(eatIdent());
	}

	if (t->type == TokenType::EQUAL) {

		eat(TokenType::EQUAL);

		if (t->type == TokenType::AROBASE) {
			eat();
			vd->references.push_back(true);
		} else {
			vd->references.push_back(false);
		}
		vd->expressions.push_back(eatExpression());


		while (t->type == TokenType::COMMA && vd->expressions.size() < vd->variables.size()) {
			eat();
			if (t->type == TokenType::AROBASE) {
				eat();
				vd->references.push_back(true);
			} else {
				vd->references.push_back(false);
			}
			vd->expressions.push_back(eatExpression());
		}
	}

	return vd;
}

Function* SyntaxicAnalyser::eatFunction() {

	if (t->type == TokenType::FUNCTION) {
		eat();
	}

	Function* f = new Function();

	eat(TokenType::OPEN_PARENTHESIS);

	while (t->type != TokenType::FINISHED && t->type != TokenType::CLOSING_PARENTHESIS) {

		bool reference = false;
		if (t->type == TokenType::AROBASE) {
			eat();
			reference = true;
		}

		Token* ident = eatIdent();

		Value* defaultValue = nullptr;
		if (t->type == TokenType::EQUAL) {
			eat();
			defaultValue = eatExpression();
		}

		f->addArgument(ident, reference, defaultValue);

		if (t->type == TokenType::COMMA) {
			eat();
		}
	}
	eat(TokenType::CLOSING_PARENTHESIS);

	bool braces = false;
	if (t->type == TokenType::OPEN_BRACE) {
		braces = true;
	}

	f->body = eatBlock();

	if (!braces) {
		eat(TokenType::END);
	}

	return f;
}

VariableDeclaration* SyntaxicAnalyser::eatFunctionDeclaration() {

	eat(TokenType::FUNCTION);

	VariableDeclaration* vd = new VariableDeclaration();
	vd->global = true;

	vd->variables.push_back(eatIdent());
	vd->expressions.push_back(eatFunction());

	return vd;
}

bool SyntaxicAnalyser::beginingOfExpression(TokenType type) {

	return type == TokenType::NUMBER or type == TokenType::IDENT
		/*or type == TokenType::AROBASE*/ or type == TokenType::OPEN_BRACKET
		or type == TokenType::OPEN_BRACE or type == TokenType::OPEN_PARENTHESIS
		or type == TokenType::STRING or type == TokenType::PI or type == TokenType::TRUE
		or type == TokenType::FALSE or type == TokenType::NULLL;
}

Value* SyntaxicAnalyser::eatSimpleExpression(bool pipe_opened) {

	Value* e = nullptr;

	if (t->type == TokenType::OPEN_PARENTHESIS) {

		eat();

		if (t->type == TokenType::CLOSING_PARENTHESIS) {
			eat();
			e = new Nulll();
		} else {
			e = eatExpression();
			e->parenthesis = true;
			eat(TokenType::CLOSING_PARENTHESIS);
		}

	} else if (t->type == TokenType::PIPE) {

		eat();
		AbsoluteValue* av = new AbsoluteValue();
		av->expression = eatExpression(true);
		eat(TokenType::PIPE);
		e = new Expression(av);

	} else {

		// Opérateurs unaires préfixe
		if (t->type == TokenType::NEW || t->type == TokenType::MINUS ||
			t->type == TokenType::PLUS || t->type == TokenType::NOT ||
			t->type == TokenType::MINUS_MINUS || t->type == TokenType::PLUS_PLUS
			|| t->type == TokenType::TILDE) {

			if (t->type == TokenType::MINUS && nt != nullptr && t->line == nt->line) {

				Token* minus = eat();
				Operator* op = new Operator(minus);

				if (beginingOfExpression(t->type)) {

					Value* ex = eatExpression(pipe_opened);
					Expression* expr = dynamic_cast<Expression*>(ex);

					if (expr and expr->op->priority >= op->priority) {
						PrefixExpression* pexp = new PrefixExpression();
						pexp->operatorr = op;
						pexp->expression = expr->v1;
						expr->v1 = pexp;
						e = expr;
					} else {
						PrefixExpression* pe = new PrefixExpression();
						pe->operatorr = op;
						pe->expression = ex;
						e = pe;
					}
				} else {
					// No expression after the -, so it's the variable '-'
					e = new VariableValue(minus);
				}

			} else if (t->type == TokenType::PLUS) {

				Token* plus = eat(); // eat the +

				if (beginingOfExpression(t->type)) {
					e = eatExpression(pipe_opened);
				} else {
					// No expression after the +, so it's the variable '+'
					e = new VariableValue(plus);
				}

			} else {
				PrefixExpression* ex = new PrefixExpression();
				ex->operatorr = new Operator(eat());
				ex->expression = eatSimpleExpression();
				e = new Expression(ex);
			}

		} else {
			e = eatValue();
		}
	}

	while (t->type == TokenType::OPEN_BRACKET || t->type == TokenType::OPEN_PARENTHESIS
		   || t->type == TokenType::DOT) {

		if (t->character != lt->character + lt->size)
			break;

		switch (t->type) {

			case TokenType::OPEN_BRACKET: {

				ArrayAccess* aa = new ArrayAccess();
				eat(TokenType::OPEN_BRACKET);

				aa->array = e;
				aa->key = eatExpression();

				if (t->type == TokenType::COLON) {
					eat();
					aa->key2 = eatExpression();
				}

				eat(TokenType::CLOSING_BRACKET);
				e = aa;
				break;
			}

			case TokenType::OPEN_PARENTHESIS: {

				FunctionCall* fc = new FunctionCall();
				fc->function = e;

				eat(TokenType::OPEN_PARENTHESIS);
				if (t->type != TokenType::CLOSING_PARENTHESIS) {
					fc->arguments.push_back(eatExpression());
					while (t->type == TokenType::COMMA) {
						eat();
						fc->arguments.push_back(eatExpression());
					}
				}
				eat(TokenType::CLOSING_PARENTHESIS);

				e = fc;
				break;
			}

			case TokenType::DOT: {

				ObjectAccess* oa = new ObjectAccess();
				eat(TokenType::DOT);

				oa->object = e;

				if (t->type == TokenType::NEW || t->type == TokenType::CLASS) {
					oa->field = t;
					eat();
				} else if (t->type == TokenType::NUMBER) {
					oa->field = t;
					//					oa->isNumber = true;
					eat();
				} else {
					oa->field = eatIdent();
				}
				e = oa;
				break;
			}

			default:
				break;
		}
	}

	// Opérateurs unaires postfixes
	if (t->type == TokenType::MINUS_MINUS || t->type == TokenType::PLUS_PLUS) {

		if (lt->line == t->line) {

			Token* op = eat();
			PostfixExpression* ex = new PostfixExpression();

			ex->operatorr = new Operator(op);
			ex->expression = (LeftValue*) e;

			e = ex;
		}
	}

	return e;
}

Value* SyntaxicAnalyser::eatExpression(bool pipe_opened) {

	Expression* ex = nullptr;
	Value* e = eatSimpleExpression(pipe_opened);

	// Opérateurs binaires
	while (t->type == TokenType::PLUS || t->type == TokenType::MINUS ||
		   t->type == TokenType::TIMES || t->type == TokenType::DIVIDE ||
		   t->type == TokenType::MODULO || t->type == TokenType::AND ||
		   t->type == TokenType::OR || t->type == TokenType::XOR ||
		   t->type == TokenType::EQUAL || t->type == TokenType::POWER ||
		   t->type == TokenType::DOUBLE_EQUAL || t->type == TokenType::DIFFERENT ||
		   t->type == TokenType::TRIPLE_EQUAL || t->type == TokenType::TRIPLE_DIFFERENT ||
		   t->type == TokenType::GREATER || t->type == TokenType::LOWER ||
		   t->type == TokenType::GREATER_EQUALS || t->type == TokenType::LOWER_EQUALS ||
		   t->type == TokenType::TIMES_EQUAL || t->type == TokenType::PLUS_EQUAL ||
		   t->type == TokenType::MINUS_EQUAL || t->type == TokenType::DIVIDE_EQUAL ||
		   t->type == TokenType::MODULO_EQUAL || t->type == TokenType::POWER_EQUAL ||
		   t->type == TokenType::SWAP || t->type == TokenType::TILDE ||
		   t->type == TokenType::TILDE_TILDE || t->type == TokenType::TILDE_EQUAL ||
		   t->type == TokenType::TILDE_TILDE_EQUAL || t->type == TokenType::IN ||
		   t->type == TokenType::INSTANCEOF ||
		   t->type == TokenType::BIT_AND || t->type == TokenType::BIT_AND_EQUALS ||
		   (!pipe_opened and t->type == TokenType::PIPE) || t->type == TokenType::BIT_OR_EQUALS ||
		   t->type == TokenType::BIT_XOR || t->type == TokenType::BIT_XOR_EQUALS ||
		   t->type == TokenType::BIT_SHIFT_LEFT ||	t->type == TokenType::BIT_SHIFT_LEFT_EQUALS ||
		   t->type == TokenType::BIT_SHIFT_RIGHT || t->type == TokenType::BIT_SHIFT_RIGHT_EQUALS ||
		   t->type == TokenType::BIT_SHIFT_RIGHT_UNSIGNED || t->type == TokenType::BIT_SHIFT_RIGHT_UNSIGNED_EQUALS ||
		   t->type == TokenType::BIT_ROTATE_LEFT || t->type == TokenType::BIT_ROTATE_LEFT_EQUALS ||
		   t->type == TokenType::BIT_ROTATE_RIGHT || t->type == TokenType::BIT_ROTATE_RIGHT_EQUALS ||
		   t->type == TokenType::DOUBLE_QUESTION_MARK
		   ) {

		if (t->type == TokenType::MINUS && t->line != lt->line && nt != nullptr && t->line == nt->line)
			break;

		Operator* op = new Operator(t);
		eat();

		bool reference_v2 = false;
		if (op->type == TokenType::EQUAL && t->type == TokenType::AROBASE) {
			eat();
			reference_v2 = true;
		}

		if (ex == nullptr) {
			if (Expression* exx = dynamic_cast<Expression*>(e)) {
				ex = exx;
			} else {
				ex = new Expression();
				ex->v1 = e;
			}
		}
		ex->append(op, eatSimpleExpression(), reference_v2);
	}

	if (ex != nullptr) {
		return ex;
	}
	return e;
}

double stod_(string str) {

	if (str.size() > 2 && str[0] == '0' && str[1] == 'b') {
		double x = 0.0;
		for (size_t i = 2; i < str.size(); ++i) {
			x *= 2.0;
			if (str[i] == '1') x += 1.0;
		}
		return x;
	} else {
		return stod(str);
	}
}

Value* SyntaxicAnalyser::eatValue() {

	switch (t->type) {

		case TokenType::PLUS:
		case TokenType::MINUS:
		case TokenType::MODULO:
		case TokenType::TIMES:
		case TokenType::DIVIDE:
		case TokenType::POWER:
		case TokenType::TERNARY:
		{
			VariableValue* v = new VariableValue(t);
			eat();
			return v;
		}

		case TokenType::NUMBER:
		{
			Number* n = new Number(stod_(t->content), t);
			eat();
			return n;
		}

		case TokenType::PI:
			eat();
			return new Number(M_PI, t);

		case TokenType::STRING:
		{
			String* v = new String(t->content, t);
			eat();
			return v;
		}

		case TokenType::TRUE:
		case TokenType::FALSE:
		{
			Boolean* bv = new Boolean(t->type == TokenType::TRUE);
			eat();
			return bv;
		}

		case TokenType::NULLL:
			eat();
			return new Nulll();

		case TokenType::IDENT:
		{
			Token* ident = eatIdent();

			switch (t->type) {

				case TokenType::ARROW: {

					Function* l = new Function();
					l->lambda = true;
					l->addArgument(ident, false, nullptr);
					eat(TokenType::ARROW);
					l->body = new Block();
					l->body->instructions.push_back(new ExpressionInstruction(eatExpression()));

					return l;
				}

				case TokenType::COMMA: {

					// Là y'a une virgule, il faut voir si y'a une flèche
					// plus loin
					bool canBeLamda = true;
					int pos = i;
					while (true) {
						if (tokens.at(pos).type == TokenType::ARROW) {
							break;
						}
						if (tokens.at(pos).type != TokenType::COMMA || tokens.at(pos + 1).type != TokenType::IDENT) {
							canBeLamda = false;
							break;
						}
						pos += 2;
					}

					if (canBeLamda) {

						Function* l = new Function();
						l->lambda = true;
						l->addArgument(ident, false, nullptr);
						while (t->type == TokenType::COMMA) {
							eat();
							l->addArgument(eatIdent(), false, nullptr);
						}

						eat(TokenType::ARROW);
						l->body = new Block();
						l->body->instructions.push_back(new ExpressionInstruction(eatExpression()));

						return l;

					} else {
						return new VariableValue(ident);
					}
				}
				default: {
					return new VariableValue(ident);
				}
			}
			break;
		}

//		case TokenType::AROBASE:
//			eat();
//			return new Reference(eatIdent());

		case TokenType::OPEN_BRACKET:
			return eatArrayOrMap();

		case TokenType::OPEN_BRACE:
			return eatBlockOrObject();

		case TokenType::IF:
			return eatIf();

		case TokenType::MATCH:
			return eatMatch(true);

		case TokenType::FUNCTION:
			return eatFunction();


		case TokenType::ARROW:
		{
			Function* l = new Function();
			l->lambda = true;
			eat(TokenType::ARROW);
			l->body = new Block();
			l->body->instructions.push_back(new ExpressionInstruction(eatExpression()));
			return l;
		}

		default:
			break;
	}

	errors.push_back(new SyntaxicalError(t, "Expected value, got <" + to_string((int)t->type) + "> (" + t->content + ")"));
	eat();
	return nullptr;
}

Value* SyntaxicAnalyser::eatArrayOrMap() {

	eat(TokenType::OPEN_BRACKET);

	// Empty array
	if (t->type == TokenType::CLOSING_BRACKET) {
		eat();
		return new Array();
	}

	// Empty map
	if (t->type == TokenType::COLON) {
		eat();
		eat(TokenType::CLOSING_BRACKET);
		return new Map();
	}

	// Array For
	if (t->type == TokenType::FOR) {
		ArrayFor* arrayFor = new ArrayFor();
		arrayFor->forr = eatFor();
		eat(TokenType::CLOSING_BRACKET);
		return arrayFor;
	}

	Value* value = eatExpression();

	// eatInterval
	if (t->type == TokenType::TWO_DOTS) {

		Array* interval = new Array();
		interval->interval = true;
		interval->expressions.push_back(value);
		eat();
		interval->expressions.push_back(eatExpression());

		eat(TokenType::CLOSING_BRACKET);
		return interval;
	}

	// eatMap
	if (t->type == TokenType::COLON) {

		Map* map = new Map();
		map->keys.push_back(value);
		eat();
		map->values.push_back(eatExpression());

		while (t->type != TokenType::CLOSING_BRACKET && t->type != TokenType::FINISHED) {
			if (t->type == TokenType::COMMA)
				eat();
			map->keys.push_back(eatExpression());
			eat(TokenType::COLON);
			map->values.push_back(eatExpression());
		}
		eat(TokenType::CLOSING_BRACKET);
		return map;
	}

	// eatArray
	Array* array = new Array();
	array->expressions.push_back(value);

	while (t->type != TokenType::CLOSING_BRACKET && t->type != TokenType::FINISHED) {
		if (t->type == TokenType::COMMA)
			eat();
		array->expressions.push_back(eatExpression());
	}
	eat(TokenType::CLOSING_BRACKET);
	return array;
}

If* SyntaxicAnalyser::eatIf() {

	If* iff = new If();

	eat(TokenType::IF);

	iff->condition = eatExpression();

	bool braces = false;
	bool then = false;
	if (t->type == TokenType::OPEN_BRACE) {
		braces = true;
	} else if (t->type == TokenType::THEN) {
		eat(TokenType::THEN);
		then = true;
	}

	if (then or braces) {
		Value* v = eatBlockOrObject();
		if (dynamic_cast<Block*>(v)) {
			iff->then = (Block*) v;
		} else {
			Block* block = new Block();
			block->instructions.push_back(new ExpressionInstruction(v));
			iff->then = block;
		}
	} else {
		Block* block = new Block();
		block->instructions.push_back(eatInstruction());
		iff->then = block;
	}

	if (then) {
		if (t->type != TokenType::ELSE) {
			eat(TokenType::END);
		}
	}

	if (t->type == TokenType::ELSE) {
		eat();

		bool bracesElse = false;
		if (t->type == TokenType::OPEN_BRACE) {
			bracesElse = true;
		}

		if (then or bracesElse) {
			Value* v = eatBlockOrObject();
			if (dynamic_cast<Block*>(v)) {
				iff->elze = (Block*) v;
			} else {
				Block* block = new Block();
				block->instructions.push_back(new ExpressionInstruction(v));
				iff->elze = block;
			}
		} else {
			Block* body = new Block();
			body->instructions.push_back(eatInstruction());
			iff->elze = body;
		}

		if (then) {
			eat(TokenType::END);
		}
	}

	return iff;
}

Match* SyntaxicAnalyser::eatMatch(bool force_value) {

	Match* match = new Match();

	eat(TokenType::MATCH);

	match->value = eatExpression();

	eat(TokenType::OPEN_BRACE);

	while (t->type != TokenType::CLOSING_BRACE && t->type != TokenType::FINISHED) {
		vector<Match::Pattern> patterns;
		patterns.push_back(eatMatchPattern());
		while (t->type == TokenType::PIPE) {
			eat();
			patterns.push_back(eatMatchPattern());
		}
		match->pattern_list.push_back(patterns);
		eat(TokenType::COLON);
		if (t->type == TokenType::OPEN_BRACE) {
			match->returns.push_back(eatBlockOrObject());
		} else if (force_value) {
			Block* block = new Block();
			block->instructions.push_back(new ExpressionInstruction(eatExpression()));
			match->returns.push_back(block);
		} else {
			Block* body = new Block();
			body->instructions.push_back(eatInstruction());
			match->returns.push_back(body);
		}

		while (t->type == TokenType::SEMICOLON) eat();
	}

	eat(TokenType::CLOSING_BRACE);
	return match;
}

Match::Pattern SyntaxicAnalyser::eatMatchPattern() {

	if (t->type == TokenType::TWO_DOTS) {
		eat();

		if (t->type == TokenType::COLON || t->type == TokenType::PIPE) {
			return Match::Pattern(nullptr, nullptr);
		} else {
			return Match::Pattern(nullptr, eatSimpleExpression());
		}
	}

	Value* value = eatSimpleExpression();

	if (t->type == TokenType::TWO_DOTS) {
		eat();
		if (t->type == TokenType::COLON || t->type == TokenType::PIPE) {
			return Match::Pattern(value, nullptr);
		} else {
			return Match::Pattern(value, eatSimpleExpression());
		}
	} else {
		return Match::Pattern(value);
	}
}

Instruction* SyntaxicAnalyser::eatFor() {

	eat(TokenType::FOR);

	bool parenthesis = false;
	if (t->type == TokenType::OPEN_PARENTHESIS) {
		parenthesis = true;
		eat();
	}

	save_current_state();
	vector<Instruction*> inits;
	while (true) {
		if (t->type == TokenType::FINISHED || t->type == TokenType::SEMICOLON || t->type == TokenType::IN || t->type == TokenType::OPEN_BRACE) {
			break;
		}
		Instruction* ins = eatInstruction();
		if (ins) inits.push_back(ins);
	}

	if (errors.empty() && t->type == TokenType::SEMICOLON) {
		forgot_saved_state();

		// for inits ; condition ; increments { body }
		For* f = new For();

		// init
		f->inits = inits;
		eat(TokenType::SEMICOLON);

		// condition
		f->condition = eatExpression();
		eat(TokenType::SEMICOLON);

		// increment
		while (true) {
			if (t->type == TokenType::FINISHED || t->type == TokenType::SEMICOLON || t->type == TokenType::DO || t->type == TokenType::OPEN_BRACE || t->type == TokenType::CLOSING_PARENTHESIS) {
				break;
			}
			Instruction* ins = eatInstruction();
			if (ins) f->increments.push_back(ins);
		}

		if (parenthesis)
			eat(TokenType::CLOSING_PARENTHESIS);

		// body
		if (t->type == TokenType::OPEN_BRACE) {
			f->body = eatBlock();
		} else {
			eat(TokenType::DO);
			f->body = eatBlock();
			eat(TokenType::END);
		}

		return f;
	} else {
		for (Instruction* ins : inits) delete ins;
		restore_saved_state();

		// for key , value in container { body }
		Foreach* f = new Foreach();

		if (t->type == TokenType::LET) eat();

		if (nt->type == TokenType::COMMA || nt->type == TokenType::COLON) {
			f->key = eatIdent();
			eat();
		}
		if (t->type == TokenType::LET)
			eat();

		f->value = eatIdent();

		eat(TokenType::IN);

		f->container = eatExpression();

		if (parenthesis)
			eat(TokenType::CLOSING_PARENTHESIS);

		// body
		if (t->type == TokenType::OPEN_BRACE) {
			f->body = eatBlock();
		} else {
			eat(TokenType::DO);
			f->body = eatBlock();
			eat(TokenType::END);
		}

		return f;
	}
}

Instruction* SyntaxicAnalyser::eatWhile() {

	eat(TokenType::WHILE);

	While* w = new While();

	bool parenthesis = false;
	bool braces = false;

	if (t->type == TokenType::OPEN_PARENTHESIS) {
		parenthesis = true;
		eat();
	}

	w->condition = eatExpression();

	if (parenthesis) {
		eat(TokenType::CLOSING_PARENTHESIS);
	}
	if (t->type == TokenType::OPEN_BRACE) {
		//eat();
		braces = true;
	} else {
		eat(TokenType::DO);
	}

	w->body = eatBlock();

	if (!braces) {
		eat(TokenType::END);
	}

	return w;
}

Break*SyntaxicAnalyser::eatBreak() {
	eat(TokenType::BREAK);
	Break* b = new Break();

	if (t->type == TokenType::NUMBER /*&& t->line == lt->line*/) {
		double deepness = stod_(t->content);
		if (deepness != int(deepness) || deepness <= 0) {
			errors.push_back(new SyntaxicalError(t, "Break should only be followed by a positive integer"));
		} else {
			b->deepness = deepness;
			eat();
		}
	}

	return b;
}

Continue*SyntaxicAnalyser::eatContinue() {
	eat(TokenType::CONTINUE);
	Continue* c = new Continue();

	if (t->type == TokenType::NUMBER /*&& t->line == lt->line*/) {
		double deepness = stod_(t->content);
		if (deepness != int(deepness) || deepness <= 0) {
			errors.push_back(new SyntaxicalError(t, "Continue should only be followed by a positive integer"));
		} else {
			c->deepness = deepness;
			eat();
		}
	}

	return c;
}

ClassDeclaration* SyntaxicAnalyser::eatClassDeclaration() {

	ClassDeclaration* cd = new ClassDeclaration();

	eat(TokenType::CLASS);
	cd->name = eatIdent()->content;
	eat(TokenType::OPEN_BRACE);

	while (t->type == TokenType::LET) {
		cd->fields.push_back(eatVariableDeclaration());
	}

	eat(TokenType::CLOSING_BRACE);

	return cd;
}

Token* SyntaxicAnalyser::eatIdent() {
	return eat(TokenType::IDENT);
}

Token* SyntaxicAnalyser::eat() {
	return eat(TokenType::DONT_CARE);
}

Token* SyntaxicAnalyser::eat(TokenType type) {

	Token* eaten = t;

	lt = t;
	if (i < tokens.size() - 1) {
		t = &tokens[++i];
	} else {
		t = new Token(TokenType::FINISHED, 0, 0, "");
	}
	nt = i < tokens.size() - 1 ? &tokens[i + 1] : nullptr;

	if (type != TokenType::DONT_CARE && eaten->type != type) {
		errors.push_back(new SyntaxicalError(eaten, "Expected token of type <" + to_string((int)type) + ">, got <" + to_string((int)eaten->type) + "> (" + eaten->content + ")"));
		return new Token(type, 0, 0, "**Error**");
	}
	return eaten;
}

Token* SyntaxicAnalyser::nextTokenAt(int pos) {
	if (i + pos < tokens.size())
		return &tokens[i + pos];
	else
		return nullptr;
}

void SyntaxicAnalyser::save_current_state() {
	stack.push_back(make_pair(i, errors.size()));
}

void SyntaxicAnalyser::restore_saved_state() {
	if (!stack.empty()) {
		i = stack.back().first;
		errors.resize(stack.back().second);
		stack.pop_back();

		lt = i > 0 ? &tokens[i-1] : nullptr;
		t = i < tokens.size() ? &tokens[i] : nullptr;
		nt = i+1 < tokens.size() ? &tokens[i+1] : nullptr;
	}
}

void SyntaxicAnalyser::forgot_saved_state() {
	stack.pop_back();
}

vector<SyntaxicalError*> SyntaxicAnalyser::getErrors() {
	return errors;
}

long SyntaxicAnalyser::getTime() {
	return time;
}

}
