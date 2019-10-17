CC=gcc

simulator: simulator.c
	@echo "Compiling simulator..."
	${CC} -std=c99 *.c -o simulator.exe

clean:
	@echo "Cleaning up..."
	rm -rf *.exe

.PHONY:
	clean