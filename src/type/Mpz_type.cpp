#include "Mpz_type.hpp"
#include "Type.hpp"
#include "../colors.h"
#include "Number_type.hpp"
#include "Any_type.hpp"
#include "Real_type.hpp"
#include "Mpz_type.hpp"
#include "Bool_type.hpp"
#include "Integer_type.hpp"
#include "Long_type.hpp"
#if COMPILER
#include "../compiler/Compiler.hpp"
#endif

namespace ls {

#if COMPILER
llvm::Type* Mpz_type::mpz_type = nullptr;
#endif

bool Mpz_type::operator == (const Type* type) const {
	return dynamic_cast<const Mpz_type*>(type) != nullptr;
}
int Mpz_type::distance(const Type* type) const {
	if (not temporary and type->temporary) return -1;
	if (dynamic_cast<const Mpz_type*>(type->folded)) { return 0; }
	if (dynamic_cast<const Number_type*>(type->folded)) { return 1; }
	if (dynamic_cast<const Any_type*>(type->folded)) { return 2; }
	if (dynamic_cast<const Real_type*>(type->folded)) { return 100; }
	if (dynamic_cast<const Long_type*>(type->folded)) { return 200; }
	if (dynamic_cast<const Integer_type*>(type->folded)) { return 300; }
	if (dynamic_cast<const Bool_type*>(type->folded)) { return 400; }
	return -1;
}
#if COMPILER
llvm::Type* Mpz_type::llvm(Compiler& c) const {
	return get_mpz_type(c);
}
#endif
std::string Mpz_type::class_name() const {
	return "Number";
}
std::ostream& Mpz_type::print(std::ostream& os) const {
	os << BLUE_BOLD << "mpz" << END_COLOR;
	return os;
}
Type* Mpz_type::clone() const {
	return new Mpz_type { env };
}

#if COMPILER
llvm::Type* Mpz_type::get_mpz_type(Compiler& c) {
	if (mpz_type == nullptr) {
		mpz_type = llvm::StructType::create({ llvm::Type::getInt128Ty(c.getContext()) }, "mpz");
	}
	return mpz_type;
}
#endif

}