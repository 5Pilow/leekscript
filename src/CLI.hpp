#ifndef CLI_HPP
#define CLI_HPP

#include <algorithm>
#include <fstream>
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
#include "util/Util.hpp"
#include "analyzer/resolver/File.hpp"
#include "util/CLI11.hpp"

namespace CLI11 = CLI;

namespace ls {

struct CLI_option {
	std::string letter;
	std::string command;
	std::string description;
};

struct CLI_options {
	bool legacy = false;		// L
	bool format = false;		// F
	bool debug = false;			// D
	bool json_output = false;	// J
	bool display_time = false;	// T
	bool operations = false;	// O
	bool bitcode = false;		// B
	bool version = false;		// V
	bool documentation = false;	// --documentation
	int optimization = 0;		// O
	bool intermediate = false;	// I
	bool example = false;		// E
	bool execute_ir = false;	// R --execute-ir
	bool execute_bitcode = false; // W --execute_bitcode
};

class CLI {
public:

	void seed_random();
	void vm_init();

	void setup_options(CLI11::App& app, CLI_options& options, int argc, char* argv[]);

	int start_analyzer(int argc, char* argv[]);
	int start_full(int argc, char* argv[]);

	int analyze_snippet(std::string, CLI_options options);
	int analyze_file(std::string, CLI_options options);
	int execute_snippet(std::string, CLI_options options);
	int execute_file(std::string, CLI_options options);
	int repl(CLI_options);

	void print_errors(ls::Result& result, std::ostream& os, bool json);
	void print_result(ls::Result& result, const std::string& output, bool json, bool display_time, bool ops);
	std::string read_file(std::string);
};

}

#endif