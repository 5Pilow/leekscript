#include "Long_type.hpp"
#include "../colors.h"
#include <iostream>
#include "../compiler/Compiler.hpp"
#include "Number_type.hpp"
#include "Any_type.hpp"
#include "Real_type.hpp"
#include "Mpz_type.hpp"
#include "Bool_type.hpp"
#include "Integer_type.hpp"

namespace ls {

const Type* Long_type::key() const {
	return Type::integer;
}
const Type* Long_type::element() const {
	return Type::integer;
}
const Type* Long_type::iterator() const {
	return Type::structure("long_iterator", {
		Type::long_,
		Type::long_,
		Type::integer
	});
}
bool Long_type::operator == (const Type* type) const {
	return dynamic_cast<const Long_type*>(type) != nullptr;
}
int Long_type::distance(const Type* type) const {
	if (not temporary and type->temporary) return -1;
	if (dynamic_cast<const Long_type*>(type->folded)) { return 0; }
	if (dynamic_cast<const Real_type*>(type->folded)) { return 1; }
	if (dynamic_cast<const Mpz_type*>(type->folded)) { return 4; }
	if (dynamic_cast<const Number_type*>(type->folded)) { return 5; }
	if (dynamic_cast<const Any_type*>(type->folded)) { return 6; }
	if (dynamic_cast<const Integer_type*>(type->folded)) { return 100; }
	if (dynamic_cast<const Bool_type*>(type->folded)) { return 101; }
	return -1;
}
#if COMPILER
llvm::Type* Long_type::llvm(const Compiler& c) const {
	return llvm::Type::getInt64Ty(c.getContext());
}
#endif
std::string Long_type::class_name() const {
	return "Number";
}
std::ostream& Long_type::print(std::ostream& os) const {
	os << BLUE_BOLD << "long" << END_COLOR;
	return os;
}
Type* Long_type::clone() const {
	return new Long_type {};
}

}