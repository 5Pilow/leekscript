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

StandardLibrary::StandardLibrary(bool legacy) : legacy(legacy) {
	add_class(std::make_unique<ValueSTD>(this));
	add_class(std::make_unique<NullSTD>(this));
	add_class(std::make_unique<BooleanSTD>(this));
	add_class(std::make_unique<NumberSTD>(this));
	add_class(std::make_unique<StringSTD>(this));
	add_class(std::make_unique<ArraySTD>(this));
	add_class(std::make_unique<MapSTD>(this));
	add_class(std::make_unique<SetSTD>(this));
	add_class(std::make_unique<ObjectSTD>(this));
	add_class(std::make_unique<FunctionSTD>(this));
	add_class(std::make_unique<ClassSTD>(this));
	add_class(std::make_unique<SystemSTD>(this));
	add_class(std::make_unique<IntervalSTD>(this));
	add_class(std::make_unique<JsonSTD>(this));
}

void StandardLibrary::add_class(std::unique_ptr<Module> m) {
	classes.insert({ m->name, std::move(m) });
}

}