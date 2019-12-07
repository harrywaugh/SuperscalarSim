#include "Processor.h"

using namespace std;

#pragma once
class Processor::ExecuteUnit
{
private:
public:
    ExecuteUnit();
    Instruction current_instruction;
    Instruction next_instruction;
    void update_next_instruction(Processor::DecodeUnit &decode_unit);
    void update_current_instruction();
    void execute(Processor *processor);
    void completeInstruction(Processor *processor);
    bool is_empty = true;
};