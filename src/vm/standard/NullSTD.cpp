#include "NullSTD.hpp"
#include "../value/LSNull.hpp"

namespace ls {

NullSTD::NullSTD(VM* vm) : Module(vm, "Null") {
	LSNull::clazz = lsclass.get();
}

}
