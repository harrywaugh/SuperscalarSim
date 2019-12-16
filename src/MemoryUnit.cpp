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
    current_operand0          = rs_entry.val0;
    current_operand1          = rs_entry.val1;
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
            current_result = processor->main_memory[current_operand1 + current_operand0];
            processor->cycles_waiting_for_memory+=2;
            break;
        case LW_F:
            if (blocking_for == -1)  blocking_for = MEM_ACCESS_TIME;
            blocking_for--;
            processor->cycles_waiting_for_memory++;
            if (blocking_for > 0)  return;
            memcpy(&current_result, &(processor->main_memory[current_operand1 + current_operand0]), sizeof(float));
            processor->cycles_waiting_for_memory+=2;
            break;
        case LA:
            current_result = current_operand0;
            break;
        case SW:
            if (blocking_for == -1)  blocking_for = MEM_ACCESS_TIME;
            blocking_for--;
            processor->cycles_waiting_for_memory++;
            if (blocking_for > 0)  return;
            processor->main_memory[current_operand1 + current_operand0] = current_result;
            processor->cycles_waiting_for_memory+=2;
            break;
        case SW_F:
            if (blocking_for == -1)  blocking_for = MEM_ACCESS_TIME;
            blocking_for--;
            processor->cycles_waiting_for_memory++;
            if (blocking_for > 0)  return;
            memcpy(&(processor->main_memory[current_operand1 + current_operand0]), &current_result, sizeof(float));
            processor->cycles_waiting_for_memory+=2;
            break;
    }
    completeInstruction(processor);
}

void Processor::MemoryUnit::completeInstruction(Processor *processor)  
{
    processor->executed_instructions++;
    is_empty = true;
    ready_for_broadcast = true;
}

void Processor::MemoryUnit::print_state_string()
{
     cout << "Memory Unit: ";
    if (is_empty)
        cout << "Empty";
    else 
        cout << current_operation << " " << current_operand0 << " " << current_operand1;
    cout << endl;
}