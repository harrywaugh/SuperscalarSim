#include "DecodeUnit.h"
#include "ALU.h"

// EXECUTE UNIT
Processor::ALU::ALU()  
{
    
}

void Processor::ALU::update_next_instruction(Processor *processor)  
{
    if (processor->alu_reservation_station.size() == 0)  return;
    next_instruction = processor->alu_reservation_station.at(0);
    processor->alu_reservation_station.erase(processor->alu_reservation_station.begin());
    is_empty = false;
}

void Processor::ALU::update_current_instruction()  
{
    current_instruction = next_instruction;
    if (current_instruction.opcode == "")
        is_empty = true;
}


void Processor::ALU::execute(Processor *processor)  
{

    if(is_empty)  return;

    switch (processor->string_to_op_map[current_instruction.opcode]) {
        case EXIT:
            processor->registers[31] = -1;
            break;
        case ADD:   
            processor->registers[processor->register_map.at(current_instruction.operand0)] =
                processor->registers[processor->register_map.at(current_instruction.operand1)] + processor->registers[processor->register_map.at(current_instruction.operand2)];
            break;
        case ADD_F:
            processor->fp_registers[processor->fp_register_map.at(current_instruction.operand0)] =
                processor->fp_registers[processor->fp_register_map.at(current_instruction.operand1)] + processor->fp_registers[processor->fp_register_map.at(current_instruction.operand2)];
            break;
        case  ADDI:
            processor->registers[processor->register_map.at(current_instruction.operand0)] =
                processor->registers[processor->register_map.at(current_instruction.operand1)] + stoi(current_instruction.operand2);
            break;
        case  ADDI_F:
            processor->fp_registers[processor->fp_register_map.at(current_instruction.operand0)] =
                processor->fp_registers[processor->fp_register_map.at(current_instruction.operand1)] + stof(current_instruction.operand2);
            break;
        case SUB:
            processor->registers[processor->register_map.at(current_instruction.operand0)] =
                processor->registers[processor->register_map.at(current_instruction.operand1)] - processor->registers[processor->register_map.at(current_instruction.operand2)];
            break;
        case SUBI:
            processor->registers[processor->register_map.at(current_instruction.operand0)] =
                processor->registers[processor->register_map.at(current_instruction.operand1)] - stoi(current_instruction.operand2);
            break;
        case MUL:
            processor->registers[processor->register_map.at(current_instruction.operand0)] =
                processor->registers[processor->register_map.at(current_instruction.operand1)] * processor->registers[processor->register_map.at(current_instruction.operand2)];
            break;
        case MULI:
            processor->registers[processor->register_map.at(current_instruction.operand0)] =
                processor->registers[processor->register_map.at(current_instruction.operand1)] * stoi(current_instruction.operand2);
            break;
        case MULI_F:
            processor->fp_registers[processor->fp_register_map.at(current_instruction.operand0)] =
                processor->fp_registers[processor->fp_register_map.at(current_instruction.operand1)] * stof(current_instruction.operand2);
            break;
        case DIVI:
            processor->registers[processor->register_map.at(current_instruction.operand0)] =
                processor->registers[processor->register_map.at(current_instruction.operand1)] / stoi(current_instruction.operand2);
            break;
        case DIVI_F:
            processor->fp_registers[processor->fp_register_map.at(current_instruction.operand0)] =
                processor->fp_registers[processor->fp_register_map.at(current_instruction.operand1)] / stof(current_instruction.operand2);
            break;
        case AND:
            processor->registers[processor->register_map.at(current_instruction.operand0)] =
                processor->registers[processor->register_map.at(current_instruction.operand1)] & processor->registers[processor->register_map.at(current_instruction.operand2)];
            break;
        case OR:
            processor->registers[processor->register_map.at(current_instruction.operand0)] =
                processor->registers[processor->register_map.at(current_instruction.operand1)] | processor->registers[processor->register_map.at(current_instruction.operand2)];
            break;
        case SLL:
            processor->registers[processor->register_map.at(current_instruction.operand0)] =
                processor->registers[processor->register_map.at(current_instruction.operand1)] << stoi(current_instruction.operand2);
            break;
        case SRL:
            processor->registers[processor->register_map.at(current_instruction.operand0)] =
                processor->registers[processor->register_map.at(current_instruction.operand1)] >> stoi(current_instruction.operand2);
            break;
        case NOP:
            break;
    }
    completeInstruction(processor);
}

void Processor::ALU::completeInstruction(Processor *processor)  
{
    processor->executed_instructions++;
    is_empty = true;

}