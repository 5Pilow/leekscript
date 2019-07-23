#include "CLI.hpp"

int main(int argc, char* argv[]) {
	ls::CLI cli;
	return cli.start_analyzer(argc, argv);
}