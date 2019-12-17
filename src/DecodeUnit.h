#include "Processor.h"

using namespace std;

#pragma once
class Processor::DecodeUnit
{
private:
public:
    DecodeUnit();
    Instruction current_instruction;
    Instruction next_instruction;
    vector<Instruction> instructions;
    void update_next_instruction(Processor::FetchUnit &fetch_unit);
    void update_current_instruction();
    void get_instructions(Processor::FetchUnit &fetch_unit);
    void decode(Processor *processor);
    void passToExecuteUnit(Processor::ExecuteUnit *executeUnit);
    bool is_empty = true;
};