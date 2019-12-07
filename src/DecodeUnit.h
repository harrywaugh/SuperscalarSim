#include "Processor.h"

using namespace std;

#pragma once
class Processor::DecodeUnit
{
private:
public:
    DecodeUnit();
    Instruction current_instruction;
    void newInstruction(Instruction new_instruction);
    void decode();
    void passToExecuteUnit(Processor::ExecuteUnit *executeUnit);
    bool is_empty = true;
};