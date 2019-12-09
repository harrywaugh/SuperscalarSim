#include "DecodeUnit.h"
#include "MemoryUnit.h"

// EXECUTE UNIT
Processor::MemoryUnit::MemoryUnit()  
{
    
}

void Processor::MemoryUnit::update_next_instruction(Processor *processor)  
{
    if (processor->mem_reservation_station.size() == 0)  return;
    if (!is_empty)  return;
    next_instruction = processor->mem_reservation_station.at(0);
    processor->mem_reservation_station.erase(processor->mem_reservation_station.begin());
    is_empty = false;
}

void Processor::MemoryUnit::update_current_instruction()  
{
    current_instruction = next_instruction;
    if (current_instruction.opcode == "")
        is_empty = true;
}


void Processor::MemoryUnit::execute(Processor *processor)  
{

    if(is_empty)  return;

    switch (processor->string_to_op_map[current_instruction.opcode]) {
        case MV:
            processor->registers[processor->register_map.at(current_instruction.operand0)] = processor->registers[processor->register_map.at(current_instruction.operand1)];
            break;
        case LI:
            processor->registers[processor->register_map.at(current_instruction.operand0)] = stoi(current_instruction.operand1);
            break;
        case LI_F:
            processor->fp_register_file[processor->fp_register_map.at(current_instruction.operand0)] = stof(current_instruction.operand1);
            break;
        case LW:
            if (blocking_for == -1)  blocking_for = MEM_ACCESS_TIME;
            blocking_for--;
            processor->cycles_waiting_for_memory++;
            if (blocking_for > 0)  return;
            processor->registers[processor->register_map.at(current_instruction.operand0)] = processor->main_memory[processor->registers[processor->register_map.at(current_instruction.operand2)] +
                                                                                   processor->registers[processor->register_map.at(current_instruction.operand1)]];
            processor->cycles_waiting_for_memory+=2;
            break;
        case LW_F:
            if (blocking_for == -1)  blocking_for = MEM_ACCESS_TIME;
            blocking_for--;
            processor->cycles_waiting_for_memory++;
            if (blocking_for > 0)  return;
            memcpy(&processor->fp_register_file[processor->fp_register_map.at(current_instruction.operand0)], 
                   &(processor->main_memory)[processor->registers[processor->register_map.at(current_instruction.operand2)] + processor->registers[processor->register_map.at(current_instruction.operand1)]],
                   sizeof(float));
            processor->cycles_waiting_for_memory+=2;
            break;
        case LA:
            processor->registers[processor->register_map.at(current_instruction.operand0)] = processor->var_map.at(current_instruction.operand1);
            break;
        case SW:
            if (blocking_for == -1)  blocking_for = MEM_ACCESS_TIME;
            blocking_for--;
            processor->cycles_waiting_for_memory++;
            if (blocking_for > 0)  return;
            processor->main_memory[processor->registers[processor->register_map.at(current_instruction.operand2)] +
                        processor->registers[processor->register_map.at(current_instruction.operand1)]] = processor->registers[processor->register_map.at(current_instruction.operand0)];
            processor->cycles_waiting_for_memory+=2;
            break;
        case SW_F:
            if (blocking_for == -1)  blocking_for = MEM_ACCESS_TIME;
            blocking_for--;
            processor->cycles_waiting_for_memory++;
            if (blocking_for > 0)  return;
            memcpy(&(processor->main_memory)[processor->registers[processor->register_map.at(current_instruction.operand2)] + processor->registers[processor->register_map.at(current_instruction.operand1)]],
                   &processor->fp_register_file[processor->fp_register_map.at(current_instruction.operand0)], 
                   sizeof(float));
            processor->cycles_waiting_for_memory+=2;
            break;
    }
    completeInstruction(processor);
}

void Processor::MemoryUnit::completeInstruction(Processor *processor)  
{
    processor->executed_instructions++;
    is_empty = true;

}