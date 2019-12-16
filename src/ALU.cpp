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
    // current_instruction = next_instruction;
    // if (current_instruction.opcode == "")
    //     is_empty = true;
}

void Processor::ALU::update_current_instruction(RS_entry &rs_entry)  
{
    current_operand0          = rs_entry.val0;
    current_operand1          = rs_entry.val1;
    current_operation = rs_entry.op;
    ready_for_broadcast = false;
    rob_dst = rs_entry.rob_dst;
    is_empty = false;
}


void Processor::ALU::execute(Processor *processor)  
{
    if(is_empty)  return;

    float tmp0, tmp1, tmp2;
    switch (current_operation) {
        case ADD: case ADDI:
            current_result = current_operand0 + current_operand1;
            break;
        case ADD_F: case ADDI_F:
            memcpy(&tmp1, &current_operand0, sizeof(float));
            memcpy(&tmp2, &current_operand1, sizeof(float));
            tmp0 = tmp1 + tmp2;
            memcpy(&current_result, &tmp0, sizeof(float));
            break;
        case SUB: case SUBI:
            current_result = current_operand0 - current_operand1;
            break;
        case MUL: case MULI:
            current_result = current_operand0 * current_operand1;
            break;
        case MULI_F:
            memcpy(&tmp1, &current_operand0, sizeof(float));
            memcpy(&tmp2, &current_operand1, sizeof(float));
            tmp0 = tmp1 * tmp2;
            memcpy(&current_result, &tmp0, sizeof(float));
            break;
        case DIVI:
            current_result = current_operand0 / current_operand1;
            break;
        case DIVI_F:
            memcpy(&tmp1, &current_operand0, sizeof(float));
            memcpy(&tmp2, &current_operand1, sizeof(float));
            tmp0 = tmp1 / tmp2;
            memcpy(&current_result, &tmp0, sizeof(float));
            break;
        case AND:
            current_result = current_operand0 & current_operand1;
            break;
        case OR:
            current_result = current_operand0 | current_operand1;
            break;
        case SLL:
            current_result = current_operand0 << current_operand1;
            break;
        case SRL:
            current_result = current_operand0 >> current_operand1;
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
    ready_for_broadcast = true;
}

void Processor::ALU::print_state_string()
{
    cout << "ALU: ";
    if (is_empty)
        cout << "Empty";
    else 
        cout << current_operation << " " << current_operand0 << " " << current_operand1;
    cout << endl;
}