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
    switch (processor->string_to_op_map[current_instruction.opcode]) 
    {
        case BEQ: case BLT: case J: case RETURN: case EXIT:
            processor->branch_reservation_station.push_back(current_instruction);
            break;
        case LW: case LA: case LI: case SW: case LW_F: case SW_F: case MV:
            processor->mem_reservation_station.push_back(current_instruction);
            break;
        default:
            processor->alu_reservation_station.push_back(current_instruction);
            break;
    }
}
