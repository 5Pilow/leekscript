#include "Ident.hpp"
#include "../../vm/value/LSNull.hpp"

namespace ls {

Ident::Ident(Token* token) {
	this->token = token;
}

Ident::~Ident() {
	// Do not own the token
}

}
