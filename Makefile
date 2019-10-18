CC=icpc

simulator: simulator.cpp
	@echo "Compiling simulator..."
	${CC} -std=c++11 *.c *.cpp -o simulator.exe

clean:
	@echo "Cleaning up..."
	rm -rf *.exe

.PHONY:
	clean