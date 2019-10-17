#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int read_program_file(char *filename, char ***program_str);

int find_fn(char* fn, char ***program, int program_len);