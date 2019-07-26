#ifndef JSON_STD_HPP
#define JSON_STD_HPP

#include "../Module.hpp"

namespace ls {

class JsonSTD : public Module {
public:
	JsonSTD(Environment& env);

	static Compiler::value encode(Compiler&, std::vector<Compiler::value>, int);
	static LSValue* decode(LSString* string);
};

}

#endif
