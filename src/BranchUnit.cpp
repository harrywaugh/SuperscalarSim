#include "DecodeUnit.h"
#include "BranchUnit.h"

// EXECUTE UNIT
Processor::BranchUnit::BranchUnit()  
{
    
}

void Processor::BranchUnit::update_next_instruction(Processor *processor)  
{
    if (processor->branch_reservation_station.size() == 0)  return;
    next_instruction = processor->branch_reservation_station.at(0);
    processor->branch_reservation_station.erase(processor->branch_reservation_station.begin());
    is_empty = false;
}

void Processor::BranchUnit::update_current_instruction()  
{
    current_instruction = next_instruction;
    if (current_instruction.opcode == "")
        is_empty = true;
}


void Processor::BranchUnit::execute(Processor *processor)  
{

    if(is_empty)  return;

    switch (processor->string_to_op_map[current_instruction.opcode]) {
        case EXIT:
            processor->registers[31] = -1;
            break;
        case BEQ:
            if (processor->registers[processor->register_map.at(current_instruction.operand0)] !=
                processor->registers[processor->register_map.at(current_instruction.operand1)])
            {
                processor->PC = processor->branch_record.front() + 1;
                processor->refresh_pipeline();
                while (!processor->branch_record.empty())
                    processor->branch_record.pop();
            }
            else
            {
                processor->branch_record.pop();
            }
            break;
        case BLT:
            if (processor->registers[processor->register_map.at(current_instruction.operand0)] >=
                processor->registers[processor->register_map.at(current_instruction.operand1)])
            {
                processor->PC = processor->branch_record.front() + 1;
                processor->refresh_pipeline();
                while (!processor->branch_record.empty())
                    processor->branch_record.pop();
            }
            else
            {
                processor->branch_record.pop();
            }
            break;
    }
    completeInstruction(processor);
}

void Processor::BranchUnit::completeInstruction(Processor *processor)  
{
    processor->executed_instructions++;
    is_empty = true;
}