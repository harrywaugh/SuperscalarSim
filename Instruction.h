#include <stdio.h>
#include <string>

using namespace std;

class Instruction {

private:
    string opcode;
    string operand0;
    string operand1;
    string operand2;

public:
    Instruction(string opcode);
    ~Instruction();
};