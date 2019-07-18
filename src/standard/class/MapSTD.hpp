#ifndef VM_STANDARD_MAPSTD_HPP_
#define VM_STANDARD_MAPSTD_HPP_

#include "../Module.hpp"

namespace ls {

class MapSTD : public Module {
public:
	MapSTD(VM* vm);

	static Compiler::value look(Compiler&, std::vector<Compiler::value>, int);

	static Compiler::value fold_left(Compiler&, std::vector<Compiler::value>, int);
	static Compiler::value fold_right(Compiler&, std::vector<Compiler::value>, int);
	
	static Compiler::value iter(Compiler& c, std::vector<Compiler::value>, int);
};

}

#endif
