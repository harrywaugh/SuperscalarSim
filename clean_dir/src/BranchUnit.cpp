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
    branch_taken              = rs_entry.val2;
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
            current_result = (current_operand0 == current_operand1) ? 1 : 0;
            if (BRANCH_PREDICTOR <= 2)
                processor->update_branch_state(current_result);
            else
                processor->update_branch_state(current_result, processor->reorder_buffer[rob_dst].pc_src);
            if ((branch_taken == 1 && current_result == 0) || (branch_taken == 0 && current_result == 1))
                current_result = 0;
            else
                current_result = 1;
            break;
        case BLT:
            current_result = (current_operand0 < current_operand1) ? 1 : 0;
            if (BRANCH_PREDICTOR <= 2)
                processor->update_branch_state(current_result);
            else
                processor->update_branch_state(current_result, processor->reorder_buffer[rob_dst].pc_src);
            if ((branch_taken == 1 && current_result == 0) || (branch_taken == 0 && current_result == 1))
                current_result = 0;
            else
                current_result = 1;
            break;
    }
    completeInstruction(processor);
}

void Processor::BranchUnit::completeInstruction(Processor *processor)  
{
    processor->executed_instructions++;
    is_empty = true;
    ready_for_broadcast = true;

    if (rob_dst < 0 || rob_dst >= REORDER_BUFFER_SIZE)
        cout << "ERROR: Branch unit broadcasting to non-existant ROB entry" << endl;

    memcpy(&processor->reorder_buffer[rob_dst].value, &current_result, sizeof(int32_t));
    processor->reorder_buffer[rob_dst].done = true;
}

void Processor::BranchUnit::print_state_string()
{
    cout << "  Branch Unit: ";
    if (is_empty)
        cout << "Empty";
    else 
        cout << "ROB" << rob_dst << " " << current_operation << " " << current_operand0 << " " << current_operand1;
}