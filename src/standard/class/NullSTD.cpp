#include "NullSTD.hpp"
#include "../../vm/value/LSNull.hpp"

namespace ls {

NullSTD::NullSTD(StandardLibrary* stdLib) : Module(stdLib, "Null") {
	#if COMPILER
	LSNull::clazz = lsclass.get();
	#endif
}

}
