#include "SyntaxicAnalyzer.hpp"
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
#include "../instruction/Throw.hpp"
#include "../instruction/While.hpp"
#include "../value/AbsoluteValue.hpp"
#include "../value/Array.hpp"
#include "../value/Interval.hpp"
#include "../value/Map.hpp"
#include "../value/Set.hpp"
#include "../value/ArrayAccess.hpp"
#include "../value/Boolean.hpp"
#include "../value/FunctionCall.hpp"
#include "../value/Nulll.hpp"
#include "../value/Number.hpp"
#include "../value/Object.hpp"
#include "../value/ObjectAccess.hpp"
#include "../value/PostfixExpression.hpp"
#include "../value/PrefixExpression.hpp"
#include "../value/String.hpp"
#include "../value/VariableValue.hpp"
#include "../value/ArrayFor.hpp"
#include "../../vm/Program.hpp"
#include "../../vm/value/LSNumber.hpp"
#include "../lexical/Token.hpp"
#include "../../util/Util.hpp"
#include "../lexical/LexicalAnalyzer.hpp"
#include "../resolver/Resolver.hpp"
#include "../error/Error.hpp"
#include "../semantic/FunctionVersion.hpp"

namespace ls {

SyntaxicAnalyzer::SyntaxicAnalyzer(Resolver* resolver) : resolver(resolver) {
	time = 0;
	nt = nullptr;
	t = nullptr;
	i = 0;
}

Block* SyntaxicAnalyzer::analyze(File* file) {
	this->file = file;

	// Call the lexical analyzer to parse tokens
	file->tokens = LexicalAnalyzer().analyze(file);

	this->t = &file->tokens.at(0);
	this->nt = file->tokens.size() > 1 ? &file->tokens.at(1) : nullptr;
	this->i = 0;

	return eatMain(file);
}

Block* SyntaxicAnalyzer::eatMain(File* file) {

	auto block = new Block(true);

	while (true) {
		if (t->type == TokenType::FINISHED) {
			eat();
			break;
		} else if (t->type == TokenType::SEMICOLON) {
			eat();
		} else {
			auto ins = eatInstruction();
			// Include instruction
			if (auto ei = dynamic_cast<ExpressionInstruction*>(ins)) {
				if (auto fc = dynamic_cast<FunctionCall*>(ei->value.get())) {
					if (auto vv = dynamic_cast<VariableValue*>(fc->function.get())) {
						if (fc->arguments.size() == 1) {
							auto str = dynamic_cast<String*>(fc->arguments.at(0).get());
							if (vv->name == "include" and str) {
								auto included_file = resolver->resolve(str->token->content, file->context);
								auto included_block = SyntaxicAnalyzer(resolver).analyze(included_file);
								for (auto& ins : included_block->instructions) {
									block->instructions.push_back(std::move(ins));
								}
								continue;
							}
						}
					}
				}
			}
			if (ins) {
				block->instructions.emplace_back(ins);
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
bool SyntaxicAnalyzer::isObject() {

	if (nt != nullptr and nt->type == TokenType::CLOSING_BRACE) {
		return true;
	}

	auto nnt = nextTokenAt(2);
	if (nt != nullptr and nt->type == TokenType::IDENT and nnt != nullptr and nnt->type == TokenType::COLON) {
		return true;
	}
	return false;
}

Value* SyntaxicAnalyzer::eatBlockOrObject() {
	if (isObject()) {
		return eatObject();
	} else {
		return eatBlock();
	}
}

Block* SyntaxicAnalyzer::eatBlock(bool is_function_block) {

	Block* block = new Block(is_function_block);

	bool brace = false;
	if (t->type == TokenType::OPEN_BRACE) {
		brace = true;
		eat();
	}

	while (true) {
		if (t->type == TokenType::CLOSING_BRACE) {
			eat();
			break;
		} else if (t->type == TokenType::FINISHED || t->type == TokenType::ELSE || t->type == TokenType::END || t->type == TokenType::IN) {
			if (brace) {
				file->errors.push_back(Error(Error::Type::BLOCK_NOT_CLOSED, t, {}));
			}
			break;
		} else if (t->type == TokenType::SEMICOLON) {
			eat();
		} else {
			Instruction* ins = eatInstruction();
			if (ins) block->instructions.emplace_back(ins);
		}
	}

	return block;
}

Object* SyntaxicAnalyzer::eatObject() {

	eat(TokenType::OPEN_BRACE);

	auto o = new Object();

	while (t->type == TokenType::IDENT) {

		o->keys.push_back(eatIdent());
		eat(TokenType::COLON);
		o->values.emplace_back(eatExpression());

		if (t->type == TokenType::COMMA) {
			eat();
		}
	}
	eat(TokenType::CLOSING_BRACE);

	return o;
}

Instruction* SyntaxicAnalyzer::eatInstruction() {

	switch (t->type) {

		case TokenType::LET:
		case TokenType::VAR:
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
		case TokenType::AROBASE:
		case TokenType::PLUS:
		case TokenType::TIMES:
		case TokenType::DIVIDE:
		case TokenType::INT_DIV:
		case TokenType::POWER:
		case TokenType::MODULO:
		case TokenType::PIPE:
		case TokenType::TILDE:
		case TokenType::LOWER:
		case TokenType::GREATER:
		case TokenType::LOWER_EQUALS:
		case TokenType::GREATER_EQUALS:
		case TokenType::OR:
			return new ExpressionInstruction(std::unique_ptr<Value>(eatExpression()));

		case TokenType::MATCH:
			return new ExpressionInstruction(std::unique_ptr<Value>(eatMatch(false)));

		case TokenType::FUNCTION:
			return eatFunctionDeclaration();

		case TokenType::RETURN: {
			eat();
			if (t->type == TokenType::FINISHED or t->type == TokenType::CLOSING_BRACE
				or t->type == TokenType::ELSE or t->type == TokenType::END or t->type == TokenType::SEMICOLON) {
				return new Return();
			} else {
				return new Return(std::unique_ptr<Value>(eatExpression()));
			}
		}
		case TokenType::THROW: {
			auto throw_token = eat_get();
			if (t->type == TokenType::FINISHED or t->type == TokenType::CLOSING_BRACE
				or t->type == TokenType::ELSE or t->type == TokenType::END) {
				return new Throw(throw_token);
			} else {
				return new Throw(throw_token, std::unique_ptr<Value>(eatExpression()));
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
			// std::cout << "Unexpected token : " << (int)t->type << " (" << t->content << ")" << std::endl;
			file->errors.push_back(Error(Error::Type::UNEXPECTED_TOKEN, t, {t->content}));
			eat();
			return nullptr;
	}
}

VariableDeclaration* SyntaxicAnalyzer::eatVariableDeclaration() {

	auto vd = new VariableDeclaration();

	if (t->type == TokenType::GLOBAL) {
		vd->keyword = eat_get(TokenType::GLOBAL);
		vd->global = true;
	} else if (t->type == TokenType::LET) {
		vd->keyword = eat_get(TokenType::LET);
		vd->constant = true;
	} else {
		vd->keyword = eat_get(TokenType::VAR);
	}

	auto ident = eatIdent();
	vd->variables.push_back(ident);
	if (t->type == TokenType::EQUAL) {
		eat(TokenType::EQUAL);
		vd->expressions.emplace_back(eatExpression());
	} else {
		vd->expressions.push_back(nullptr);
	}

	while (t->type == TokenType::COMMA) {
		eat();
		ident = eatIdent();
		vd->variables.push_back(ident);
		if (t->type == TokenType::EQUAL) {
			eat(TokenType::EQUAL);
			vd->expressions.emplace_back(eatExpression());
		} else {
			vd->expressions.push_back(nullptr);
		}
	}
	return vd;
}

Function* SyntaxicAnalyzer::eatFunction(Token* token) {

	if (t->type == TokenType::FUNCTION) {
		token = eat_get();
	}

	auto f = new Function(token);

	eat(TokenType::OPEN_PARENTHESIS);

	while (t->type != TokenType::FINISHED && t->type != TokenType::CLOSING_PARENTHESIS) {

		auto ident = eatIdent();

		Value* defaultValue = nullptr;
		if (t->type == TokenType::EQUAL) {
			eat();
			defaultValue = eatExpression();
		}

		f->addArgument(ident, defaultValue);

		if (t->type == TokenType::COMMA) {
			eat();
		}
	}
	eat(TokenType::CLOSING_PARENTHESIS);

	bool braces = false;
	if (t->type == TokenType::OPEN_BRACE) {
		braces = true;
	}

	f->body = eatBlock(true);

	if (!braces) {
		eat(TokenType::END);
	}

	return f;
}

VariableDeclaration* SyntaxicAnalyzer::eatFunctionDeclaration() {

	auto token = eat_get(TokenType::FUNCTION);

	auto vd = new VariableDeclaration();
	vd->global = true;
	vd->function = true;

	vd->variables.emplace_back(eatIdent());
	vd->expressions.emplace_back(eatFunction(token));

	return vd;
}

bool SyntaxicAnalyzer::beginingOfExpression(TokenType type) {

	return type == TokenType::NUMBER or type == TokenType::IDENT
		or type == TokenType::AROBASE or type == TokenType::OPEN_BRACKET
		or type == TokenType::OPEN_BRACE or type == TokenType::OPEN_PARENTHESIS
		or type == TokenType::STRING or type == TokenType::PI or type == TokenType::TRUE
		or type == TokenType::FALSE or type == TokenType::NULLL;
}

int SyntaxicAnalyzer::findNextClosingParenthesis() {
	int p = i;
	int level = 1;
	while (level > 0) {
		auto t = file->tokens.at(p++).type;
		if (t == TokenType::FINISHED) return -1;
		if (t == TokenType::OPEN_PARENTHESIS) level++;
		if (t == TokenType::CLOSING_PARENTHESIS) level--;
	}
	return p - 1;
}

int SyntaxicAnalyzer::findNextArrow() {
	int p = i;
	while (true) {
		auto t = file->tokens.at(p++).type;
		if (t == TokenType::FINISHED) return -1;
		if (t == TokenType::ARROW) break;
	}
	return p - 1;
}

int SyntaxicAnalyzer::findNextColon() {
	int p = i;
	while (true) {
		auto t = file->tokens.at(p++).type;
		if (t == TokenType::FINISHED) return -1;
		if (t == TokenType::COLON) break;
	}
	return p - 1;
}

void SyntaxicAnalyzer::splitCurrentOrInTwoPipes() {
	file->tokens.erase(file->tokens.begin() + i);
	file->tokens.insert(file->tokens.begin() + i, { TokenType::PIPE, file, t->location.end.raw, t->location.start.line, t->location.end.column, "|" });
	file->tokens.insert(file->tokens.begin() + i + 1, { TokenType::PIPE, file, t->location.end.raw + 1, t->location.start.line, t->location.end.column + 1, "|" });
	t = &file->tokens.at(i);
	nt = &file->tokens.at(i + 1);
}

Value* SyntaxicAnalyzer::eatSimpleExpression(bool pipe_opened, bool set_opened, bool comma_list, Value* initial) {

	Value* e = nullptr;

	if (initial == nullptr) {
		if (t->type == TokenType::OPEN_PARENTHESIS) {

			e = eatLambdaOrParenthesisExpression(pipe_opened, set_opened, comma_list);

		} else if (t->type == TokenType::PIPE or (t->type == TokenType::OR and t->content == "||")) {

			// If we start a instruction with a ||, split it into two |, and it's an absolute value expression
			if (t->type == TokenType::OR) {
				splitCurrentOrInTwoPipes();
			}

			auto open_pipe = eat_get();
			auto av = new AbsoluteValue();
			av->open_pipe = open_pipe;
			av->expression = std::unique_ptr<Value>(eatExpression(true));

			// We want a closing pipe, if there's a || operator, we split it
			if (t->type == TokenType::OR and t->content == "||") {
				splitCurrentOrInTwoPipes();
			}
			av->close_pipe = eat_get(TokenType::PIPE);
			e = new Expression(av);

		} else {

			// Opérateurs unaires préfixe
			if (t->type == TokenType::NEW || t->type == TokenType::MINUS ||
				t->type == TokenType::PLUS || t->type == TokenType::NOT ||
				t->type == TokenType::MINUS_MINUS || t->type == TokenType::PLUS_PLUS
				|| t->type == TokenType::TILDE) {

				if (t->type == TokenType::MINUS && nt != nullptr && t->location.start.line == nt->location.start.line) {
					auto minus = eat_get();
					if (beginingOfExpression(t->type)) {

						auto op = new Operator(minus);
						auto ex = eatExpression(pipe_opened, set_opened);
						auto expr = dynamic_cast<Expression*>(ex);

						if (expr and expr->op->priority >= op->priority) {
							auto pexp = new PrefixExpression(std::shared_ptr<Operator>(op), std::move(expr->v1));
							expr->v1.reset(pexp);
							e = expr;
						} else {
							auto pe = new PrefixExpression(std::shared_ptr<Operator>(op), std::unique_ptr<Value>(ex));
							e = pe;
						}
					} else {
						// No expression after the -, so it's the variable '-'
						e = new VariableValue(minus);
					}

				} else if (t->type == TokenType::PLUS) {

					auto plus = eat_get(); // eat the +

					if (beginingOfExpression(t->type)) {
						e = eatExpression(pipe_opened);
					} else {
						// No expression after the +, so it's the variable '+'
						e = new VariableValue(plus);
					}
				} else if (t->type == TokenType::TILDE) {

					auto tilde = eat_get(); // eat the ~

					if (beginingOfExpression(t->type)) {
						auto op = std::make_shared<Operator>(tilde);
						auto ex = new PrefixExpression(op, std::unique_ptr<Value>(eatSimpleExpression()));
						e = new Expression(ex);
					} else {
						// No expression after the ~, so it's the variable '~'
						e = new VariableValue(tilde);
					}
				} else {
					auto op = std::make_shared<Operator>(eat_get());
					auto ex = new PrefixExpression(op, std::unique_ptr<Value>(eatSimpleExpression()));
					e = new Expression(ex);
				}
			} else {
				e = eatValue(comma_list);
			}
		}
	} else {
		e = initial;
	}

	while (t->type == TokenType::OPEN_BRACKET || t->type == TokenType::OPEN_PARENTHESIS || t->type == TokenType::DOT) {

		if (t->location.start.column != last_character + last_size)
			break;

		switch (t->type) {
			case TokenType::OPEN_BRACKET: {

				auto aa = new ArrayAccess();
				aa->open_bracket = eat_get(TokenType::OPEN_BRACKET);

				aa->array = std::unique_ptr<Value>(e);
				if (t->type == TokenType::CLOSING_BRACKET) {
					aa->key = nullptr;
				} else {
					aa->key = std::unique_ptr<Value>(eatExpression());
				}
				if (t->type == TokenType::COLON) {
					eat();
					aa->key2 = std::unique_ptr<Value>(eatExpression());
				}
				aa->close_bracket = eat_get(TokenType::CLOSING_BRACKET);
				e = aa;
				break;
			}
			case TokenType::OPEN_PARENTHESIS: {

				auto par = eat_get(TokenType::OPEN_PARENTHESIS);

				auto fc = new FunctionCall(par);
				fc->function = std::unique_ptr<Value>(e);

				if (t->type != TokenType::CLOSING_PARENTHESIS) {
					fc->arguments.emplace_back(eatExpression(false, false, nullptr, true));
					while (t->type != TokenType::CLOSING_PARENTHESIS && t->type != TokenType::FINISHED) {
						if (t->type == TokenType::COMMA) {
							eat();
						}
						fc->arguments.emplace_back(eatExpression(false, false, nullptr, true));
					}
				}
				fc->closing_parenthesis = eat_get(TokenType::CLOSING_PARENTHESIS);

				e = fc;
				break;
			}
			case TokenType::DOT: {

				ObjectAccess* oa;
				eat(TokenType::DOT);

				if (t->type == TokenType::NEW || t->type == TokenType::CLASS) {
					oa = new ObjectAccess(eat_get());
				} else if (t->type == TokenType::RETURN) {
					oa = new ObjectAccess(eat_get());
				} else {
					oa = new ObjectAccess(eatIdent());
				}
				oa->object = std::unique_ptr<Value>(e);
				e = oa;
				break;
			}
			default: {}
		}
	}

	// Opérateurs unaires postfixes
	if (t->type == TokenType::MINUS_MINUS || t->type == TokenType::PLUS_PLUS) {

		if (last_line == t->location.start.line) {

			auto op = eat_get();
			auto ex = new PostfixExpression();

			ex->operatorr = std::make_shared<Operator>(op);
			ex->expression = std::unique_ptr<LeftValue>((LeftValue*) e);
			e = ex;
		}
	}

	return e;
}

Value* SyntaxicAnalyzer::eatExpression(bool pipe_opened, bool set_opened, Value* initial, bool comma_list) {

	Expression* ex = nullptr;
	Value* e = (initial != nullptr) ? initial : eatSimpleExpression(pipe_opened, set_opened, comma_list);

	// Opérateurs binaires
	while (t->type == TokenType::PLUS || t->type == TokenType::MINUS ||
		   t->type == TokenType::TIMES || t->type == TokenType::DIVIDE ||
		   t->type == TokenType::INT_DIV || t->type == TokenType::INT_DIV_EQUAL ||
		   t->type == TokenType::MODULO || t->type == TokenType::AND ||
		   (!pipe_opened and t->type == TokenType::OR and t->content == "||") || (t->type == TokenType::OR and t->content == "or")
		   || t->type == TokenType::XOR ||
		   t->type == TokenType::EQUAL || t->type == TokenType::POWER ||
		   t->type == TokenType::DOUBLE_EQUAL || t->type == TokenType::DIFFERENT ||
		   t->type == TokenType::TRIPLE_EQUAL || t->type == TokenType::TRIPLE_DIFFERENT ||
		   (!set_opened && t->type == TokenType::GREATER) || t->type == TokenType::LOWER ||
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
		   t->type == TokenType::DOUBLE_QUESTION_MARK || t->type == TokenType::CATCH_ELSE || t->type == TokenType::DOUBLE_MODULO || t->type == TokenType::DOUBLE_MODULO_EQUALS
		   ) {

		if (t->type == TokenType::MINUS && t->location.start.line != last_line && nt != nullptr && t->location.start.line == nt->location.start.line)
			break;

		auto op = new Operator(eat_get());

		if (ex == nullptr) {
			if (auto exx = dynamic_cast<Expression*>(e)) {
				ex = exx;
			} else {
				ex = new Expression();
				ex->v1.reset(e);
			}
		}
		ex->append(std::shared_ptr<Operator>(op), eatSimpleExpression());
	}
	if (ex != nullptr) {
		e = ex;
	}

	// Ternary
	if (t->type == TokenType::QUESTION_MARK) {
		eat();
		auto ternary = new If(true);
		ternary->condition = std::unique_ptr<Value>(e);

		auto then = new Block();
		then->instructions.emplace_back(new ExpressionInstruction(std::unique_ptr<Value>(eatExpression())));
		ternary->then = std::unique_ptr<Block>(then);

		eat(TokenType::COLON);

		auto elze = new Block();
		elze->instructions.emplace_back(new ExpressionInstruction(std::unique_ptr<Value>(eatExpression())));
		ternary->elze = std::unique_ptr<Block>(elze);
		return ternary;
	}

	return e;
}

Value* SyntaxicAnalyzer::eatValue(bool comma_list) {

	switch (t->type) {

		case TokenType::PLUS:
		case TokenType::MINUS:
		case TokenType::MODULO:
		case TokenType::TIMES:
		case TokenType::DIVIDE:
		case TokenType::POWER:
		case TokenType::TERNARY:
		case TokenType::INT_DIV:
		case TokenType::TILDE:
		case TokenType::GREATER:
		case TokenType::LOWER_EQUALS:
		case TokenType::GREATER_EQUALS:
		{
			return new VariableValue(eat_get());
		}

		case TokenType::NUMBER:
		{
			auto n_token = eat_get();
			auto n = new Number(n_token->content, n_token);

			if (t->type == TokenType::STAR) {
				n->pointer = true;
				eat();
			}
			return n;
		}

		case TokenType::PI: {
			std::stringstream stream;
			stream << std::fixed << std::setprecision(19) << M_PI;
			return new Number(stream.str(), eat_get());
		}
		case TokenType::STRING:
		{
			return new String(eat_get());
		}

		case TokenType::TRUE:
		case TokenType::FALSE:
		{
			return new Boolean(eat_get());
		}

		case TokenType::NULLL:
			return new Nulll(eat_get());

		case TokenType::AROBASE:
		case TokenType::IDENT:
			return eatLambdaOrParenthesisExpression(false, false, comma_list);
			break;

		case TokenType::OPEN_BRACKET:
			return eatArrayOrMap();

		case TokenType::LOWER:
			return eatSetOrLowerOperator();

		case TokenType::OPEN_BRACE:
			return eatBlockOrObject();

		case TokenType::IF:
			return eatIf();

		case TokenType::MATCH:
			return eatMatch(true);

		case TokenType::FUNCTION:
			return eatFunction(nullptr);

		case TokenType::ARROW:
		{	
			auto token = eat_get(TokenType::ARROW);
			Function* l = new Function(token);
			l->lambda = true;
			l->body = new Block(true);
			l->body->instructions.emplace_back(new ExpressionInstruction(std::unique_ptr<Value>(eatExpression())));
			return l;
		}

		default:
			break;
	}

	file->errors.push_back(Error(Error::EXPECTED_VALUE, t, {t->content}));
	eat();
	return nullptr;
}

/*
 * Starting from:
 *   <ident>, <open_parenthesis>, <arobase>
 * Can return:
 *   <variable_value>, <expression>, <lambda>
 */
Value* SyntaxicAnalyzer::eatLambdaOrParenthesisExpression(bool pipe_opened, bool set_opened, bool comma_list) {
	bool parenthesis = false;
	if (t->type == TokenType::OPEN_PARENTHESIS) {
		eat();
		parenthesis = true;
	}
	if (parenthesis and t->type != TokenType::IDENT) {
		if (t->type == TokenType::CLOSING_PARENTHESIS) {
			return eatValue(); // error, expected a value got ')', it's wrong
		}
		// (...)
		auto ex = eatExpression();
		ex->parenthesis = true;
		eat(TokenType::CLOSING_PARENTHESIS);
		return ex;
	}
	auto ident = eatIdent();
	// var
	if (t->type == TokenType::EQUAL) {
		// var =
		auto eq = eat_get();
		auto ex = eatExpression();
		// var = <ex>
		if (parenthesis and t->type == TokenType::CLOSING_PARENTHESIS) {
			// (var = <ex>)
			eat();
			if (t->type == TokenType::ARROW) {
				// (var = <ex>) ->  [lambda]
				return eatLambdaContinue(false, ident, ex, comma_list);
			} else {
				// (var = <ex>) <token ?>	[expression]
				Expression* e = new Expression();
				e->parenthesis = true;
				e->v1.reset(new VariableValue(ident));
				e->op = std::make_shared<Operator>(eq);
				e->v2.reset(ex);
				return e;
			}
		} else if (t->type == TokenType::COMMA or t->type == TokenType::ARROW) {
			// var = <ex> ,|->  [lambda]
			return eatLambdaContinue(parenthesis, ident, ex, comma_list);
		} else {
			// var = <ex> <?>
			Expression* e = new Expression();
			e->v1.reset(new VariableValue(ident));
			e->op = std::make_shared<Operator>(eq);
			e->v2.reset(ex);
			return eatExpression(pipe_opened, set_opened, e);
		}
	} else if (t->type == TokenType::ARROW) {
		// var ->
		return eatLambdaContinue(parenthesis, ident, nullptr, comma_list);
	} else if (t->type == TokenType::COMMA) {
		// var,  [lambda]
		if (!parenthesis && comma_list) {
			return new VariableValue(ident);
		}
		int p = findNextClosingParenthesis();
		int a = findNextArrow();
		int c = findNextColon();
		if (parenthesis or (a != -1 and (a < p or p == -1) and (a < c or c == -1))) {
			return eatLambdaContinue(parenthesis, ident, nullptr, comma_list);
		} else {
			return new VariableValue(ident);
		}
	} else {
		if (parenthesis) {
			if (t->type == TokenType::CLOSING_PARENTHESIS) {
				// (var)
				eat();
				if (t->type == TokenType::ARROW) {
					return eatLambdaContinue(false, ident, nullptr, comma_list);
				}
				return new VariableValue(ident);
			} else {
				// ( var + ... )
				auto v = new VariableValue(ident);
				auto exx = eatSimpleExpression(false, false, false, v);
				auto ex = eatExpression(pipe_opened, set_opened, exx);
				ex->parenthesis = true;
				eat(TokenType::CLOSING_PARENTHESIS);
				return ex;
			}
		}
		// var <?>  [expression]
		return new VariableValue(ident);
	}
}

/*
 * Continue to eat a lambda starting from a comma or the arrow
 */
Value* SyntaxicAnalyzer::eatLambdaContinue(bool parenthesis, Ident ident, Value* expression, bool comma_list) {
	auto l = new Function(nullptr);
	l->lambda = true;
	// Add first argument
	l->addArgument(ident.token, expression);
	// Add other arguments
	while (t->type == TokenType::COMMA) {
		eat();
		auto ident = eatIdent();
		Value* defaultValue = nullptr;
		if (t->type == TokenType::EQUAL) {
			eat();
			defaultValue = eatExpression();
		}
		l->addArgument(ident, defaultValue);
	}
	if (t->type == TokenType::CLOSING_PARENTHESIS) {
		eat();
		parenthesis = false;
	}
	auto token = eat_get(TokenType::ARROW);
	l->token = token;
	l->body = new Block(true);
	l->body->instructions.emplace_back(new ExpressionInstruction(std::unique_ptr<Value>(eatExpression(false, false, nullptr, comma_list))));
	if (parenthesis) {
		eat(TokenType::CLOSING_PARENTHESIS);
	}
	return l;
}

Value* SyntaxicAnalyzer::eatArrayOrMap() {

	auto opening_bracket = eat_get(TokenType::OPEN_BRACKET);

	// Empty array
	if (t->type == TokenType::CLOSING_BRACKET) {
		auto array = new Array();
		array->opening_bracket = opening_bracket;
		array->closing_bracket = eat_get();
		return array;
	}

	// Empty map
	if (t->type == TokenType::COLON) {
		eat();
		auto map = new Map();
		map->opening_bracket = opening_bracket;
		map->closing_bracket = eat_get();
		return map;
	}

	// Array For
	if (t->type == TokenType::FOR) {
		ArrayFor* arrayFor = new ArrayFor();
		arrayFor->forr = std::unique_ptr<Instruction>(eatFor());
		eat(TokenType::CLOSING_BRACKET);
		return arrayFor;
	}

	Value* value = eatExpression(false, false, nullptr, true);

	// eatInterval
	if (t->type == TokenType::TWO_DOTS) {

		Interval* interval = new Interval();
		interval->opening_bracket = opening_bracket;
		interval->start = std::unique_ptr<Value>(value);
		eat();
		interval->end = std::unique_ptr<Value>(eatExpression());

		interval->closing_bracket = eat_get(TokenType::CLOSING_BRACKET);
		return interval;
	}

	// eatMap
	if (t->type == TokenType::COLON) {

		auto map = new Map();
		map->opening_bracket = opening_bracket;
		map->keys.emplace_back(value);
		eat();
		map->values.emplace_back(eatExpression());

		while (t->type != TokenType::CLOSING_BRACKET && t->type != TokenType::FINISHED) {
			if (t->type == TokenType::COMMA)
				eat();
			map->keys.emplace_back(eatExpression());
			eat(TokenType::COLON);
			map->values.emplace_back(eatExpression());
		}
		map->closing_bracket = eat_get(TokenType::CLOSING_BRACKET);
		return map;
	}

	// eatArray
	auto array = new Array();
	array->opening_bracket = opening_bracket;
	array->expressions.emplace_back(value);
	if (t->type == TokenType::COMMA) {
		eat();
	}
	while (t->type != TokenType::CLOSING_BRACKET && t->type != TokenType::FINISHED) {
		array->expressions.emplace_back(eatExpression(false, false, nullptr, true));
		if (t->type == TokenType::COMMA)
			eat();
	}
	array->closing_bracket = eat_get(TokenType::CLOSING_BRACKET);
	return array;
}

Value* SyntaxicAnalyzer::eatSetOrLowerOperator() {
	
	auto lower = eat_get();

	if (t->type == TokenType::CLOSING_PARENTHESIS or t->type == TokenType::COMMA or t->type == TokenType::SEMICOLON or t->type == TokenType::DOT) {
		return new VariableValue(lower);
	}

	auto set = new Set();

	if (t->type == TokenType::GREATER) {
		eat();
		return set;
	}

	set->expressions.emplace_back(eatExpression(false, true));

	while (t->type != TokenType::GREATER && t->type != TokenType::FINISHED) {
		if (t->type == TokenType::COMMA) eat();
		set->expressions.emplace_back(eatExpression(false, true));
	}
	eat(TokenType::GREATER);

	return set;
}

If* SyntaxicAnalyzer::eatIf() {

	auto iff = new If();

	eat(TokenType::IF);

	iff->condition = std::unique_ptr<Value>(eatExpression());

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
			iff->then = std::unique_ptr<Block>((Block*) v);
		} else {
			auto block = new Block();
			block->instructions.emplace_back(new ExpressionInstruction(std::unique_ptr<Value>(v)));
			iff->then = std::unique_ptr<Block>(block);
		}
	} else {
		Block* block = new Block();
		block->instructions.emplace_back(eatInstruction());
		iff->then = std::unique_ptr<Block>(block);
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
				iff->elze = std::unique_ptr<Block>((Block*) v);
			} else {
				Block* block = new Block();
				block->instructions.emplace_back(new ExpressionInstruction(std::unique_ptr<Value>(v)));
				iff->elze = std::unique_ptr<Block>(block);
			}
		} else {
			Block* body = new Block();
			body->instructions.emplace_back(eatInstruction());
			iff->elze = std::unique_ptr<Block>(body);
		}
	}

	if (then) {
		eat(TokenType::END);
	}

	return iff;
}

Match* SyntaxicAnalyzer::eatMatch(bool force_value) {

	Match* match = new Match();

	eat(TokenType::MATCH);

	match->value.reset(eatExpression());

	eat(TokenType::OPEN_BRACE);

	while (t->type != TokenType::CLOSING_BRACE && t->type != TokenType::FINISHED) {
		std::vector<Match::Pattern> patterns;
		patterns.push_back(eatMatchPattern());
		while (t->type == TokenType::PIPE) {
			eat();
			patterns.push_back(eatMatchPattern());
		}
		match->pattern_list.emplace_back(std::move(patterns));
		eat(TokenType::COLON);
		if (t->type == TokenType::OPEN_BRACE) {
			match->returns.emplace_back(eatBlockOrObject());
		} else if (force_value) {
			Block* block = new Block();
			block->instructions.emplace_back(new ExpressionInstruction(std::unique_ptr<Value>(eatExpression())));
			match->returns.emplace_back(block);
		} else {
			Block* body = new Block();
			body->instructions.emplace_back(eatInstruction());
			match->returns.emplace_back(body);
		}

		while (t->type == TokenType::SEMICOLON) eat();
	}

	eat(TokenType::CLOSING_BRACE);
	return match;
}

Match::Pattern SyntaxicAnalyzer::eatMatchPattern() {

	if (t->type == TokenType::TWO_DOTS) {
		eat();

		if (t->type == TokenType::COLON || t->type == TokenType::PIPE) {
			return Match::Pattern(std::unique_ptr<Value>(nullptr), std::unique_ptr<Value>(nullptr));
		} else {
			return Match::Pattern(std::unique_ptr<Value>(nullptr), std::unique_ptr<Value>(eatSimpleExpression()));
		}
	}

	Value* value = eatSimpleExpression();

	if (t->type == TokenType::TWO_DOTS) {
		eat();
		if (t->type == TokenType::COLON || t->type == TokenType::PIPE) {
			return Match::Pattern(std::unique_ptr<Value>(value), std::unique_ptr<Value>(nullptr));
		} else {
			return Match::Pattern(std::unique_ptr<Value>(value), std::unique_ptr<Value>(eatSimpleExpression()));
		}
	} else {
		return Match::Pattern(std::unique_ptr<Value>(value));
	}
}

Instruction* SyntaxicAnalyzer::eatFor() {

	auto for_token = eat_get(TokenType::FOR);

	bool parenthesis = false;
	if (t->type == TokenType::OPEN_PARENTHESIS) {
		parenthesis = true;
		eat();
	}

	// Foreach:
	// for v in || for var v in || for k, v in
	// for var k, v in || for k, var v in || for var k, var v in
	auto t1 = t->type, t2 = nt->type, t3 = nextTokenAt(2)->type, t4 = nextTokenAt(3)->type, t5 = nextTokenAt(4)->type, t6 = nextTokenAt(5)->type;
	auto var = TokenType::VAR, let = TokenType::LET, ident = TokenType::IDENT, comma = TokenType::COMMA, colon = TokenType::COLON, in = TokenType::IN;
	bool is_foreach = (t1 == ident and t2 == in)
		or ((t1 == var or t1 == let) and t2 == ident and t3 == in)
		or (t1 == ident and (t2 == comma or t2 == colon) and t3 == ident and t4 == in)
		or ((t1 == var or t1 == let) and t2 == ident and (t3 == comma or t3 == colon) and t4 == ident and t5 == in)
		or (t1 == ident and (t2 == comma or t2 == colon) and (t3 == var or t3 == let) and t4 == ident and t5 == in)
		or ((t1 == var or t1 == let) and t2 == ident and (t3 == comma or t3 == colon) and (t4 == var or t4 == let) and t5 == ident and t6 == in);

	if (!is_foreach) {

		auto init_block = new Block(false);
		while (true) {
			if (t->type == TokenType::FINISHED || t->type == TokenType::SEMICOLON || t->type == TokenType::IN || t->type == TokenType::OPEN_BRACE) {
				break;
			}
			auto ins = eatInstruction();
			if (ins) init_block->instructions.emplace_back(ins);
		}

		// for inits; condition; increments { body }
		auto f = new For();
		f->token = for_token;

		// init
		f->init = std::unique_ptr<Block>(init_block);
		eat(TokenType::SEMICOLON);

		// condition
		if (t->type == TokenType::SEMICOLON) {
			f->condition = nullptr;
			eat();
		} else {
			f->condition = std::unique_ptr<Value>(eatExpression());
			eat(TokenType::SEMICOLON);
		}

		// increment
		auto increment_block = new Block(false);
		while (true) {
			if (t->type == TokenType::FINISHED || t->type == TokenType::SEMICOLON || t->type == TokenType::DO || t->type == TokenType::OPEN_BRACE || t->type == TokenType::CLOSING_PARENTHESIS) {
				break;
			}
			auto ins = eatInstruction();
			if (ins) increment_block->instructions.emplace_back(ins);
			if (t->type == TokenType::COMMA) {
				eat(TokenType::COMMA);
			}
		}
		f->increment = std::unique_ptr<Block>(increment_block);

		if (parenthesis)
			eat(TokenType::CLOSING_PARENTHESIS);

		// body
		if (t->type == TokenType::OPEN_BRACE) {
			f->body = std::unique_ptr<Block>(eatBlock());
		} else if (t->type == TokenType::DO) {
			eat(TokenType::DO);
			f->body = std::unique_ptr<Block>(eatBlock());
			eat(TokenType::END);
		} else {
			auto block = new Block();
			block->instructions.emplace_back(eatInstruction());
			f->body = std::unique_ptr<Block>(block);
		}
		return f;

	} else {

		// for key , value in container { body }
		auto f = new Foreach();

		if (t->type == TokenType::LET or t->type == TokenType::VAR) eat();

		if (nt->type == TokenType::COMMA || nt->type == TokenType::COLON) {
			f->key = eatIdent();
			eat();
		}
		if (t->type == TokenType::LET or t->type == TokenType::VAR) eat();

		f->value = eatIdent();

		eat(TokenType::IN);

		f->container = std::unique_ptr<Value>(eatExpression());

		if (parenthesis)
			eat(TokenType::CLOSING_PARENTHESIS);

		// body
		if (t->type == TokenType::OPEN_BRACE) {
			f->body = std::unique_ptr<Block>(eatBlock());
		} else {
			eat(TokenType::DO);
			f->body = std::unique_ptr<Block>(eatBlock());
			eat(TokenType::END);
		}

		return f;
	}
}

Instruction* SyntaxicAnalyzer::eatWhile() {

	auto while_token = eat_get(TokenType::WHILE);

	auto w = new While();
	w->token = while_token;

	bool parenthesis = false;
	bool braces = false;

	if (t->type == TokenType::OPEN_PARENTHESIS) {
		parenthesis = true;
		eat();
	}

	w->condition = std::unique_ptr<Value>(eatExpression());

	if (parenthesis) {
		eat(TokenType::CLOSING_PARENTHESIS);
	}
	if (t->type == TokenType::OPEN_BRACE) {
		braces = true;
	} else {
		eat(TokenType::DO);
	}

	w->body = std::unique_ptr<Block>(eatBlock());

	if (!braces) {
		eat(TokenType::END);
	}

	return w;
}

Break* SyntaxicAnalyzer::eatBreak() {
	auto token = eat_get(TokenType::BREAK);
	Break* b = new Break();
	b->token = token;

	if (t->type == TokenType::NUMBER /*&& t->line == lt->line*/) {
		int deepness = std::stoi(t->content);
		if (deepness <= 0) {
			file->errors.push_back(Error(Error::Type::BREAK_LEVEL_ZERO, t, {}));
		} else {
			b->deepness = deepness;
			eat();
		}
	}

	return b;
}

Continue* SyntaxicAnalyzer::eatContinue() {
	eat(TokenType::CONTINUE);
	Continue* c = new Continue();

	if (t->type == TokenType::NUMBER /*&& t->line == lt->line*/) {
		int deepness = std::stoi(t->content);
		if (deepness <= 0) {
			file->errors.push_back(Error(Error::Type::CONTINUE_LEVEL_ZERO, t, {}));
		} else {
			c->deepness = deepness;
			eat();
		}
	}

	return c;
}

ClassDeclaration* SyntaxicAnalyzer::eatClassDeclaration() {

	eat(TokenType::CLASS);

	auto token = eatIdent();

	auto cd = new ClassDeclaration(token);
	eat(TokenType::OPEN_BRACE);

	while (t->type == TokenType::LET) {
		cd->fields.emplace_back(eatVariableDeclaration());
	}

	eat(TokenType::CLOSING_BRACE);

	return cd;
}

Token* SyntaxicAnalyzer::eatIdent() {
	return eat_get(TokenType::IDENT);
}

void SyntaxicAnalyzer::eat() {
	eat(TokenType::DONT_CARE);
}

void SyntaxicAnalyzer::eat(TokenType type) {
	eat_get(type);
}

Token* SyntaxicAnalyzer::eat_get() {
	return eat_get(TokenType::DONT_CARE);
}

Token* SyntaxicAnalyzer::eat_get(TokenType type) {

	auto eaten = t;

	last_character = t->location.start.column;
	last_line = t->location.start.line;
	last_size = t->size;
	if (i < file->tokens.size() - 1) {
		t = &file->tokens[++i];
	} else {
		t = new Token(TokenType::FINISHED, file, 0, 0, 0, "");
	}
	nt = i < file->tokens.size() - 1 ? &file->tokens[i + 1] : nullptr;

	if (type != TokenType::DONT_CARE && eaten->type != type) {
		file->errors.push_back(Error(Error::Type::UNEXPECTED_TOKEN, eaten, {eaten->content}));
		// std::cout << "unexpected token : " << to_string((int) type) << " != " << to_string((int) eaten->type) << " (" << eaten->content << ") char " << eaten->location.start.column << std::endl;
		return new Token(TokenType::FINISHED, file, 0, 0, 0, "");
	}
	return eaten;
}

Token* SyntaxicAnalyzer::nextTokenAt(int pos) {
	if (i + pos < file->tokens.size())
		return &file->tokens[i + pos];
	else
		return new Token(TokenType::FINISHED, file, 0, 0, 0, "");
}

}
