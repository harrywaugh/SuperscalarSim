#include "FetchUnit.h"
#include "DecodeUnit.h"

// FETCH UNIT
Processor::FetchUnit::FetchUnit()  
{
    
}

void Processor::FetchUnit::update_next_instruction(Processor *processor)  
{
    if (processor->PC >= processor->instructions.size())  
        next_instruction = processor->nop_instruction;
    else
        next_instruction = processor->instructions.at(processor->PC);
    is_empty = false;
}

void Processor::FetchUnit::update_current_instruction()  
{
    current_instruction = next_instruction;
}

void Processor::FetchUnit::fetch(Processor *processor)
{
    switch (processor->string_to_op_map[current_instruction.opcode]) {
        case J:
            processor->return_address_stack.push(processor->PC);
            processor->PC = processor->fn_map.at(current_instruction.operand0)-1;
            break;
        case RETURN:
            processor->PC = processor->return_address_stack.top();
            processor->return_address_stack.pop();
            break;
        case BEQ:
            if (PIPELINED==0) return;
            processor->branch_record.push(processor->PC);
            processor->PC += stoi(current_instruction.operand2)-1;
            break;
        case BLT:
            if (PIPELINED==0) return;
            processor->branch_record.push(processor->PC);
            processor->PC += stoi(current_instruction.operand2)-1;
            break;
        default:
            break;
    }
    return;
}

// void Processor::FetchUnit::passToDecodeUnit(Processor::DecodeUnit *decode_unit)  
// {
//     decode_unit->update_next_instruction(current_instruction);
// #if PIPELINED==0
//     is_empty = true;
// #endif
// }