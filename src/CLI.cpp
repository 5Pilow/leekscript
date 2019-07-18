#include "CLI.hpp"
#include <chrono>
#include <unistd.h>
#include <iostream>
#include "analyzer/Context.hpp"
#include "vm/LSValue.hpp"
#include "colors.h"
#include "constants.h"
#include "vm/VM.hpp"
#include "util/CLI11.hpp"

namespace CLI11 = CLI;

namespace ls {

void CLI::seed_random() {
	/** Seed random one for all */
	long ns = std::chrono::duration_cast<std::chrono::nanoseconds>(
		std::chrono::system_clock::now().time_since_epoch()
	).count();
	srand(ns);
}

void CLI::vm_init() {
	#if COMPILER
		VM::static_init();
	#endif
}

int CLI::start(int argc, char* argv[]) {

	CLI11::App app("~~~ LeekScript v2.0 ~~~");
	CLI_options options;
	app.allow_extras();
    app.add_flag("-d,--debug", options.debug, "Output debug information");
    app.add_flag("-v,--version", options.version, "Output version");
    app.add_flag("-j,--json", options.json_output, "Output in JSON format");
    app.add_flag("-t,--time", options.display_time, "Output time results");
    app.add_flag("-l,--legacy", options.legacy, "Use legacy (V1) mode");
    app.add_flag("-i,--intermediate", options.intermediate, "Output the code intermediate representation");
    app.add_flag("-e,--example", options.example, "Get an example snippet");
    app.add_flag("-o,--operations", options.operations, "Enable operations counter and limit");
    app.add_option("-O", options.optimization, "Optimization level");
    app.add_flag("-r,--execute_ir", options.execute_ir, "Execute as an IR file (.ll or .ir)");
    app.add_flag("-b,--execute_bitcode", options.execute_bitcode, "Execute as an bitcode file (.bc)");
    try {
        app.parse(argc, argv);
    } catch (const CLI11::ParseError& e) {
        return app.exit(e);
    }

	/** Generate the standard functions documentation */
	if (options.documentation) {
		ls::VM vm {};
		ls::Documentation().generate(&vm, std::cout);
		return 0;
	}

	/** Display version? */
	if (options.version) {
		std::cout << "LeekScript 2.0" << std::endl;
		return 0;
	}
	/** Output an example code */
	if (options.example) {
		auto codes = Util::read_file_lines("src/doc/examples.txt");
		std::cout << codes[rand() % codes.size()] << std::endl;
		return 0;
	}

	if (app.remaining().size()) {
		auto file_or_code = app.remaining().at(0);
		/** Input file or code snippet? */
		if (Util::is_file_name(file_or_code)) {
			return file(file_or_code, options);
		} else {
			return snippet(file_or_code, options);
		}
	} else {
		return repl(options);
	}
}

int CLI::snippet(std::string code, CLI_options options) {
	/** Execute **/
	ls::VM vm { options.legacy };
	OutputStringStream oss;
	if (options.json_output)
		vm.output = &oss;
	auto result = vm.execute(code, nullptr, "snippet", options.debug, options.operations, false, options.intermediate, options.optimization, options.execute_ir, options.execute_bitcode);
	vm.output = ls::VM::default_output;
	print_result(result, oss.str(), options.json_output, options.display_time, options.operations);
	return 0;
}

int CLI::file(std::string file, CLI_options options) {
	std::ifstream ifs(file.data());
	if (!ifs.good()) {
		std::cout << "[" << C_YELLOW << "warning" << END_COLOR << "] File '" << BOLD << file << END_STYLE << "' does not exist." << std::endl;
		return 0;
	}
	std::string code = std::string((std::istreambuf_iterator<char>(ifs)), (std::istreambuf_iterator<char>()));
	ifs.close();
	auto file_name = Util::file_short_name(file);
	ls::VM vm { options.legacy };
	OutputStringStream oss;
	if (options.json_output)
		vm.output = &oss;
	auto result = vm.execute(code, nullptr, file_name, options.debug, options.operations, false, options.intermediate, options.optimization, options.execute_ir, options.execute_bitcode);
	vm.output = ls::VM::default_output;
	print_result(result, oss.str(), options.json_output, options.display_time, options.operations);
	return 0;
}

int CLI::repl(CLI_options options) {
	/** Interactive console mode */
	std::cout << "~~~ LeekScript v2.0 ~~~" << std::endl;
	std::string code;
	ls::Context ctx;
	ls::VM vm(options.legacy);

	while (!std::cin.eof()) {
		// Get a instruction
		std::cout << ">> ";
		std::getline(std::cin, code);
		// Execute
		auto result = vm.execute(code, &ctx, "(top-level)", options.debug, options.operations, options.assembly, options.intermediate);
		print_result(result, "", options.json_output, options.display_time, options.operations);
		// std::cout << &ctx << std::endl;
	}
	return 0;
}

void CLI::print_result(ls::VM::Result& result, const std::string& output, bool json, bool display_time, bool ops) {
	if (json) {
		std::ostringstream oss;
		print_errors(result, oss, json);
		std::string res = oss.str() + result.value;
		res = Util::replace_all(res, "\"", "\\\"");
		res = Util::replace_all(res, "\n", "");
		std::cout << "{\"success\":true,\"ops\":" << result.operations
			<< ",\"time\":" << result.execution_time
			<< ",\"res\":\"" << res << "\""
			<< ",\"output\":" << Json(output)
			<< "}" << std::endl;
	} else {
		print_errors(result, std::cout, json);
		if (result.execution_success && result.value != "(void)") {
			std::cout << result.value << std::endl;
		}
		if (display_time) {
			std::cout << C_GREY << "(";
			if (ops) {
				std::cout << result.operations << " ops, ";
			}
			std::cout << result.parse_time << "ms + " << result.compilation_time << "ms + " << result.execution_time << "ms)" << END_COLOR << std::endl;
		}
	}
}

void CLI::print_errors(ls::VM::Result& result, std::ostream& os, bool json) {
	bool first = true;
	for (const auto& e : result.errors) {
		if (!first) std::cout << std::endl;
		os << C_RED << "❌ " << END_COLOR << e.message() << std::endl;
		os << "    " << BOLD << "> " << e.location.file->path << ":" << e.location.start.line << END_COLOR << ": " << e.underline_code << std::endl;
		first = false;
	}
	if (result.exception.type != ls::vm::Exception::NO_EXCEPTION) {
		os << result.exception.to_string(json ? false : true);
	}
}

}
