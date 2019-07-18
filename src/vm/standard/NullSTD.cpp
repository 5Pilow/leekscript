#include "NullSTD.hpp"
#include "../value/LSNull.hpp"

namespace ls {

NullSTD::NullSTD(VM* vm) : Module(vm, "Null") {
	#if COMPILER
	LSNull::clazz = lsclass.get();
	#endif
}

}
