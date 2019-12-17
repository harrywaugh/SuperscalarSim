#include "DecodeUnit.h"
#include "BranchUnit.h"

// EXECUTE UNIT
Processor::BranchUnit::BranchUnit()  
{
    
}

void Processor::BranchUnit::update_next_instruction(Processor *processor)  
{
//     if (processor->branch_reservation_station.size() == 0)  return;
//     next_instruction = processor->branch_reservation_station.at(0);
//     processor->branch_reservation_station.erase(processor->branch_reservation_station.begin());
    // is_empty = false;
}

void Processor::BranchUnit::update_current_instruction()  
{
    current_instruction = next_instruction;
    if (current_instruction.opcode == "")
        is_empty = true;
}

void Processor::BranchUnit::update_current_instruction(RS_entry &rs_entry)  
{
    current_operand0          = rs_entry.val0;
    current_operand1          = rs_entry.val1;
    current_operation = rs_entry.op;
    rob_dst = rs_entry.rob_dst;
    is_empty = false;
}

void Processor::BranchUnit::execute(Processor *processor)  
{

    if(is_empty)  return;

    switch (current_operation) {
        case EXIT:
            current_result = -1;
            break;
        case BEQ:
            // if (current_operand0 != current_operand1)
            // {
            //     processor->PC = processor->branch_record.front() + 1;
            //     processor->refresh_pipeline();
            //     while (!processor->branch_record.empty())
            //         processor->branch_record.pop();
            // }
            // else
            // {
            //     processor->branch_record.pop();
            // }
            current_result = (current_operand0 == current_operand1) ? 1 : 0;
            break;
        case BLT:
            // if (current_operand0 >= current_operand1)
            // {
            //     processor->PC = processor->branch_record.front() + 1;
            //     processor->refresh_pipeline();
            //     while (!processor->branch_record.empty())
            //         processor->branch_record.pop();
            // }
            // else
            // {
            //     processor->branch_record.pop();
            // }
            current_result = (current_operand0 < current_operand1) ? 1 : 0;
            break;
    }
    completeInstruction(processor);
}

void Processor::BranchUnit::completeInstruction(Processor *processor)  
{
    processor->executed_instructions++;
    is_empty = true;
    ready_for_broadcast = true;

    memcpy(&processor->reorder_buffer[rob_dst].value, &current_result, sizeof(int32_t));
    processor->reorder_buffer[rob_dst].done = true;
}

void Processor::BranchUnit::print_state_string()
{
    cout << "  Branch Unit: ";
    if (is_empty)
        cout << "Empty";
    else 
        cout << current_operation << " " << current_operand0 << " " << current_operand1;
}