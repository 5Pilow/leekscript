#include "Ident.hpp"

namespace ls {

Ident::Ident(Token* token) {
	this->token = token;
}

Ident::~Ident() {
	// Do not own the token
}

}
