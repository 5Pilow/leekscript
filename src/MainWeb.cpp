#include "CLI.hpp"

int main(int argc, char* argv[]) {
	ls::CLI cli;
	cli.seed_random();
	return cli.start();
}