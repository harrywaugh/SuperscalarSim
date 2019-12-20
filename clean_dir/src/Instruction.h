#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;

#pragma once
class Instruction {

private:
    int iterate_until_delim(int index, string line);
    void debug(int pin);
    void correct_mem_syntax();

public :
    Instruction(string line, int PC);
    Instruction();
    ~Instruction();

    string opcode   = "";
    string operand0 = "";
    string operand1 = "";
    string operand2 = "";
    int PC;
    int branch_taken = 0;

    string to_string();
};