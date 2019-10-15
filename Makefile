CC=gcc

simulator: simulator.c
	@echo "Compiling simulator..."
	${CC} -std=c99 $^ parse.c -o simulator.exe

clean:
	@echo "Cleaning up..."
	rm *.exe

.PHONY:
	clean