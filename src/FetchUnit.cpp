#include "FetchUnit.h"
#include "DecodeUnit.h"

// FETCH UNIT
Processor::FetchUnit::FetchUnit()  
{
    
}

void Processor::FetchUnit::update_next_instruction(Processor *processor)  
{
    if (processor->PC >= processor->instructions.size())  
    {
        is_empty = true;
        return;
    }
    next_instruction = processor->instructions.at(processor->PC);
    is_empty = false;
}

void Processor::FetchUnit::update_current_instruction()  
{
    if (is_empty)  return;
    current_instruction = next_instruction;
}

void Processor::FetchUnit::fetch(Processor *processor)
{
    if (is_empty)  return;
    switch (processor->string_to_op_map[current_instruction.opcode]) {
        case J:
            processor->return_address_stack.push(current_instruction.PC);
            processor->PC = processor->fn_map.at(current_instruction.operand0);
            break;
        case RETURN:
            processor->PC = processor->return_address_stack.top()+1;
            processor->return_address_stack.pop();
            // Suspected bug here
            break;
        case BEQ:
            #ifdef PIPELINED 
                processor->branch_record.push(current_instruction.PC);
                processor->PC += stoi(current_instruction.operand2)-1;
            #endif
            break;
        case BLT:
            #ifdef PIPELINED 
                processor->branch_record.push(current_instruction.PC);
                processor->PC += stoi(current_instruction.operand2)-1;
            #endif
            break;
        default:
            break;
    }
    return;
}