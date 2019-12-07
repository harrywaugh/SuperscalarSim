#include "Processor.h"

using namespace std;

#pragma once
class Processor::ExecuteUnit
{
private:
public:
    ExecuteUnit();
    Instruction current_instruction;
    void newInstruction(Instruction new_instruction);
    void execute(Processor *processor);
    void completeInstruction(Processor *processor);
    bool is_empty = true;
};