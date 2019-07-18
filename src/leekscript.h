#ifndef LEEKSCRIPT_H__
#define LEEKSCRIPT_H__

#include "vm/VM.hpp"
#include "vm/Module.hpp"
#include "vm/value/LSNumber.hpp"
#include "vm/value/LSArray.hpp"
#include "vm/value/LSObject.hpp"
#include "vm/Program.hpp"
#include "type/Integer_type.hpp"
#include "type/Object_type.hpp"
#include "analyzer/semantic/Callable.hpp"
#include "analyzer/resolver/File.hpp"

namespace ls {
	#define init() VM::static_init()
}

#endif
