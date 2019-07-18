SRC_DIR := src/vm src/vm/value src/vm/standard src/doc \
src/analyzer src/analyzer/lexical src/analyzer/syntaxic src/analyzer/semantic src/analyzer/resolver src/analyzer/error src/analyzer/value src/analyzer/instruction \
src/compiler src/standard src/standard/class src/type src/util
WEB_DIR := src/analyzer src/analyzer/lexical src/analyzer/syntaxic src/analyzer/semantic src/analyzer/resolver src/analyzer/error \
src/analyzer/value src/analyzer/instruction src/standard src/standard/class src/type src/util
TEST_DIR := test

SRC := $(foreach d,$(SRC_DIR),$(wildcard $(d)/*.cpp))
TEST_SRC := $(foreach d,$(TEST_DIR),$(wildcard $(d)/*.cpp))
SRC_WEB := $(foreach d,$(WEB_DIR),$(wildcard $(d)/*.cpp))

BUILD_DIR := $(addprefix build/default/,$(SRC_DIR))
BUILD_DIR += $(addprefix build/default/,$(TEST_DIR))
BUILD_DIR += $(addprefix build/shared/,$(SRC_DIR))
BUILD_DIR += $(addprefix build/coverage/,$(SRC_DIR))
BUILD_DIR += $(addprefix build/profile/,$(SRC_DIR))
BUILD_DIR += $(addprefix build/sanitized/,$(SRC_DIR))
BUILD_DIR += $(addprefix build/deps/,$(SRC_DIR))
BUILD_DIR += $(addprefix build/deps/,$(TEST_DIR))
BUILD_DIR += $(addprefix build/web/,$(SRC_WEB))

OBJ := $(patsubst %.cpp,build/default/%.o,$(SRC))
DEPS := $(patsubst %.cpp,build/deps/%.d,$(SRC))

OBJ_TOPLEVEL = build/default/src/TopLevel.o
OBJ_BENCHMARK = build/benchmark/Benchmark.o
OBJ_TEST := $(patsubst %.cpp,build/default/%.o,$(TEST_SRC))
OBJ_LIB := $(patsubst %.cpp,build/shared/%.o,$(SRC))
OBJ_COVERAGE := $(patsubst %.cpp,build/coverage/%.o,$(SRC))
OBJ_PROFILE := $(patsubst %.cpp,build/profile/%.o,$(SRC))
OBJ_SANITIZED := $(patsubst %.cpp,build/sanitized/%.o,$(SRC))
OBJ_WEB := $(patsubst %.cpp,build/web/%.o,$(SRC_WEB))
OBJ_WEB += build/web/src/MainWeb.o

COMPILER := g++
OPTIM := -O0 -Wall
DEBUG := -g3
FLAGS := -std=c++17 -Wno-pmf-conversions
SANITIZE_FLAGS := -fsanitize=address -fno-omit-frame-pointer -fsanitize=undefined -fsanitize=float-divide-by-zero # -fsanitize=float-cast-overflow
LIBS := -lm -lgmp -lstdc++fs `llvm-config --cxxflags --ldflags --system-libs --libs core orcjit native`
MAKEFLAGS += --jobs=$(shell nproc)

CLOC_EXCLUDED := .git,lib,build,doxygen

.PHONY: test

all: FLAGS += -DDEBUG_LEAKS
all: build/leekscript

clang: COMPILER=clang++
clang: all

ninja:
	@mkdir -p build/default
	gyp leekscript.gyp --depth=. -f ninja -Goutput_dir=build --generator-output default
	ninja -v -C build/default leekscript-test
	build/default/leekscript-test

# Main build task, default build
build/leekscript: $(BUILD_DIR) $(OBJ) $(OBJ_TOPLEVEL)
	$(COMPILER) $(FLAGS) -o build/leekscript $(OBJ) $(OBJ_TOPLEVEL) $(LIBS)
	@echo "---------------"
	@echo "Build finished!"
	@echo "---------------"

build/default/%.o: %.cpp
	$(COMPILER) -c $(OPTIM) $(FLAGS) $(DEBUG) -o $@ $<
	@$(COMPILER) $(FLAGS) -MM -MT $@ $*.cpp -MF build/deps/$*.d

build/web/%.o: %.cpp
	$(COMPILER) -c $(OPTIM) $(FLAGS) $(DEBUG) -o $@ $<
	@$(COMPILER) $(FLAGS) -MM -MT $@ $*.cpp -MF build/deps/$*.d

build/shared/%.o: %.cpp
	$(COMPILER) -c $(OPTIM) $(FLAGS) -fPIC -o $@ $<
	@$(COMPILER) $(FLAGS) -MM -MT $@ $*.cpp -MF build/deps/$*.d

build/coverage/%.o: %.cpp
	$(COMPILER) -c $(FLAGS) -O0 -fprofile-arcs -ftest-coverage -o $@ $<
	@$(COMPILER) $(FLAGS) -MM -MT $@ $*.cpp -MF build/deps/$*.d

build/profile/%.o: %.cpp
	$(COMPILER) -c $(OPTIM) $(FLAGS) -pg -o $@ $<
	@$(COMPILER) $(FLAGS) -MM -MT $@ $*.cpp -MF build/deps/$*.d

build/sanitized/%.o: %.cpp
	$(COMPILER) -c $(OPTIM) $(FLAGS) $(SANITIZE_FLAGS) -o $@ $<
	@$(COMPILER) $(FLAGS) -MM -MT $@ $*.cpp -MF build/deps/$*.d

$(BUILD_DIR):
	@mkdir -p $@

# Build test target
build/leekscript-test: $(BUILD_DIR) $(OBJ) $(OBJ_TEST)
	$(COMPILER) $(FLAGS) -o build/leekscript-test $(OBJ) $(OBJ_TEST) $(LIBS)
	@echo "--------------------------"
	@echo "Build (test) finished!"
	@echo "--------------------------"

# Build web target
build/leekscript-web: $(BUILD_DIR) $(OBJ_WEB)
	$(COMPILER) $(FLAGS) -o build/leekscript-web $(OBJ_WEB) $(LIBS)
	@echo "--------------------------"
	@echo "Build (web) finished!"
	@echo "--------------------------"

# Build the shared library version of the leekscript
# (libleekscript.so in build/)
build/libleekscript.so: $(BUILD_DIR) $(OBJ_LIB)
	$(COMPILER) $(FLAGS) -shared -o build/libleekscript.so $(OBJ_LIB) $(LIBS)
	@echo "-----------------------"
	@echo "Library build finished!"
	@echo "-----------------------"
lib: build/libleekscript.so

# Install the shared library by copying the libleekscript.so file
# into /usr/lib/ folder.
install: lib
	cp build/libleekscript.so /usr/lib/
	@find -iregex '.*\.\(hpp\|h\|tcc\)' | cpio -updm /usr/include/leekscript/
	@echo "------------------"
	@echo "Library installed!"
	@echo "------------------"

# Build with coverage flags enabled
build/leekscript-coverage: $(BUILD_DIR) $(OBJ_COVERAGE) $(OBJ_TEST)
	$(COMPILER) $(FLAGS) -fprofile-arcs -ftest-coverage -o build/leekscript-coverage $(OBJ_COVERAGE) $(OBJ_TEST) $(LIBS)
	@echo "--------------------------"
	@echo "Build (coverage) finished!"
	@echo "--------------------------"

# Run tests
test: FLAGS += -DDEBUG_LEAKS
test: build/leekscript-test
	@build/leekscript-test

opti: FLAGS += -DNDEBUG
opti: OPTIM := -O2
opti: DEBUG :=
opti: test

web: FLAGS += -DCOMPILER=0
web: build/leekscript-web
	@build/leekscript-web

# Benchmark
benchmark-dir:
	@mkdir -p build/benchmark

build/benchmark/Benchmark.o: benchmark/Benchmark.cpp
	$(COMPILER) -c $(OPTIM) $(FLAGS) -o "$@" "$<"

build/leekscript-benchmark: benchmark-dir build/leekscript $(OBJ_BENCHMARK)
	$(COMPILER) $(FLAGS) -o build/leekscript-benchmark $(OBJ_BENCHMARK)
	@echo "-------------------------"
	@echo "Benchmark build finished!"
	@echo "-------------------------"

# Run a benchmark
benchmark: build/leekscript-benchmark
	@build/leekscript-benchmark
	@rm results

# Run a benchmark on operator
benchmark-op: build/leekscript-benchmark
	@build/leekscript-benchmark -o
	@rm result

# Valgrind
# `apt install valgrind`
valgrind: build/leekscript-test
	valgrind --verbose --track-origins=yes --leak-check=full --show-leak-kinds=all build/leekscript-test

# Travis task, useless in local.
# Build a leekscript docker image, compile, run tests and run cpp-coveralls
# (coverage results are sent to coveralls.io).
travis:
	docker build -t leekscript --file tool/Dockerfile .
	docker run -e COVERALLS_REPO_TOKEN="$$COVERALLS_REPO_TOKEN" -e TRAVIS_BRANCH="$$TRAVIS_BRANCH" \
	    leekscript /bin/bash -c "cd leekscript; make build/leekscript-coverage && build/leekscript-coverage \
	    && cpp-coveralls -i src/ --gcov-options='-rp'"
travis-pr:
	docker build -t leekscript tool
	docker run -e TRAVIS_BRANCH="$$TRAVIS_BRANCH" \
	    leekscript /bin/bash -c "cd leekscript; make test"

# Coverage results with lcov.
# `apt install lcov`
coverage: build/leekscript-coverage
	mkdir -p build/html
	lcov --quiet --no-external --rc lcov_branch_coverage=1 --capture --initial --directory build/coverage/src --base-directory src --output-file build/html/app.info
	build/leekscript-coverage
	lcov --quiet --no-external --rc lcov_branch_coverage=1 --capture --directory build/coverage/src --base-directory src --output-file build/html/app.info
	cd build/html; genhtml --ignore-errors source --legend --precision 2 --branch-coverage app.info

demangle-coverage:
	find build/html -name "*.func-sort-c.html" -type f -exec bash -c 'echo "Demangle $$1 ..."; node tool/demangle.js $$1 > $$1.tmp; mv $$1.tmp $$1' - {} \;

# Build with profile flags enabled
build/leekscript-profile: $(BUILD_DIR) $(OBJ_PROFILE) $(OBJ_TEST)
	$(COMPILER) $(FLAGS) -pg -o build/leekscript-profile $(OBJ_PROFILE) $(OBJ_TEST) $(LIBS)
	@echo "--------------------------"
	@echo "Build (profile) finished!"
	@echo "--------------------------"

# gprof profiling, results displayed by gprof2dot & dot
profile: build/leekscript-profile
	gprof build/leekscript-profile > profile.stats
	gprof2dot profile.stats | dot -Tpng -o output.png

# Build with sanitize flags enabled
build/leekscript-sanitized: $(BUILD_DIR) $(OBJ_SANITIZED) $(OBJ_TEST)
	$(COMPILER) $(FLAGS) $(SANITIZE_FLAGS) -o build/leekscript-sanitized $(OBJ_SANITIZED) $(OBJ_TEST) $(LIBS)
	@echo "--------------------------"
	@echo "Build (sanitized) finished!"
	@echo "--------------------------"

sanitized: build/leekscript-sanitized
	@build/leekscript-sanitized

# callgrind profiling, results displayed by kcachegrind
# `apt install kcachegrind`
callgrind: FLAGS += -DNDEBUG -w
callgrind: OPTIM := -O3
callgrind: build/leekscript-test
	valgrind --tool=callgrind --dump-instr=yes --callgrind-out-file=build/callgrind.out build/leekscript-test
	kcachegrind build/callgrind.out

# Clean every build files by destroying the build/ folder.
clean:
	rm -rf build
	find . -type f -name '*.d' -delete
	rm -rf doxygen
	@echo "----------------"
	@echo "Project cleaned."
	@echo "----------------"

# Check useless headers and stuff using cppclean
# `pip install --upgrade cppclean`
cppclean:
	cppclean .

# Line couning with cloc.
# `apt-get install cloc`
cloc:
	cloc . --exclude-dir=$(CLOC_EXCLUDED)
cloc-xml:
	cloc --quiet --xml . --exclude-dir=$(CLOC_EXCLUDED)

# Documentation with doxygen
doc:
	doxygen
	@echo "------------------------"
	@echo "Documentation generated."
	@echo "------------------------"

# Get remaining to do work in project
todo:
	@grep "TODO" . -R

# Objects dependencies
-include $(DEPS)
