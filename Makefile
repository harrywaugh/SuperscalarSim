CC=g++

WARN=-Wno-braced-scalar-init -Wno-writable-strings

simulator: src/main.cpp
	@echo "Compiling simulator..."
	${CC} -std=c++11 src/*.cpp -o simulator.exe -DPIPELINED=1 $(VARS) ${WARN}

stats: src/main.cpp
	@echo "Compiling simulator..."
	${CC} -std=c++11 src/*.cpp -o simulator.exe -DPRINT_STATS=1 -DPIPELINED=1 ${WARN}

non-pipelined-stats: src/main.cpp
	@echo "Compiling simulator..."
	${CC} -std=c++11 src/*.cpp -o simulator.exe -DPRINT_STATS=1 -DPIPELINED=0 ${WARN}

debug: src/main.cpp
	@echo "Compiling simulator..."
	${CC} -std=c++11 src/*.cpp -o simulator.exe -DDEBUG=1 -DPRINT_STATS=1 -DPIPELINED=1 $(VARS) ${WARN}

tests: clean simulator
	@echo "Running tests...\n"
	@python src/run_tests.py

clean:
	@echo "Cleaning up..."
	rm -rf *.exe

.PHONY:
	clean
