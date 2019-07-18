#ifndef VM_STANDARD_ARRAYSTD_HPP_
#define VM_STANDARD_ARRAYSTD_HPP_

#include "../Module.hpp"

namespace ls {

class ArraySTD : public Module {
public:
	ArraySTD(VM* vm);

	static Compiler::value in(Compiler& c, std::vector<Compiler::value> args, int);
	static Compiler::value op_add(Compiler&, std::vector<Compiler::value>, int);
	static Compiler::value array_add_eq(Compiler& c, std::vector<Compiler::value> args, int);

	static Compiler::value size(Compiler&, std::vector<Compiler::value>, int);

	static LSArray<LSValue*>* chunk_1_ptr(LSArray<LSValue*>* array);
	static LSArray<LSValue*>* chunk_1_int(LSArray<int>* array);
	static LSArray<LSValue*>* chunk_1_float(LSArray<double>* array);

	static LSValue* sub(LSArray<LSValue*>* array, int begin, int end);

	static Compiler::value fill(Compiler&, std::vector<Compiler::value>, int);

	static Compiler::value remove_element_any(Compiler&, std::vector<Compiler::value>, int);
	static Compiler::value remove_element_real(Compiler&, std::vector<Compiler::value>, int);
	static Compiler::value remove_element_int(Compiler&, std::vector<Compiler::value>, int);

	static Compiler::value partition(Compiler&, std::vector<Compiler::value>, int);
	static Compiler::value map(Compiler&, std::vector<Compiler::value>, int);
	static Compiler::value first(Compiler&, std::vector<Compiler::value>, int);
	static Compiler::value last(Compiler&, std::vector<Compiler::value>, int);
	static Compiler::value fold_left(Compiler&, std::vector<Compiler::value>, int);
	static Compiler::value fold_right(Compiler&, std::vector<Compiler::value>, int);
	static Compiler::value iter(Compiler&, std::vector<Compiler::value>, int);
	static Compiler::value sort(Compiler&, std::vector<Compiler::value>, int);
	static Compiler::value push(Compiler&, std::vector<Compiler::value>, int);
	static Compiler::value filter(Compiler&, std::vector<Compiler::value>, int);
	static Compiler::value push_all(Compiler&, std::vector<Compiler::value>, int);

	static int convert_key(LSValue*);
};

}

#endif
