#include <stdint.h>

#include "output.h"


void debug_simulator(int pc, char ***program, uint8_t debug)  {
	if (debug == 1)  {
		printf("Debug:\n");
		printf("PC %d\n", pc);
		printf("Current Instruction %s\n", (*program)[pc]);
	}
}