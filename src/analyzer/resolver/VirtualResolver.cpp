#include "VirtualResolver.hpp"
#include <iostream>
#include "../../util/Util.hpp"
#include <unordered_map>
#include "../Program.hpp"
#include "../../colors.h"

namespace ls {

static std::unordered_map<std::string, std::unique_ptr<File>> file_cache;

File* VirtualResolver::create(std::string path, Program* program) const {
	auto fspath = std::filesystem::path(path);
	auto code = program ? program->code : "";
	return file_cache.emplace(path, new File(path, code, FileContext(fspath.parent_path()), program)).first->second.get();
}

File* VirtualResolver::delete_(std::string path) const {
	auto fspath = std::filesystem::path(path);
	auto file = file_cache[path].release();
	file_cache.erase(path);
	return file;
}

File* VirtualResolver::resolve(std::string path, FileContext context) const {
	auto resolvedPath = (context.folder / path).lexically_normal();
	auto i = file_cache.find(resolvedPath);
	if (i != file_cache.end()) {
		return i->second.get();
	}
	// std::cout << "[" << C_YELLOW << "warning" << END_COLOR << "] Virtual file " << BOLD << resolvedPath << END_STYLE << " does not exist in context " << context.folder << std::endl;
	return nullptr;
}

std::unordered_map<std::string, std::unique_ptr<File>>& VirtualResolver::get_cache() {
	return file_cache;
}

}