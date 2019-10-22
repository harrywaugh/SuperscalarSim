#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

#include <string>
#include <list>
#include <iostream>
#include <fstream>

#include "Processor.h"

void run_simulator(char ***program, int program_len);

int iterate_until_delim(int index, string line);