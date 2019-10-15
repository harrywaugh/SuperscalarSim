#include "simulator.h"

#define MAX_PROGRAM_LEN 50
#define MAX_PROGRAM_LINE_LEN 100

int main(int argn, char *argv[])  {
    char *filename = argv[1];
    char **program_str;
    int program_len;

    if (argn < 1)  {
        printf("No filename given.");
        return 1;
    }
    printf("Program Name: %s\n", filename);
    program_len = read_program_file(filename, &program_str);

    return 0;
}