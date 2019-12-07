#include "DecodeUnit.h"
#include "MemoryUnit.h"

// EXECUTE UNIT
Processor::MemoryUnit::MemoryUnit()  
{
    
}

void Processor::MemoryUnit::update_next_instruction(Processor *processor)  
{
    if (processor->mem_reservation_station.size() == 0)  return;
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
            processor->fp_registers[processor->fp_register_map.at(current_instruction.operand0)] = stof(current_instruction.operand1);
            break;
        case LW:
            processor->registers[processor->register_map.at(current_instruction.operand0)] = processor->main_memory[processor->registers[processor->register_map.at(current_instruction.operand2)] +
                                                                                   processor->registers[processor->register_map.at(current_instruction.operand1)]];
            break;
        case LW_F:
            memcpy(&processor->fp_registers[processor->fp_register_map.at(current_instruction.operand0)], 
                   &(processor->main_memory)[processor->registers[processor->register_map.at(current_instruction.operand2)] + processor->registers[processor->register_map.at(current_instruction.operand1)]],
                   sizeof(float));
            break;
        case LA:
            processor->registers[processor->register_map.at(current_instruction.operand0)] = processor->var_map.at(current_instruction.operand1);
            break;
        case SW:
            processor->main_memory[processor->registers[processor->register_map.at(current_instruction.operand2)] +
                        processor->registers[processor->register_map.at(current_instruction.operand1)]] = processor->registers[processor->register_map.at(current_instruction.operand0)];
            break;
        case SW_F:
            memcpy(&(processor->main_memory)[processor->registers[processor->register_map.at(current_instruction.operand2)] + processor->registers[processor->register_map.at(current_instruction.operand1)]],
                   &processor->fp_registers[processor->fp_register_map.at(current_instruction.operand0)], 
                   sizeof(float));
            break;
    }
    completeInstruction(processor);
}

void Processor::MemoryUnit::completeInstruction(Processor *processor)  
{
    processor->executed_instructions++;
    is_empty = true;

}