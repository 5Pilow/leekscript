#include <algorithm>
#include <fstream>
#include <gmp.h>
#include <gmpxx.h>
#include <iostream>
#include <iterator>
#include <math.h>
#include <unistd.h>
#include <string>
#include <chrono>
#include "leekscript.h"
#include "constants.h"
#include "colors.h"
#include "util/utf8.h"
#include "doc/Documentation.hpp"
#include "../benchmark/Benchmark.hpp"
#include "vm/LSValue.hpp"
#include "util/Util.hpp"
#include "../test/Test.hpp"
#include "analyzer/Context.hpp"
#include "analyzer/resolver/File.hpp"

void print_errors(ls::VM::Result& result, std::ostream& os, bool json);
void print_result(ls::VM::Result& result, const std::string& output, bool json, bool display_time, bool ops);

int main(int argc, char* argv[]) {

	ls::init();

	/** Seed random one for all */
	long ns = std::chrono::duration_cast<std::chrono::nanoseconds>(
		std::chrono::system_clock::now().time_since_epoch()
	).count();
	srand(ns);

	/** Generate the standard functions documentation */
	if (argc > 1 && std::string(argv[1]) == "-doc") {
		ls::VM vm {};
		ls::Documentation().generate(&vm, std::cout);
		return 0;
	}

	/** Arguments */
	bool output_json = false;
	bool display_time = false;
	bool print_version = false;
	bool debug_mode = false;
	bool v1 = false;
	bool ops = true;
	bool example = false;
	bool assembly = false;
	bool pseudo_code = false;
	bool optimized_ir = false;
	bool execute_ir = false;
	bool execute_bitcode = false;
	std::string file_or_code;

	for (int i = 1; i < argc; ++i) {
		auto a = std::string(argv[i]);
		if (a == "-j" or a == "-J" or a == "--json") output_json = true;
		else if (a == "-t" or a == "-T" or a == "--time") display_time = true;
		else if (a == "-v" or a == "-V" or a == "--version") print_version = true;
		else if (a == "-d" or a == "-D" or a == "--debug") debug_mode = true;
		else if (a == "-l" or a == "-L" or a == "--legacy") v1 = true;
		else if (a == "-n" or a == "-N" or a == "--no-operations") ops = false;
		else if (a == "-e" or a == "-E" or a == "--example") example = true;
		else if (a == "-a" or a == "-A" or a == "--assembly") assembly = true;
		else if (a == "-p" or a == "-P" or a == "--pseudo-code") pseudo_code = true;
		else if (a == "-o" or a == "-O" or a == "--optimized-ir") optimized_ir = true;
		else if (a == "-ir" or a == "-IR") execute_ir = true;
		else if (a == "-bc" or a == "-BC") execute_bitcode = true;
		else file_or_code = a;
	}

	/** Display version? */
	if (print_version) {
		std::cout << "LeekScript 2.0" << std::endl;
		return 0;
	}
	/** Output an example code */
	if (example) {
		auto codes = Util::read_file_lines("src/doc/examples.txt");
		std::cout << codes[rand() % codes.size()] << std::endl;
		return 0;
	}

	/** Input file or code snippet? */
	if (file_or_code.size() > 0) {
		/** Get the code **/
		std::string code;
		std::string file_name;
		if (Util::is_file_name(file_or_code)) {
			std::ifstream ifs(file_or_code.data());
			if (!ifs.good()) {
				std::cout << "[" << C_YELLOW << "warning" << END_COLOR << "] File '" << BOLD << file_or_code << END_STYLE << "' does not exist." << std::endl;
				return 0;
			}
			code = std::string((std::istreambuf_iterator<char>(ifs)), (std::istreambuf_iterator<char>()));
			ifs.close();
			file_name = Util::file_short_name(file_or_code);
		} else {
			code = file_or_code;
			file_name = "snippet";
		}
		/** Execute **/
		ls::VM vm {v1};
		OutputStringStream oss;
		if (output_json)
			vm.output = &oss;
		auto result = vm.execute(code, nullptr, file_name, debug_mode, ops, assembly, pseudo_code, optimized_ir, execute_ir, execute_bitcode);
		vm.output = ls::VM::default_output;
		print_result(result, oss.str(), output_json, display_time, ops);
		return 0;
	}

	/** Interactive console mode */
	std::cout << "~~~ LeekScript v2.0 ~~~" << std::endl;
	std::string code;
	ls::Context ctx;
	ls::VM vm(v1);

	while (!std::cin.eof()) {
		// Get a instruction
		std::cout << ">> ";
		std::getline(std::cin, code);
		// Execute
		auto result = vm.execute(code, &ctx, "(top-level)", debug_mode, ops, assembly, pseudo_code);
		print_result(result, "", output_json, display_time, ops);
		// std::cout << &ctx << std::endl;
	}
	return 0;
}

void print_result(ls::VM::Result& result, const std::string& output, bool json, bool display_time, bool ops) {
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

void print_errors(ls::VM::Result& result, std::ostream& os, bool json) {
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
