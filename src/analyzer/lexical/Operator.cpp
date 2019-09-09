#include "Operator.hpp"
#include <iostream>

namespace ls {

/*
 * Binary operators precedence
 * ------------------
 * 0| ~ ~~ ~= ~~=
 * ------------------
 * 1| ^
 * ------------------
 * 2| * / % \ %%
 * ------------------
 * 3| + -
 * ------------------
 * 4| < <= > >= is
 * ------------------
 * 5| == != === !===
 * ------------------
 * 6| && and
 * ------------------
 * 7| || or xor
 * ------------------
 * 8| ?? ?!
 * ------------------
 * 9| = += -= *= /= %= ^= <=> \=
 */

static int operator_priorities[] = {
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	5, /* != */
	9, /* = */
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	9, /* <=> */
	0,
	5, 5, 5, /* === == !== */
	3, 3, /* + - */
	2, 2, /* * / */
	0, 0,
	9, 9, 9, 9, /* += -= /= *= */
	0,
	4, 4, 4, 4, /* > >= < <= */
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	7, /* or */
	6, /* and */
	7, /* xor */
	0, 0,
	1, /* ^ */
	9, 9, /* ^= %= */
	2, /* % */
	0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, /* ~ ~~ ~= ~~= */
	0, 0, 0,
	4, /* is */
	0, 0, 0, 0, 0, 3, 0, 3, 0, 3, 0, 3, 0, 3, 0,
	8, /* ?? */
	2, /* \ */
	0, 0, 0,
	9, /* \= */
	8, /* ?! */
	2, /* %% */
	9, /* %%= */
};

Operator::Operator(Token* token) : token(token) {
	this->type = token->type;
	this->character = token->content;
	this->priority = operator_priorities[(int) token->type];

	this->reversed = type == TokenType::IN;
//	cout << "operator " << character << " (" << (int)token->type << ") : " << priority << endl;
}

void Operator::print(std::ostream& os) {
	os << character;
}

}
