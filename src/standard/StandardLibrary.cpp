#include "StandardLibrary.hpp"
#include "class/ValueSTD.hpp"
#include "class/NullSTD.hpp"
#include "class/NumberSTD.hpp"
#include "class/BooleanSTD.hpp"
#include "class/StringSTD.hpp"
#include "class/ArraySTD.hpp"
#include "class/MapSTD.hpp"
#include "class/SetSTD.hpp"
#include "class/ObjectSTD.hpp"
#include "class/SystemSTD.hpp"
#include "class/FunctionSTD.hpp"
#include "class/ClassSTD.hpp"
#include "class/IntervalSTD.hpp"
#include "class/JsonSTD.hpp"

namespace ls {

StandardLibrary::StandardLibrary(Environment& env, bool legacy) : env(env), legacy(legacy) {
	add_class(std::make_unique<ValueSTD>(env));
	add_class(std::make_unique<NullSTD>(env));
	add_class(std::make_unique<BooleanSTD>(env));
	add_class(std::make_unique<NumberSTD>(env));
	add_class(std::make_unique<StringSTD>(env));
	add_class(std::make_unique<ArraySTD>(env));
	add_class(std::make_unique<MapSTD>(env));
	add_class(std::make_unique<FunctionSTD>(env));
	add_class(std::make_unique<ClassSTD>(env));
	add_class(std::make_unique<SystemSTD>(env));
	add_class(std::make_unique<JsonSTD>(env));
	if (not legacy) {
		add_class(std::make_unique<SetSTD>(env));
		add_class(std::make_unique<ObjectSTD>(env));
		add_class(std::make_unique<IntervalSTD>(env));
	}
}

void StandardLibrary::add_class(std::unique_ptr<Module> m) {
	classes.insert({ m->name, std::move(m) });
}

}