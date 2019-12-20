#include "Processor.h"

using namespace std;

#pragma once
class Processor::BranchUnit
{
private:
public:
    BranchUnit();
    Instruction current_instruction;
    Instruction next_instruction;
    void update_next_instruction(Processor *processor);
    void update_current_instruction();
    void execute(Processor *processor);
    void completeInstruction(Processor *processor);
    void update_current_instruction(RS_entry &rs_entry);
    void print_state_string();

    int32_t current_operand0, current_operand1, branch_taken;
    int32_t current_result;
    int rob_dst;
    OPERATION current_operation;
    bool is_empty = true;
    bool ready_for_broadcast = false;
};