#include "Processor.h"

using namespace std;

#pragma once
class Processor::ALU
{
private:
public:
    ALU();
    Instruction current_instruction;
    Instruction next_instruction;
    void update_next_instruction(Processor *processor);
    void update_current_instruction();
    void execute(Processor *processor);
    void completeInstruction(Processor *processor);
    bool is_empty = true;
};