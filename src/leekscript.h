#ifndef LEEKSCRIPT_H__
#define LEEKSCRIPT_H__

#include "constants.h"

#if COMPILER
#include "vm/VM.hpp"
#include "vm/value/LSObject.hpp"
#include "vm/value/LSArray.hpp"
#endif
#include "environment/Environment.hpp"
#include "standard/Module.hpp"
#include "analyzer/Program.hpp"
#include "type/Integer_type.hpp"
#include "type/Object_type.hpp"
#include "analyzer/semantic/Callable.hpp"
#include "analyzer/resolver/File.hpp"

namespace ls {
	#define init() VM::static_init()
}

#endif
