#include "Processor.h"

using namespace std;

#pragma once
class Processor::FetchUnit
{
private:
public:
    FetchUnit();
    Instruction current_instruction;
    Instruction next_instruction;
    vector<Instruction> instructions;
    void update_next_instruction(Processor *processor);
    void update_current_instruction();
    void get_instructions(Processor *processor);
    void fetch(Processor *processor);
    void passToDecodeUnit(Processor::DecodeUnit *decodeUnit);
    bool is_empty = true;
};