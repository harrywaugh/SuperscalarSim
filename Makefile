CC=g++

CFLAGS=-std=c++11 -Wno-braced-scalar-init -Wno-writable-strings -Wno-switch

simulator: src/main.cpp
	@echo "Compiling simulator..."
	${CC} ${CFLAGS} src/*.cpp -o simulator.exe $(VARS)


################################################
## Non Pipelined
################################################

stats: src/main.cpp
	make simulator VARS="-DPRINT_STATS=1 ${VARS}"

debug: src/main.cpp
	make stats VARS="-DDEBUG ${VARS}"

test: clean simulator
	@python3 src/run_tests.py


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
	@python3 src/run_tests.py

################################################
## Superscalar
################################################

superscalar: src/main.cpp
	make pipelined VARS="-DSUPERSCALAR=1 ${VARS}"

superscalar-stats: src/main.cpp
	make superscalar VARS="-DPRINT_STATS=1 ${VARS}"

superscalar-debug: src/main.cpp
	make superscalar-stats VARS="-DDEBUG ${VARS}"

superscalar-test: clean superscalar
	@python3 src/run_tests.py
	./simulator.exe programs/stencil.benchmark > /dev/null
	@echo "Difference between stencil results and correct image:\n"
	diff 16x16_correct.pgm after.pgm 

################################################
## Misc
################################################


check:
	@echo "Running scalar tests..."
	make test -j
	./simulator.exe programs/stencil.benchmark > /dev/null
	@echo "Difference between stencil results and correct image:\n"
	diff 16x16_correct.pgm after.pgm 

	@echo "Running pipelined tests..."
	make pipelined-test -j
	./simulator.exe programs/stencil.benchmark > /dev/null
	@echo "Difference between stencil results and correct image:\n"
	diff 16x16_correct.pgm after.pgm 

	@echo "Running superscalar tests..."
	make superscalar-test -j



clean:
	@echo "Cleaning up..."
	rm -rf *.exe

.PHONY:
	clean
