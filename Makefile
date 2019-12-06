CC=g++

simulator: src/main.cpp
	@echo "Compiling simulator..."
	${CC} -std=c++11 src/*.cpp -o simulator.exe -DPIPELINED=1 $(VARS)

stats: src/main.cpp
	@echo "Compiling simulator..."
	${CC} -std=c++11 src/*.cpp -o simulator.exe -DPRINT_STATS=1 -DPIPELINED=1

non-pipelined-stats: src/main.cpp
	@echo "Compiling simulator..."
	${CC} -std=c++11 src/*.cpp -o simulator.exe -DPRINT_STATS=1 -DPIPELINED=0

debug: src/main.cpp
	@echo "Compiling simulator..."
	${CC} -std=c++11 src/*.cpp -o simulator.exe -DDEBUG=1 -DPRINT_STATS=1 -DPIPELINED=1 $(VARS)

tests: clean simulator
	@echo "Running tests...\n"
	@python src/run_tests.py

clean:
	@echo "Cleaning up..."
	rm -rf *.exe

.PHONY:
	clean
