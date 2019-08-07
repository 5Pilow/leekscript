<div align="center">
    <h1>
        <a href="https://leekscript.com">LeekScript language</a>
    </h1>
</div>

[![Build Status](https://travis-ci.org/leek-wars/leekscript.svg?branch=master)](https://travis-ci.org/leek-wars/leekscript)
[![Coverage Status](https://coveralls.io/repos/github/leek-wars/leekscript/badge.svg?branch=master)](https://coveralls.io/github/leek-wars/leekscript?branch=master)
[![GPL Licence](https://badges.frapsoft.com/os/gpl/gpl.svg?v=103)](https://opensource.org/licenses/GPL-3.0/)

LeekScript is a dynamically typed, compiled just-in-time programming language initially designed for [Leek Wars](https://leekwars.com) AIs, and for games in general. Design to be simple to use for beginners, but with rich syntax and functionnalities, and a good speed thanks to an optimized compiler.

## Contents
1. [Demonstration](#demonstration) - try it online
2. [Building](#building) - build instructions \
  2.1 [Shared library](#shared-library) - build and use \
  2.2 [WebAssembly](#webassembly) - build analyzer for the web
3. [Usage](#usage) - usage \
  3.1 [Command-line options](#command-line-options) - CLI options description
4. [Tests](#tests-coverage-valgrind-benchmark-doc) - run tests and more
5. [Projects using LeekScript](#projects-using-leekscript) - discover projects using LeekScript language
6. [Libraries](#libraries-used) - check library dependencies
7. [License](#license) - license information
---

## Demonstration

Play with online editor at https://leekscript.com/editor including syntax highlighting, smart completion and execution.

---

## Building
```
make
```
The executable `leekscript` is in the `build/` folder.

### Shared library
Use `make lib` to build `libleekscript.so` in `build/` folder. You can use `sudo make install` to copy it in `/usr/lib/` and make it available everywhere.

### WebAssembly analyzer
Run `make analyzer-web` to build the analyzer to target browsers in *WebAssembly*. Try it by running a small web server with `python tool/wasm_server.py` and browsing the link. Then check the **console** for the output result.

---

## Usage
After [building](#building), enter a LeekScript top-level (REPL):
```bash
leekscript
```
Execute a file or a code snippet:
```bash
leekscript my_file.leek
leekscript "[5, 6, 7] ~~ x -> x ** 2"
```

### Command-line options
The following command-line options are available when calling `leekscript` command. You can use multiple option by combining them (example: `leekscript -dijt`).

Option                              | Description
----------------------------------- | --------------------------------------------
`-b` \| `--bitcode`         | Output the program's bitcode file (LLVM's `.bc` file).
`-c` \| `--execute_bitcode` | Execute input as a bitcode file (LLVM's `.bc` file).
`-d` \| `--debug`           | Print debug information : types.
`-e` \| `--example`         | Output a simple one-liner example code.
`-f` \| `--format`          | Output the program nicely-formatted.
`-h` \| `--help`            | Display help.
`-i` \| `--intermediate`    | Output the program's intermediate representation (LLVM's `.ll` file).
`-j` \| `--json`	        | Get all the results in JSON format.
`-l` \| `--legacy`          | Use legacy mode (LeekScript 1.0): enable old functions, arrays and other behaviors.
`-o` \| `--operations`      | Enable operations counter and limit to 20 millions.
`-O<level>`                         | Optimization level.
`-r` \|  `--execute_ir`     | Execute input as an IR file (LLVM's `.ll` file).
`-t` \| `--time`	        | Print compilation and execution time and operations (if enabled).
`-v` \| `--version`         | Print the current version.

---

## Tests, coverage, Valgrind, benchmark, doc
```bash
make test
make coverage # test coverage by gcov + lcov
make valgrind # valgrind default tool
make callgrind # valgrind callgrind tool
make benchmark
make doc
```

---

## Projects using LeekScript
* [Leek Wars](https://leekwars.com), online programming game, for AI execution in [fight generation](https://github.com/leek-wars/leek-wars-generator).

## Libraries used
* [LLVM](https://llvm.org/) Version 8.0.0
* [GNU MP](https://gmplib.org/), by Torbjörn Granlund, under GPL license
* [JSON for modern C++](https://github.com/nlohmann/json), by Niels Lohmann, under MIT license
* [CLI11](https://github.com/CLIUtils/CLI11), by Henry Schreinern under NSF AWARD 1414736

## License
Distributed under the GPL3 license. Copyright (c) 2016-2019, Pierre Lauprêtre
