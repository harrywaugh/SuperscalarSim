#include "Processor.h"

#define MEM_ACCESS_TIME 1

using namespace std;

#pragma once
class Processor::MemoryUnit
{
private:
public:
    MemoryUnit();
    Instruction current_instruction;
    Instruction next_instruction;
    void update_next_instruction(Processor *processor);
    void update_current_instruction();
    void execute(Processor *processor);
    void completeInstruction(Processor *processor);
    void print_state_string();

    bool is_empty = true;
    void update_current_instruction(RS_entry &rs_entry);
    int32_t current_operand0, current_operand1;
    int32_t current_result;
    int rob_dst; 
    OPERATION current_operation;
    int blocking_for = -1;
    bool ready_for_broadcast = false;
};