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
	make pipelined VARS=" -DSUPERSCALAR=1 -DBRANCH_PREDICTOR=3 ${VARS}"

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
## Superscalar BP = Not Taken
################################################

superscalar-NT: src/main.cpp
	make superscalar VARS="-DBRANCH_PREDICTOR=0 ${VARS}"

superscalar-stats-NT: src/main.cpp
	make superscalar-NT VARS="-DPRINT_STATS=1 ${VARS}"

superscalar-debug-NT: src/main.cpp
	make superscalar-stats-NT VARS="-DDEBUG ${VARS}"

superscalar-test-NT: clean superscalar-NT
	@python3 src/run_tests.py
	./simulator.exe programs/stencil.benchmark > /dev/null
	# @echo "Difference between stencil results and correct image:\n"
	# diff 16x16_correct.pgm after.pgm 

################################################
## Superscalar BP = Taken
################################################

superscalar-T: src/main.cpp
	make superscalar VARS="-DBRANCH_PREDICTOR=1 ${VARS}"

superscalar-stats-T: src/main.cpp
	make superscalar-T VARS="-DPRINT_STATS=1 ${VARS}"

superscalar-debug-T: src/main.cpp
	make superscalar-stats-T VARS="-DDEBUG ${VARS}"

superscalar-test-T: clean superscalar-T
	@python3 src/run_tests.py
	./simulator.exe programs/stencil.benchmark > /dev/null
	# @echo "Difference between stencil results and correct image:\n"
	# diff 16x16_correct.pgm after.pgm 


################################################
## Superscalar BP = 2 Bit Dynamic
################################################

superscalar-2bit: src/main.cpp
	make superscalar VARS="-DBRANCH_PREDICTOR=2 ${VARS}"

superscalar-stats-2bit: src/main.cpp
	make superscalar-2bit VARS="-DPRINT_STATS=1 ${VARS}"

superscalar-debug-2bit: src/main.cpp
	make superscalar-stats-2bit VARS="-DDEBUG ${VARS}"

superscalar-test-2bit: clean superscalar-2bit
	@python3 src/run_tests.py
	./simulator.exe programs/stencil.benchmark > /dev/null
	# @echo "Difference between stencil results and correct image:\n"
	# diff 16x16_correct.pgm after.pgm 

################################################
## Superscalar BP = 2 Bit BTB
################################################

superscalar-2bit-btb: src/main.cpp
	make superscalar VARS="-DBRANCH_PREDICTOR=3 ${VARS}"

superscalar-stats-2bit-btb: src/main.cpp
	make superscalar-2bit-btb VARS="-DPRINT_STATS=1 ${VARS}"

superscalar-debug-2bit-btb: src/main.cpp
	make superscalar-stats-2bit-btb VARS="-DDEBUG ${VARS}"

superscalar-test-2bit-btb: clean superscalar-2bit-btb
	@python3 src/run_tests.py
	./simulator.exe programs/stencil.benchmark > /dev/null
	# @echo "Difference between stencil results and correct image:\n"
	# diff 16x16_correct.pgm after.pgm 


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
	make superscalar-test-NT -j

	@echo "Running superscalar tests..."
	make superscalar-test-T -j

	@echo "Running superscalar tests..."
	make superscalar-test-2bit -j

	@echo "Running superscalar tests..."
	make superscalar-test-2bit-btb -j



clean:
	@echo "Cleaning up..."
	rm -rf *.exe

.PHONY:
	clean
