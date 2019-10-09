#ifndef VM_STANDARD_MAPSTD_HPP_
#define VM_STANDARD_MAPSTD_HPP_

#include "../Module.hpp"

namespace ls {

class MapSTD : public Module {
public:
	MapSTD(Environment& env);

	#if COMPILER

	static Compiler::value in(Compiler&, std::vector<Compiler::value>, int);

	static Compiler::value look(Compiler&, std::vector<Compiler::value>, int);
	static Compiler::value insert(Compiler& c, std::vector<Compiler::value> args, int);
	static Compiler::value clear(Compiler& c, std::vector<Compiler::value> args, int);
	static Compiler::value fold_left(Compiler&, std::vector<Compiler::value>, int);
	static Compiler::value fold_right(Compiler&, std::vector<Compiler::value>, int);
	static Compiler::value iter(Compiler& c, std::vector<Compiler::value>, int);
	static Compiler::value values(Compiler& c, std::vector<Compiler::value>, int);
	static Compiler::value min(Compiler& c, std::vector<Compiler::value>, int);
	static Compiler::value min_key(Compiler& c, std::vector<Compiler::value>, int);
	static Compiler::value max(Compiler& c, std::vector<Compiler::value>, int);
	static Compiler::value max_key(Compiler& c, std::vector<Compiler::value>, int);
	static Compiler::value erase(Compiler& c, std::vector<Compiler::value>, int);

	#endif
};

}

#endif
