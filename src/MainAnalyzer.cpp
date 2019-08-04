#include "CLI.hpp"
#include "environment/Environment.hpp"

#if WASM
#include <emscripten/emscripten.h>

extern "C" {
const char* EMSCRIPTEN_KEEPALIVE analyze(char* code) {
	ls::Environment env;
	ls::Program program { env, code, "snippet" };
	env.analyze(program);
	std::ostringstream oss;
	program.print(oss, true);
	return oss.str().c_str();
}
}
#else

int main(int argc, char* argv[]) {
	ls::CLI cli;
	return cli.start_analyzer(argc, argv);
}

#endif
