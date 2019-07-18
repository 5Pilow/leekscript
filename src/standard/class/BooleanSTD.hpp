#ifndef BOOLEAN_STD_HPP
#define BOOLEAN_STD_HPP

#include "../Module.hpp"

namespace ls {

class BooleanSTD : public Module {
public:
	BooleanSTD(VM* vm);

	static LSString* add(int boolean, LSString* string);
	static LSString* add_tmp(int boolean, LSString* string);
	static Compiler::value add_bool(Compiler& c, std::vector<Compiler::value> args, int);

	static Compiler::value sub_bool(Compiler& c, std::vector<Compiler::value> args, int);

	static Compiler::value mul_bool(Compiler& c, std::vector<Compiler::value> args, int);

	static int compare_ptr_ptr(LSBoolean* a, LSBoolean* b);
	static LSValue* compare_ptr_ptr_ptr(LSBoolean* a, LSBoolean* b);
	static Compiler::value compare_val_val(Compiler&, std::vector<Compiler::value>, int);

	static LSValue* to_string(bool b);
};

}

#endif
