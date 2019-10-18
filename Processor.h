#include <stdio.h>
#include <iostream>
#include <vector>
#include <map>

#include "Instruction.h"

using namespace std;

class Processor
{
private:
    int PC;
    vector<string> instructions;
    map<string, int> fn_map;
    void create_fn_map();
    uint32_t main_memory[100][4] = {0};

public:
    Processor(/* args */);
    ~Processor();
    void addInstruction(string line);
    void incrementPC();
    void run_program();
};


