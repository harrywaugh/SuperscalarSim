CC=g++

simulator: main.cpp
	@echo "Compiling simulator..."
	${CC} -std=c++11 *.cpp -o simulator.exe

clean:
	@echo "Cleaning up..."
	rm -rf *.exe

.PHONY:
	clean
