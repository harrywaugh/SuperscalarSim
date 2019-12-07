#include "FetchUnit.h"
#include "DecodeUnit.h"

// FETCH UNIT
Processor::FetchUnit::FetchUnit()  
{
    
}

void Processor::FetchUnit::newInstruction(Processor *processor)  
{
    if (processor->PC >= processor->instructions.size())  
        current_instruction = processor->nop_instruction;
    else
        current_instruction = processor->instructions.at(processor->PC);
    is_empty = false;
}

void Processor::FetchUnit::fetch(Processor *processor)
{
    switch (processor->string_to_op_map[current_instruction.opcode]) {
        case J:
            // processor->registers[31] = processor->PC;
            processor->return_address_stack.push(processor->PC);
            // cout << "Store RA: " << processor->PC << endl;
            processor->PC = processor->fn_map.at(current_instruction.operand0)-1;
#if PIPELINED==0
            // processor->PC += 1;
#endif

            break;
        case RETURN:
            // cout << "Bouncing back to " << processor->registers[31] << endl;
            processor->PC = processor->return_address_stack.top();
            processor->return_address_stack.pop();
            break;
        case BEQ:
            if (PIPELINED==0) return;
            processor->branch_record.push(processor->PC);
            // printf("Pushing %d to Branch Record\n", processor->PC);
            processor->PC += stoi(current_instruction.operand2)-1;
            break;
        case BLT:
            if (PIPELINED==0) return;
            processor->branch_record.push(processor->PC);
            // printf("Pushing %d to Branch Record\n", processor->PC);
            processor->PC += stoi(current_instruction.operand2)-1;
            break;
        default:
            break;
    }
    return;
}

void Processor::FetchUnit::passToDecodeUnit(Processor::DecodeUnit *decode_unit)  
{
    decode_unit->newInstruction(current_instruction);
#if PIPELINED==0
    is_empty = true;
#endif
}