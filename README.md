<div align="center">
    <h1>
        <a href="https://leekscript.com">LeekScript language</a>
    </h1>
</div>

[![Build Status](https://travis-ci.org/leek-wars/leekscript.svg?branch=master)](https://travis-ci.org/leek-wars/leekscript)
[![Coverage Status](https://coveralls.io/repos/github/leek-wars/leekscript/badge.svg?branch=master)](https://coveralls.io/github/leek-wars/leekscript?branch=master)
[![GPL Licence](https://badges.frapsoft.com/os/gpl/gpl.svg?v=103)](https://opensource.org/licenses/GPL-3.0/)

LeekScript is a dynamically typed, compiled just-in-time programming language initially designed for [Leek Wars](https://leekwars.com) AIs, and for games in general. Design to be simple to use for beginners, but with rich syntax and functionnalities, and a good speed thanks to an optimized compiler.

---

## Demonstration

Play with online editor at https://leekscript.com/editor including syntax highlighting, smart completion and execution.

## Building
```
make
```
The executable `leekscript` is in the `build/` folder.

Export as shared library
```
make lib
```
`libleekscript.so` will be in `build/` folder. You can `make install` to copy it in `/usr/lib/`.

## Usage
Run a LeekScript top-level
```
leekscript
```
Execute a file or a code snippet
```
leekscript my_file.leek
leekscript "[5, 6, 7] ~~ x -> x ** 2"
```
Option | Effect
------ | ------
-v -V --version	| Print the current version
-e -E --example | Output an example code
-j -J --json	| Get the result in JSON format
-t -T --time	| Print execution time and operations
-d -D --debug | Print debug information like types
-n -N --no-operations | Disable operations counting
-a -A --assembly  | Print assembly of LeekScript code
-p -P --pseudo-code  | Print JIT pseudo code
-l -L --legacy | Use legacy mode  (LeekScript v1)

## Tests, coverage, Valgrind, benchmark, doc
```shell
make test
make coverage # test coverage by gcov + lcov
make valgrind # valgrind default tool
make callgrind # valgrind callgrind tool
make benchmark
make doc
```

## WebAssembly build
Run `make analyzer-web` to build the analyzer to target browsers in *WebAssembly*. Try it by running a small web server with `python tool/wasm_server.py` and browsing the link. Then check the console for the output result.

## Libraries used
* [LLVM](https://llvm.org/) Version 8.0.0
* [GNU MP](https://gmplib.org/), by Torbjörn Granlund, under GPL license
* [JSON for modern C++](https://github.com/nlohmann/json), by Niels Lohmann, under MIT license

## License
Distributed under the GPL3 license. Copyright (c) 2016-2019, Pierre Lauprêtre
