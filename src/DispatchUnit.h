#include "Processor.h"

using namespace std;

#pragma once
class Processor::DispatchUnit
{
private:
public:
    DispatchUnit();
    Instruction current_instruction;
    Instruction next_instruction;
    void update_next_instruction(Processor::DecodeUnit &decode_unit);
    void update_current_instruction();
    void decode(Processor *processor);
    void passToExecuteUnit(Processor::ExecuteUnit *executeUnit);
    bool is_empty = true;
};