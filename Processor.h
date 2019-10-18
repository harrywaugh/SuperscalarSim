#include <stdio.h>
#include <list>
#include "Instruction.h"

using namespace std;

class Processor
{
private:
    int PC;
    list<Instruction> instructions;

public:
    Processor(/* args */);
    ~Processor();
    void addInstruction(string line);
    void incrementPC();
    void start();
};


