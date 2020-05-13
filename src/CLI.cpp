#include "CLI.hpp"
#include <chrono>
#include <unistd.h>
#include <iostream>
#include "analyzer/Context.hpp"
#include "vm/LSValue.hpp"
#include "colors.h"
#include "constants.h"
#include "util/CLI11.hpp"
#include "analyzer/resolver/Resolver.hpp"
#include "analyzer/syntaxic/SyntaxicAnalyzer.hpp"
#include "analyzer/semantic/SemanticAnalyzer.hpp"
#include "standard/StandardLibrary.hpp"
#include "environment/Environment.hpp"

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

void CLI::setup_options(CLI11::App& app, CLI_options& options, int argc, char* argv[]) {
	app.name("~~~ LeekScript v2.0 ~~~");
	app.allow_extras();
    app.add_flag("-f,--format", options.format, "Format code nicely");
    app.add_flag("-d,--debug", options.debug, "Output debug information");
    app.add_flag("-v,--version", options.version, "Output version");
    app.add_flag("-j,--json", options.json_output, "Output in JSON format");
    app.add_flag("-t,--time", options.display_time, "Output time results");
    app.add_flag("-l,--legacy", options.legacy, "Use legacy (V1) mode");
    app.add_flag("-i,--intermediate", options.intermediate, "Output the code intermediate representation");
    app.add_flag("-b,--bitcode", options.bitcode, "Output the code bitcode file");
    app.add_flag("-e,--example", options.example, "Get an example snippet");
    app.add_flag("-o,--operations", options.operations, "Enable operations counter and limit");
    app.add_option("-O", options.optimization, "Optimization level");
    app.add_flag("-r,--execute_ir", options.execute_ir, "Execute as an IR file (.ll or .ir)");
    app.add_flag("-c,--execute_bitcode", options.execute_bitcode, "Execute as an bitcode file (.bc)");
	app.add_flag("--documentation", options.documentation, "Generate and output the documentation as JSON");
	app.add_flag("-s,--sections", options.sections, "Output sections colors");
    try {
        app.parse(argc, argv);
    } catch (const CLI11::ParseError& e) {
		app.exit(e);
    }
}

int CLI::start_analyzer(int argc, char* argv[]) {

	CLI11::App app;
	CLI_options options;
	setup_options(app, options, argc, argv);

	if (not app.remaining().size()) {
		std::cout << "Input code|file missing" << std::endl;
		return 0;
	}
	auto file_or_code = app.remaining().at(0);
	/** Input file or code snippet? */
	if (Util::is_file_name(file_or_code)) {
		return analyze_file(file_or_code, options);
	} else {
		return analyze_snippet(file_or_code, options);
	}
}

int CLI::start_full(int argc, char* argv[]) {

	CLI11::App app;
	CLI_options options;
	setup_options(app, options, argc, argv);

	/** Generate the standard functions documentation */
	if (options.documentation) {
		ls::Documentation().generate(std::cout);
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
			return execute_file(file_or_code, options);
		} else {
			return execute_snippet(file_or_code, options);
		}
	} else {
		return repl(options);
	}
}

int CLI::analyze_snippet(std::string code, CLI_options options) {
	ls::Environment env;
	ls::Program program { env, code, "snippet" };
	env.analyze(program, options.format, options.debug);
	std::ostringstream oss;
	program.print(oss, true);
	print_result(program.result, oss.str(), options.json_output, options.display_time, options.operations);
	return 0;
}

int CLI::analyze_file(std::string file, CLI_options options) {
	auto code = ls::Util::read_file(file);
	ls::Environment env;
	ls::Program program { env, code, "snippet" };
	env.analyze(program, options.format, options.debug);
	std::ostringstream oss;
	program.print(oss, true);
	print_result(program.result, oss.str(), options.json_output, options.display_time, options.operations);
	return 0;
}

int CLI::execute_snippet(std::string code, CLI_options options) {
	#if COMPILER
	ls::Environment env { options.legacy };
	ls::Program program { env, code, "snippet" };

	OutputStringStream oss;
	if (options.json_output) {
		env.output = &oss;
	}
	if (not options.execute_ir) {
		env.analyze(program, options.format, options.debug, options.sections);
	}
	env.compile(program, options.format, options.debug, options.operations, false, options.intermediate, options.optimization, options.execute_ir, options.execute_bitcode);
	if (not options.execute_ir) {
		env.execute(program, options.debug, options.operations, false, options.intermediate, options.optimization, options.execute_ir, options.execute_bitcode);
	}

	print_result(program.result, oss.str(), options.json_output, options.display_time, options.operations);
	#endif
	return 0;
}

int CLI::execute_file(std::string file, CLI_options options) {
	#if COMPILER
	auto code = ls::Util::read_file(file);
	auto file_name = Util::file_short_name(file);
	ls::Environment env { options.legacy };
	OutputStringStream oss;
	if (options.json_output)
		env.output = &oss;
	Program program { env, code, file_name };

	if (not options.execute_ir) {
		env.analyze(program, options.format, options.debug, options.sections);
	}
	env.compile(program, options.format, options.debug, options.operations, false, options.intermediate, options.optimization, options.execute_ir, options.execute_bitcode);

	env.execute(program, options.format, options.debug, options.operations, false, options.intermediate, options.optimization, options.execute_ir, options.execute_bitcode);

	print_result(program.result, oss.str(), options.json_output, options.display_time, options.operations);
	#endif
	return 0;
}

int CLI::repl(CLI_options options) {
	/** Interactive console mode */
	#if COMPILER
	std::cout << "~~~ LeekScript v2.0 ~~~" << std::endl;
	std::string code;
	ls::Environment env { options.legacy };
	ls::Context ctx { env };

	while (!std::cin.eof()) {
		// Get a instruction
		std::cout << ">> ";
		std::getline(std::cin, code);
		// Execute
		Program program { env, code, "(top-level)" };
		program.context = &ctx;
		env.analyze(program, options.format, options.debug, options.sections);
		env.compile(program, options.format, options.debug, options.operations, false, options.intermediate, options.optimization, options.execute_ir, options.execute_bitcode);
		env.execute(program, options.debug, options.operations, options.bitcode, options.intermediate);
		print_result(program.result, "", options.json_output, options.display_time, options.operations);
		// std::cout << &ctx << std::endl;
	}
	#endif
	return 0;
}

void CLI::print_result(ls::Result& result, const std::string& output, bool json, bool display_time, bool ops) {
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

void CLI::print_errors(ls::Result& result, std::ostream& os, bool json) {
	bool first = true;
	for (const auto& e : result.errors) {
		if (!first) std::cout << std::endl;
		os << C_RED << "❌ " << END_COLOR << e.message() << std::endl;
		os << "    " << BOLD << "> " << e.location.file->path << ":" << e.location.start.line << END_COLOR << ": " << e.underline_code << std::endl;
		first = false;
	}
	#if COMPILER
	if (result.exception.type != ls::vm::Exception::NO_EXCEPTION) {
		os << result.exception.to_string(json ? false : true);
	}
	#endif
}

}
