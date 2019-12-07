#include "Processor.h"

using namespace std;

#pragma once
class Processor::FetchUnit
{
private:
public:
    FetchUnit();
    Instruction current_instruction;
    void newInstruction(Processor *processor);
    void fetch(Processor *processor);
    void passToDecodeUnit(Processor::DecodeUnit *decodeUnit);
    bool is_empty = true;
};