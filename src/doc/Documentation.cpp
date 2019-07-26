#include "Documentation.hpp"

#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include "../standard/class/ArraySTD.hpp"
#include "../standard/class/MapSTD.hpp"
#include "../standard/class/BooleanSTD.hpp"
#include "../standard/class/NumberSTD.hpp"
#include "../standard/class/ObjectSTD.hpp"
#include "../standard/class/StringSTD.hpp"
#include "../standard/class/IntervalSTD.hpp"
#include "../standard/class/SetSTD.hpp"
#include "../standard/class/SystemSTD.hpp"
#include "../standard/class/ClassSTD.hpp"
#include "../standard/class/FunctionSTD.hpp"
#include "../standard/class/JsonSTD.hpp"
#include "../standard/class/NullSTD.hpp"
#include "../standard/class/ValueSTD.hpp"
#include "../standard/StandardLibrary.hpp"
#include "../environment/Environment.hpp"

namespace ls {

Documentation::Documentation() {}

Documentation::~Documentation() {}

void Documentation::generate(std::ostream& os, std::string lang) {

	Environment env;
	StandardLibrary stdLib { env };

	os << "{";

	int m = 0;
	for (const auto& clazz : stdLib.classes) {
		if (m > 0) os << ",";

		std::string file = "src/doc/" + clazz.second->name + "_" + lang + ".json";

		clazz.second->generate_doc(os, file);
		m++;
	}

	os << "}\n";
}

}
