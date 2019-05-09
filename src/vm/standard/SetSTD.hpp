#ifndef VM_STANDARD_SETSTD_HPP_
#define VM_STANDARD_SETSTD_HPP_

#include "../Module.hpp"

namespace ls {

class SetSTD : public Module {
public:
	SetSTD();

	static Compiler::value in_any(Compiler& c, std::vector<Compiler::value> args, bool);
	static Compiler::value set_add_eq(Compiler& c, std::vector<Compiler::value> args, bool);

	static Compiler::value insert_any(Compiler& c, std::vector<Compiler::value> args, bool);
	static Compiler::value insert_real(Compiler& c, std::vector<Compiler::value> args, bool);
	static Compiler::value insert_int(Compiler& c, std::vector<Compiler::value> args, bool);
};

}

#endif
