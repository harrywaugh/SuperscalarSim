CC=g++

CFLAGS=-std=c++11 -Wno-braced-scalar-init -Wno-writable-strings

simulator: src/main.cpp
	@echo "Compiling simulator..."
	${CC} ${CFLAGS} src/*.cpp -o simulator.exe $(VARS)


################################################
## Non Pipelined
################################################

stats: src/main.cpp
	make simulator VARS="-DPRINT_STATS=1 ${VARS}"

debug: src/main.cpp
	make simulator-stats VARS="-DDEBUG ${VARS}"

test: clean simulator
	echo "Running tests...\n"
	@python src/run_tests.py


################################################
## Pipelined
################################################

pipelined: src/main.cpp
	make simulator VARS="-DPIPELINED=1 ${VARS}"

pipelined-stats: src/main.cpp
	make pipelined VARS="-DPRINT_STATS=1 ${VARS}"

pipelined-debug: src/main.cpp
	make pipelined-stats VARS="-DDEBUG ${VARS}"

pipelined-test: clean pipelined
	echo "Running tests...\n"
	@python src/run_tests.py


################################################

clean:
	@echo "Cleaning up..."
	rm -rf *.exe

.PHONY:
	clean
