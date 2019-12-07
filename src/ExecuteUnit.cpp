#include "DecodeUnit.h"
#include "ExecuteUnit.h"

// EXECUTE UNIT
Processor::ExecuteUnit::ExecuteUnit()  
{
    
}

void Processor::ExecuteUnit::update_next_instruction(DecodeUnit &decode_unit)  
{
    next_instruction = decode_unit.current_instruction;
    is_empty = false;
}

void Processor::ExecuteUnit::update_current_instruction()  
{
    current_instruction = next_instruction;
    if (current_instruction.opcode == "")
        is_empty = true;
}


void Processor::ExecuteUnit::execute(Processor *processor)  
{

    if(is_empty)  return;

    switch (processor->string_to_op_map[current_instruction.opcode]) {
        case EXIT:
            processor->registers[31] = -1;
            break;
        case BEQ:
            #ifdef PIPELINED
                if (processor->registers[processor->register_map.at(current_instruction.operand0)] ==
                    processor->registers[processor->register_map.at(current_instruction.operand1)])
                {
                    processor->PC += stoi(current_instruction.operand2)-1;
                }
            #else
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
            #endif
            break;
        case BLT:
            #ifdef PIPELINED
                if (processor->registers[processor->register_map.at(current_instruction.operand0)] <
                    processor->registers[processor->register_map.at(current_instruction.operand1)])
                {
                    processor->PC += stoi(current_instruction.operand2)-1;
                }
            #else
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
            #endif
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
        case NOP:
            break;
        default:
            break;
    }
    completeInstruction(processor);
}

void Processor::ExecuteUnit::completeInstruction(Processor *processor)  
{
    processor->executed_instructions++;
}