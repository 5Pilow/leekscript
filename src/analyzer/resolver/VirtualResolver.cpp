#include "VirtualResolver.hpp"
#include <iostream>
#include "../../util/Util.hpp"
#include <unordered_map>
#include "../Program.hpp"
#include "../../colors.h"

namespace ls {

static std::unordered_map<std::string, File*> file_cache;

File* VirtualResolver::create(std::string path, Program* program) const {
	auto fspath = std::filesystem::path(path);
	return file_cache[path] = new File(path, program->code, FileContext(fspath.parent_path()), program);
}

File* VirtualResolver::resolve(std::string path, FileContext context) const {
	auto resolvedPath = (context.folder / path).lexically_normal();
	auto i = file_cache.find(resolvedPath);
	if (i != file_cache.end()) {
		return i->second;
	}
	// std::cout << "[" << C_YELLOW << "warning" << END_COLOR << "] Virtual file " << BOLD << resolvedPath << END_STYLE << " does not exist in context " << context.folder << std::endl;
	return nullptr;
}

}