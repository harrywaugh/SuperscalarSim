#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <string>
#include <list>
#include <fstream>
#include <iostream>

using namespace std;

void read_program_file(string filename, list<string>& program);

int find_fn(char* fn, char ***program, int program_len);