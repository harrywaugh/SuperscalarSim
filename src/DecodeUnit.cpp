#include "FetchUnit.h"
#include "DecodeUnit.h"
#include "ExecuteUnit.h"

// DECODE UNIT
Processor::DecodeUnit::DecodeUnit()  
{
    
}

void Processor::DecodeUnit::update_next_instruction(Processor::FetchUnit &fetch_unit)  
{
    next_instruction = fetch_unit.current_instruction;
    is_empty = false;
}

void Processor::DecodeUnit::update_current_instruction()
{
    current_instruction = next_instruction;
    if (current_instruction.opcode == "")
        is_empty = true;
}


void Processor::DecodeUnit::decode(Processor *processor)  
{
    if(is_empty)  return;

    processor->instruction_queue.push(current_instruction);
}
