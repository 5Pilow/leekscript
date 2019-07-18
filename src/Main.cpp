#include "CLI.hpp"

int main(int argc, char* argv[]) {
	ls::CLI cli;
	cli.seed_random();
	cli.vm_init();
	return cli.start(argc, argv);
}
