#include "Resolver.hpp"
#include <iostream>
#include "../../util/Util.hpp"

namespace ls {

File* Resolver::resolve(std::string path, FileContext* context) const {
	if (context == nullptr) {
		context = new FileContext(".");
	}
	auto resolvedPath = (context->folder / path).lexically_normal();
	auto code = Util::read_file(resolvedPath);
	auto newContext = new FileContext(resolvedPath.parent_path());
	return new File(path, code, newContext);
}

}