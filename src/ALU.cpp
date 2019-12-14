#include "DecodeUnit.h"
#include "ALU.h"

// EXECUTE UNIT
Processor::ALU::ALU()  
{
    
}

void Processor::ALU::update_next_instruction(Processor *processor)  
{
    // if (processor->alu_reservation_station.size() == 0)  return;
    // next_instruction = processor->alu_reservation_station.at(0);
    // processor->alu_reservation_station.erase(processor->alu_reservation_station.begin());
    // is_empty = false;
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

    float tmp0, tmp1, tmp2;
    switch (processor->string_to_op_map[current_instruction.opcode]) {
        case EXIT:
            processor->register_file[31] = -1;
            break;
        case ADD:   
            processor->register_file[processor->register_map.at(current_instruction.operand0)] =
                processor->register_file[processor->register_map.at(current_instruction.operand1)] + processor->register_file[processor->register_map.at(current_instruction.operand2)];
            break;
        case ADD_F:
            memcpy(&tmp1, &processor->register_file[processor->register_map.at(current_instruction.operand1)], sizeof(float));
            memcpy(&tmp2, &processor->register_file[processor->register_map.at(current_instruction.operand2)], sizeof(float));
            tmp0 = tmp1 + tmp2;
            memcpy(&processor->register_file[processor->register_map.at(current_instruction.operand0)], &tmp0, sizeof(float));
            break;
        case  ADDI:
            processor->register_file[processor->register_map.at(current_instruction.operand0)] =
                processor->register_file[processor->register_map.at(current_instruction.operand1)] + stoi(current_instruction.operand2);
            break;
        case  ADDI_F:
            memcpy(&tmp1, &processor->register_file[processor->register_map.at(current_instruction.operand1)], sizeof(float));
            tmp0 = tmp1 + stof(current_instruction.operand2);
            memcpy(&processor->register_file[processor->register_map.at(current_instruction.operand0)], &tmp0, sizeof(float));
            break;
        case SUB:
            processor->register_file[processor->register_map.at(current_instruction.operand0)] =
                processor->register_file[processor->register_map.at(current_instruction.operand1)] - processor->register_file[processor->register_map.at(current_instruction.operand2)];
            break;
        case SUBI:
            processor->register_file[processor->register_map.at(current_instruction.operand0)] =
                processor->register_file[processor->register_map.at(current_instruction.operand1)] - stoi(current_instruction.operand2);
            break;
        case MUL:
            processor->register_file[processor->register_map.at(current_instruction.operand0)] =
                processor->register_file[processor->register_map.at(current_instruction.operand1)] * processor->register_file[processor->register_map.at(current_instruction.operand2)];
            break;
        case MULI:
            processor->register_file[processor->register_map.at(current_instruction.operand0)] =
                processor->register_file[processor->register_map.at(current_instruction.operand1)] * stoi(current_instruction.operand2);
            break;
        case MULI_F:
            memcpy(&tmp1, &processor->register_file[processor->register_map.at(current_instruction.operand1)], sizeof(float));
            tmp0 = tmp1 * stof(current_instruction.operand2);
            memcpy(&processor->register_file[processor->register_map.at(current_instruction.operand0)], &tmp0, sizeof(float));
            break;
        case DIVI:
            processor->register_file[processor->register_map.at(current_instruction.operand0)] =
                processor->register_file[processor->register_map.at(current_instruction.operand1)] / stoi(current_instruction.operand2);
            break;
        case DIVI_F:
            memcpy(&tmp1, &processor->register_file[processor->register_map.at(current_instruction.operand1)], sizeof(float));
            tmp0 = tmp1 / stof(current_instruction.operand2);
            memcpy(&processor->register_file[processor->register_map.at(current_instruction.operand0)], &tmp0, sizeof(float));
            break;
        case AND:
            processor->register_file[processor->register_map.at(current_instruction.operand0)] =
                processor->register_file[processor->register_map.at(current_instruction.operand1)] & processor->register_file[processor->register_map.at(current_instruction.operand2)];
            break;
        case OR:
            processor->register_file[processor->register_map.at(current_instruction.operand0)] =
                processor->register_file[processor->register_map.at(current_instruction.operand1)] | processor->register_file[processor->register_map.at(current_instruction.operand2)];
            break;
        case SLL:
            processor->register_file[processor->register_map.at(current_instruction.operand0)] =
                processor->register_file[processor->register_map.at(current_instruction.operand1)] << stoi(current_instruction.operand2);
            break;
        case SRL:
            processor->register_file[processor->register_map.at(current_instruction.operand0)] =
                processor->register_file[processor->register_map.at(current_instruction.operand1)] >> stoi(current_instruction.operand2);
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