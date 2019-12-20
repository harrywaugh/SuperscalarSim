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

void Processor::FetchUnit::get_instructions(Processor *processor)
{
    instructions.clear();
    int counter = 0;
    while ((processor->PC < processor->instructions.size()) && (counter < FETCH_INSTR_PER_CYCLE))
    {
        switch (processor->string_to_op_map[processor->instructions.at(processor->PC).opcode]) {
            case J:
                instructions.push_back(processor->instructions.at(processor->PC));
                processor->PC = processor->fn_map.at(processor->instructions.at(processor->PC).operand0)-1;
                break;
            case BEQ:
                instructions.push_back(processor->instructions.at(processor->PC));
                if (processor->get_current_branch_state() == STRONGLY_TAKEN || processor->get_current_branch_state() == WEAKLY_TAKEN)
                {
                    processor->PC += stoi(processor->instructions.at(processor->PC).operand2)-1;
                    instructions.back().branch_taken = 1;
                }
                else
                    instructions.back().branch_taken = 0;
                break;
            case BLT:
                instructions.push_back(processor->instructions.at(processor->PC));
                if (processor->get_current_branch_state() == STRONGLY_TAKEN || processor->get_current_branch_state() == WEAKLY_TAKEN)
                {
                    processor->PC += stoi(processor->instructions.at(processor->PC).operand2)-1;
                    instructions.back().branch_taken = 1;
                }
                else
                    instructions.back().branch_taken = 0;
                break;
            default:
                instructions.push_back(processor->instructions.at(processor->PC));
                break;
        }
        processor->incrementPC();
        counter++;
    }
}

void Processor::FetchUnit::fetch(Processor *processor)
{
    
    if (is_empty)  return;

    switch (processor->string_to_op_map[current_instruction.opcode]) {
        case J:
            processor->PC = processor->fn_map.at(current_instruction.operand0);
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