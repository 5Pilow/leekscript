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

namespace ls {

Documentation::Documentation() {}

Documentation::~Documentation() {}

void Documentation::generate(VM* vm, std::ostream& os, std::string lang) {

	std::vector<Module*> modules;

	modules.push_back(new NullSTD(vm));
	modules.push_back(new BooleanSTD(vm));
	modules.push_back(new NumberSTD(vm));
	modules.push_back(new ArraySTD(vm));
	modules.push_back(new MapSTD(vm));
	modules.push_back(new ObjectSTD(vm));
	modules.push_back(new StringSTD(vm));
	modules.push_back(new IntervalSTD(vm));
	modules.push_back(new SetSTD(vm));
	modules.push_back(new SystemSTD(vm));
	modules.push_back(new ClassSTD(vm));
	modules.push_back(new FunctionSTD(vm));
	modules.push_back(new JsonSTD(vm));
	modules.push_back(new ValueSTD(vm));

	os << "{";

	for (unsigned m = 0; m < modules.size(); ++m) {
		if (m > 0) os << ",";

		Module* mod = modules[m];
		std::string file = "src/doc/" + mod->name + "_" + lang + ".json";

		mod->generate_doc(os, file);
	}

	os << "}\n";

	for (const auto& m : modules) {
		delete m;
	}
}

}
