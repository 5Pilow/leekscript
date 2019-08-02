#include "NullSTD.hpp"
#include "../../environment/Environment.hpp"

namespace ls {

NullSTD::NullSTD(Environment& env) : Module(env, "Null") {
	#if COMPILER
	env.null_class = std::make_unique<LSClass>(clazz.get());
	lsclass = env.null_class.get();
	#endif
}

}
