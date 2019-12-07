CC=g++

CFLAGS=-std=c++11 -Wno-braced-scalar-init -Wno-writable-strings

simulator: src/main.cpp
	@echo "Compiling simulator..."
	${CC} ${CFLAGS} src/*.cpp -o simulator.exe $(VARS)

pipelined: src/main.cpp
	make simulator VARS="-DPIPELINED ${VARS}"

pipelined-stats: src/main.cpp
	make pipelined VARS="-DPRINT_STATS=1 ${VARS}"

pipelined-debug: src/main.cpp
	make pipelined-stats VARS="-DDEBUG=1 ${VARS}"



# stats: src/main.cpp
# 	@echo "Compiling simulator..."
# 	${CC} -std=c++11 src/*.cpp -o simulator.exe -DPRINT_STATS=1 -DPIPELINED=1 ${WARN}

# non-pipelined: src/main.cpp
# 	@echo "Compiling simulator..."
# 	${CC} -std=c++11 src/*.cpp -o simulator.exe -DPRINT_STATS=1 -DPIPELINED=0 ${WARN}

# debug: src/main.cpp
# 	@echo "Compiling simulator..."
# 	${CC} -std=c++11 src/*.cpp -o simulator.exe -DDEBUG=1 -DPRINT_STATS=1 -DPIPELINED=1 $(VARS) ${WARN}

# tests: clean simulator
# 	@echo "Running tests...\n"
# 	@python src/run_tests.py

clean:
	@echo "Cleaning up..."
	rm -rf *.exe

.PHONY:
	clean
