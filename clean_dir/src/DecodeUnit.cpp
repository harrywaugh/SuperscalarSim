#include "FetchUnit.h"
#include "DecodeUnit.h"
#include "ExecuteUnit.h"

// DECODE UNIT
Processor::DecodeUnit::DecodeUnit()  
{
    
}

void Processor::DecodeUnit::update_next_instruction(Processor::FetchUnit &fetch_unit)  
{
    if (fetch_unit.is_empty)  
    {
        is_empty = true;
        return;
    }
    next_instruction = fetch_unit.current_instruction;
    is_empty = false;
}

void Processor::DecodeUnit::update_current_instruction()
{
    if (is_empty)  return;
    current_instruction = next_instruction;
}

void Processor::DecodeUnit::get_instructions(Processor::FetchUnit &fetch_unit)
{
    instructions.clear();
    int counter = 0;
    while ((counter < fetch_unit.instructions.size()) && (instructions.size() < DECODE_INSTR_PER_CYCLE))
        instructions.push_back(fetch_unit.instructions.at(counter++));
}



void Processor::DecodeUnit::decode(Processor *processor)  
{
    #ifdef SUPERSCALAR
        for (int i = 0; i < instructions.size(); i++)
        {
            processor->instruction_queue.push_back(instructions.at(i));
        }
    #else
        if(is_empty)  return;
        processor->instruction_queue.push_back(current_instruction);
    #endif
}
