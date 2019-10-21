#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;

class Instruction {

private:
    string opcode   = "";
    string operand0 = "";
    string operand1 = "";
    string operand2 = "";
    string fn_name;
    

    int iterate_until_delim(int index, string line);
    void debug(int pin);
    void correct_mem_syntax();

public : Instruction(string line);
    string to_string();
    bool is_fn = true;
    ~Instruction();
};