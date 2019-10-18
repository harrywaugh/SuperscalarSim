#include "Processor.h"

using namespace std;

Processor::Processor()  {
    PC = 0;
}

Processor::~Processor()
{
}

void Processor::incrementPC()  {
    PC++;
}

void Processor::addInstruction(string line)  {
    Instruction new_instruction(line);
    instructions.push_back(new_instruction);
}

void Processor::start()  {
    
}