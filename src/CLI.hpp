#ifndef CLI_HPP
#define CLI_HPP

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
#include "analyzer/resolver/File.hpp"

namespace ls {

struct CLI_option {
	std::string letter;
	std::string command;
	std::string description;
};

struct CLI_options {
	bool legacy = false;		// L
	bool debug = false;			// D
	bool json_output = false;	// J
	bool display_time = false;	// T
	bool operations = false;	// O
	bool assembly = false;		//
	bool version = false;		// V
	bool documentation = false;	// --documentation
	int optimization = 0;		// O
	bool intermediate = false;	// I
	bool example = false;		// E
	bool execute_ir = false;	// R --execute-ir
	bool execute_bitcode = false; // B --execute_bitcode
};

class CLI {
public:

	void seed_random();
	void vm_init();

	int start(int argc, char* argv[]);

	int snippet(std::string, CLI_options options);
	int file(std::string, CLI_options options);
	int repl(CLI_options);

	void print_errors(ls::VM::Result& result, std::ostream& os, bool json);
	void print_result(ls::VM::Result& result, const std::string& output, bool json, bool display_time, bool ops);
};

}

#endif