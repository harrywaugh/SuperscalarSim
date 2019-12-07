#include "DecodeUnit.h"
#include "ExecuteUnit.h"

// DECODE UNIT
Processor::DecodeUnit::DecodeUnit()  
{
    
}

void Processor::DecodeUnit::newInstruction(Instruction new_instruction)  
{
    current_instruction = new_instruction;
    is_empty = false;
}

void Processor::DecodeUnit::decode()  
{
    if (is_empty)  return;
    // cout << "Decoding instruction: " << current_instruction.to_string() << endl;
}

void Processor::DecodeUnit::passToExecuteUnit(Processor::ExecuteUnit *execute_unit)  
{
    execute_unit->newInstruction(current_instruction);
#if PIPELINED==0
    is_empty = true;
#endif
}