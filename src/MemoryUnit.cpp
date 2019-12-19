#include "DecodeUnit.h"
#include "MemoryUnit.h"

// EXECUTE UNIT
Processor::MemoryUnit::MemoryUnit()  
{
    
}

void Processor::MemoryUnit::update_next_instruction(Processor *processor)  
{
    // if (processor->mem_reservation_station.size() == 0)  return;
    // if (!is_empty)  return;
    // next_instruction = processor->mem_reservation_station.at(0);
    // processor->mem_reservation_station.erase(processor->mem_reservation_station.begin());
    // is_empty = false;
}

void Processor::MemoryUnit::update_current_instruction()  
{
    // current_instruction = next_instruction;
    // if (current_instruction.opcode == "")
    //     is_empty = true;
}

void Processor::MemoryUnit::update_current_instruction(RS_entry &rs_entry)  
{
    memcpy(&current_operand0, &rs_entry.val0, sizeof(int32_t));
    memcpy(&current_operand1, &rs_entry.val1, sizeof(int32_t));
    memcpy(&current_operand2, &rs_entry.val2, sizeof(int32_t));

    current_operation = rs_entry.op;
    is_empty = false;
    rob_dst = rs_entry.rob_dst;

}


void Processor::MemoryUnit::execute(Processor *processor)  
{

    if(is_empty)  return;

    switch (current_operation) {
        case MV:
            current_result = current_operand0;
            break;
        case LI:
            current_result = current_operand0;
            break;
        case LI_F:
            float tmp;
            tmp = stof(current_instruction.operand1); 
            memcpy(&current_result, &tmp, sizeof(float));
            break;
        case LW:
            if (blocking_for == -1)  blocking_for = MEM_ACCESS_TIME;
            blocking_for--;
            processor->cycles_waiting_for_memory++;
            
            if (blocking_for > 0)  return;
            if ((current_operand1 + current_operand0) >= 1024 || (current_operand1 + current_operand0) < 0)
            {
                cout << "Mem unit trying to access non-existant memory" << endl;
                cout << current_operation << " " << current_operand0 << " " << current_operand1 << " " << current_operand2 << endl;
            }
            current_result = processor->main_memory[current_operand1 + current_operand0];
            break;
        case LW_F:
            if (blocking_for == -1)  blocking_for = MEM_ACCESS_TIME;
            blocking_for--;
            processor->cycles_waiting_for_memory++;
            if (blocking_for > 0)  return;
            
            if ((current_operand1 + current_operand0) >= 1024 || (current_operand1 + current_operand0) < 0)
            {
                cout << "Mem unit trying to access non-existant memory" << endl;
                cout << current_operation << " " << current_operand0 << " " << current_operand1 << " " << current_operand2 << endl;
            }
            memcpy(&current_result, &(processor->main_memory[current_operand1 + current_operand0]), sizeof(float));
            break;
        case LA:
            current_result = current_operand0;
            break;
        case SW:
            if (blocking_for == -1)  blocking_for = MEM_ACCESS_TIME;
            blocking_for--;
            processor->cycles_waiting_for_memory++;
            
            if (blocking_for > 0)  return;
            if ((current_operand1 + current_operand2) >= 1024 || (current_operand1 + current_operand2) < 0)
            {
                cout << "Mem unit trying to access non-existant memory" << endl;
                cout << current_operation << " " << current_operand0 << " " << current_operand1 << " " << current_operand2 << endl;
            }
            processor->main_memory[current_operand1 + current_operand2] =  current_operand0;
            break;
        case SW_F:
            if (blocking_for == -1)  blocking_for = MEM_ACCESS_TIME;
            blocking_for--;
            processor->cycles_waiting_for_memory++;
            
            if (blocking_for > 0)  return;
            if ((current_operand1 + current_operand2) >= 1024 || (current_operand1 + current_operand2) < 0)
            {
                cout << "Mem unit trying to access non-existant memory" << endl;
                cout << current_operation << " " << current_operand0 << " " << current_operand1 << " " << current_operand2 << endl;
            }
            memcpy(&(processor->main_memory[current_operand1 + current_operand2]), &current_operand0, sizeof(float));
            break;
    }
    completeInstruction(processor);
}

void Processor::MemoryUnit::completeInstruction(Processor *processor)  
{
    processor->executed_instructions++;
    is_empty = true;

    for (int r = 0; r < MEM_RES_STATION_SIZE; r++)
    {
        RS_entry *rs_entry = &processor->mem_reservation_station[r];
        if (!rs_entry->is_empty)
        {
            if (rs_entry->rob_op0_dependency == rob_dst)
            {
                rs_entry->rob_op0_dependency = -1;
                memcpy(&rs_entry->val0, &current_result, sizeof(int32_t));
            }
            if (rs_entry->rob_op1_dependency == rob_dst)
            {
                rs_entry->rob_op1_dependency = -1;
                memcpy(&rs_entry->val1, &current_result, sizeof(int32_t));
            }
            if (rs_entry->rob_op2_dependency == rob_dst)
            {
                rs_entry->rob_op2_dependency = -1;
                memcpy(&rs_entry->val2, &current_result, sizeof(int32_t));
                
            }

        }
    }

    for (int r = 0; r < ALU_RES_STATION_SIZE; r++)
    {
        RS_entry *rs_entry = &processor->alu_reservation_station[r];
        if (!rs_entry->is_empty)
        {
            if (rs_entry->rob_op0_dependency == rob_dst)
            {
                rs_entry->rob_op0_dependency = -1;
                memcpy(&rs_entry->val0, &current_result, sizeof(int32_t));
            }
            if (rs_entry->rob_op1_dependency == rob_dst)
            {
                rs_entry->rob_op1_dependency = -1;
                memcpy(&rs_entry->val1, &current_result, sizeof(int32_t));
            }
        }
    }

    for (int r = 0; r < BRANCH_RES_STATION_SIZE; r++)
    {
        RS_entry *rs_entry = &processor->branch_reservation_station[r];
        if (!rs_entry->is_empty)
        {
            if (rs_entry->rob_op0_dependency == rob_dst)
            {
                rs_entry->rob_op0_dependency = -1;
                memcpy(&rs_entry->val0, &current_result, sizeof(int32_t));
            }
            if (rs_entry->rob_op1_dependency == rob_dst)
            {
                rs_entry->rob_op1_dependency = -1;
                memcpy(&rs_entry->val1, &current_result, sizeof(int32_t));
            }
        }
    }


    if (rob_dst < 0 || rob_dst >= REORDER_BUFFER_SIZE)
        cout << "ERROR: Memory unit broadcasting to non-existant ROB entry" << endl;

    memcpy(&processor->reorder_buffer[rob_dst].value, &current_result, sizeof(int32_t));
    processor->reorder_buffer[rob_dst].done = true;
}

void Processor::MemoryUnit::print_state_string()
{
     cout << "  Memory Unit: ";
    if (is_empty)
        cout << "Empty";
    else 
        cout << "ROB" << rob_dst << " " << current_operation << " " << current_operand0 << " " << current_operand1;
}