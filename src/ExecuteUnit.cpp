#include "DecodeUnit.h"
#include "ExecuteUnit.h"

// EXECUTE UNIT
Processor::ExecuteUnit::ExecuteUnit()  
{
    
}

void Processor::ExecuteUnit::update_next_instruction(DecodeUnit &decode_unit)  
{
    if (decode_unit.is_empty)
    {
        is_empty = true;
        return;
    }
    next_instruction = decode_unit.current_instruction;
    is_empty = false;
}

void Processor::ExecuteUnit::update_current_instruction()  
{
    current_instruction = next_instruction;
}


void Processor::ExecuteUnit::execute(Processor *processor)  
{

    if(is_empty)  return;

    float tmp0, tmp1, tmp2;
    switch (processor->string_to_op_map[current_instruction.opcode]) {
        case EXIT:
            processor->register_file[31] = -1;
            break;
        case BEQ:
            #ifdef PIPELINED
                if (processor->register_file[processor->register_map.at(current_instruction.operand0)] !=
                    processor->register_file[processor->register_map.at(current_instruction.operand1)])
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
            #else
                if (processor->register_file[processor->register_map.at(current_instruction.operand0)] ==
                    processor->register_file[processor->register_map.at(current_instruction.operand1)])
                {
                    processor->PC += stoi(current_instruction.operand2)-1;
                }
            #endif
            break;
        case BLT:
            #ifdef PIPELINED
                if (processor->register_file[processor->register_map.at(current_instruction.operand0)] >=
                    processor->register_file[processor->register_map.at(current_instruction.operand1)])
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
            #else
                if (processor->register_file[processor->register_map.at(current_instruction.operand0)] <
                    processor->register_file[processor->register_map.at(current_instruction.operand1)])
                {
                    processor->PC += stoi(current_instruction.operand2)-1;
                }
            #endif
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
        case MV:
            processor->register_file[processor->register_map.at(current_instruction.operand0)] = processor->register_file[processor->register_map.at(current_instruction.operand1)];
            break;
        case LI:
            processor->register_file[processor->register_map.at(current_instruction.operand0)] = stoi(current_instruction.operand1);
            break;
        case LI_F:
            tmp0 = stof(current_instruction.operand1); 
            memcpy(&processor->register_file[processor->register_map.at(current_instruction.operand0)], &tmp0, sizeof(float));
            break;
        case LW:
            processor->register_file[processor->register_map.at(current_instruction.operand0)] = processor->main_memory[processor->register_file[processor->register_map.at(current_instruction.operand2)] +
                                                                                   processor->register_file[processor->register_map.at(current_instruction.operand1)]];
            processor->cycles+=MEM_ACCESS_TIME-1;
            processor->cycles_waiting_for_memory+=MEM_ACCESS_TIME-1;
            break;
        case LW_F:
            memcpy(&processor->register_file[processor->register_map.at(current_instruction.operand0)], 
                   &(processor->main_memory)[processor->register_file[processor->register_map.at(current_instruction.operand2)] + processor->register_file[processor->register_map.at(current_instruction.operand1)]],
                   sizeof(float));
            processor->cycles+=MEM_ACCESS_TIME-1;
            processor->cycles_waiting_for_memory+=MEM_ACCESS_TIME-1;
            break;
        case LA:
            processor->register_file[processor->register_map.at(current_instruction.operand0)] = processor->var_map.at(current_instruction.operand1);
            break;
        case SW:
            processor->main_memory[processor->register_file[processor->register_map.at(current_instruction.operand2)] +
                        processor->register_file[processor->register_map.at(current_instruction.operand1)]] = processor->register_file[processor->register_map.at(current_instruction.operand0)];
            processor->cycles+=MEM_ACCESS_TIME-1;
            processor->cycles_waiting_for_memory+=MEM_ACCESS_TIME-1;
            break;
        case SW_F:
            memcpy(&(processor->main_memory)[processor->register_file[processor->register_map.at(current_instruction.operand2)] + processor->register_file[processor->register_map.at(current_instruction.operand1)]],
                   &processor->register_file[processor->register_map.at(current_instruction.operand0)], 
                   sizeof(float));
            processor->cycles+=MEM_ACCESS_TIME-1;
            processor->cycles_waiting_for_memory+=MEM_ACCESS_TIME-1;
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