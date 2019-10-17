#include "simulator.h"

#define MAX_PROGRAM_LEN 50
#define MAX_PROGRAM_LINE_LEN 100

int main(int argn, char *argv[])  {
    // Read in filename and declare variables.
    char *filename = argv[1];
    char **program_str;
    int program_len;

    if (argn < 1)  { // Is filename given?
        printf("No filename given.");
        return 1;
    }

    // Print filename and read filename intro string.
    printf("Program Name: %s\n", filename);
    program_len = read_program_file(filename, &program_str);

    //Run the simulator
    run_simulator(&program_str, program_len);

    return 0;
}

// Fetch -> Decode -> Execute -> Writeback
void run_simulator(char ***program, int program_len)  {
    int pc = find_fn("main", program, program_len);
    int clock = 0;
    uint32_t a[4]  = {0}; // Argument registers
    uint32_t t[10] = {0}; // Temporary registers
    uint32_t v[2]  = {0}; // Return registers
    uint32_t s[8]  = {0}; // Computed registers
    uint8_t exit   = 0;
    char *opcode;
    

    while (exit == 0)  {
        debug_simulator(pc, program, 1);   // Debug
        curr_instruction = (*program)[pc]; // Fetch



        if (pc >= program_len)  exit = 1;
    }


}

