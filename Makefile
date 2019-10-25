CC=g++

simulator: src/main.cpp
	@echo "Compiling simulator..."
	${CC} -std=c++11 src/*.cpp -o simulator.exe

clean:
	@echo "Cleaning up..."
	rm -rf *.exe

.PHONY:
	clean
