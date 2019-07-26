#include "NullSTD.hpp"
#include "../../vm/value/LSNull.hpp"

namespace ls {

NullSTD::NullSTD(Environment& env) : Module(env, "Null") {
	#if COMPILER
	LSNull::clazz = lsclass.get();
	#endif
}

}
