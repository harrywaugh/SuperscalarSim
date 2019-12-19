#include <stdio.h>
#include <iostream>
#include <sstream>
#include <vector>
#include <map>
#include <queue>
#include <stack>
#include <chrono>
#include <ctime>
#include <iomanip> 
#include <string> 
#include <cstring>

#include "Instruction.h"

// #include "FetchUnit.h"

// #define DEBUG 0
// #define PRINT_STATS 0
// #define PRINT_MEM_TO_BITMAP
// #define PIPELINED 1

#define SUPERSCALAR_WIDTH 6

#ifdef SUPERSCALAR
    #define FETCH_INSTR_PER_CYCLE SUPERSCALAR_WIDTH
    #define DECODE_INSTR_PER_CYCLE SUPERSCALAR_WIDTH
#else
    #define FETCH_INSTR_PER_CYCLE 1
    #define DECODE_INSTR_PER_CYCLE 1
#endif

#define COMMIT_INSTR_PER_CYCLE SUPERSCALAR_WIDTH
#define ISSUE_INSTR_PER_CYCLE SUPERSCALAR_WIDTH

#define EXECUTE_UNITS 1
#define MEM_UNITS 1
#define BRANCH_UNITS 1
#define ALU_UNITS 4

#define ALU_RES_STATION_SIZE 16
#define BRANCH_RES_STATION_SIZE 8
#define MEM_RES_STATION_SIZE 8
#define REORDER_BUFFER_SIZE 32

// #define BRANCH_PREDICTOR 0 //"STATIC_TAKEN"
#define BRANCH_PREDICTOR 1 //"DYNAMIC_TWO_BIT_SATURATING"
// #define BRANCH_PREDICTOR "DYNAMIC_TWO_LEVEL_ADAPTIVE"

#define MEM_ACCESS_TIME 3


enum OPERATION
{
    LI,
    LI_F,
    EXIT,
    J,
    RETURN,
    BEQ,
    BLT,
    ADD,        
    ADD_F,
    ADDI,        
    ADDI_F,
    SUB,
    SUBI,
    MUL,
    MULI,
    MULI_F,
    DIVI,
    DIVI_F,
    AND,
    OR,
    SLL,
    SRL,
    MV,
    LW,
    LW_F,
    LA,
    SW,
    SW_F,
    NOP
};

enum BRANCH_STATE {
    STRONGLY_NOT_TAKEN = 0,
    WEAKLY_NOT_TAKEN   = 1,
    WEAKLY_TAKEN       = 2,
    STRONGLY_TAKEN     = 3,
};

using namespace std;


typedef struct ROB_entry {
    // Void so works for floating point and integer
    int p_register_dst; 
    OPERATION op;
    int32_t value;
    bool done;
    bool is_empty;
} ROB_entry;

typedef struct RS_entry {
    OPERATION op;
    int rob_dst;
    int rob_op0_dependency;
    int rob_op1_dependency;
    int rob_op2_dependency;
    int32_t val0;
    int32_t val1;
    int32_t val2;
    int32_t instruction_id;
    bool is_empty;
} RS_entry;

#pragma once
class Processor
{
private:
    // Stats
    int PC;
    int cycles = 0;
    int executed_instructions = 0;
    int cycles_waiting_for_memory = 0;
    int free_mem_pointer = 0;
    int branches_mispredicts = 0;
    int executed_branches = 0;
    bool refresh_flag = false;
    BRANCH_STATE current_branch_state = WEAKLY_TAKEN;

    Instruction nop_instruction;

    class FetchUnit;
    class DecodeUnit;
    class ExecuteUnit;
    class BranchUnit;
    class MemoryUnit;
    class ALU;

    FetchUnit *fetch_unit;
    DecodeUnit *decode_unit;
    vector<ExecuteUnit> execute_units;
    vector<BranchUnit> branch_units;
    vector<MemoryUnit> mem_units;
    vector<ALU> alu_units;

    int main_memory[1024] = {0};

    int32_t register_file[64]              = { {0} };
    int32_t register_alias_table[64]       = { {-1} };

    ROB_entry reorder_buffer[REORDER_BUFFER_SIZE];
    int currently_issued_instructions = 0;
    
    short ROB_commit_pointer = 0;
    short ROB_issue_pointer  = 0;

    vector<Instruction> instructions;
    vector<Instruction> instruction_queue;

    RS_entry alu_reservation_station[ALU_RES_STATION_SIZE];
    RS_entry branch_reservation_station[BRANCH_RES_STATION_SIZE];
    RS_entry mem_reservation_station[MEM_RES_STATION_SIZE];

    queue<int> branch_record;
    stack<int> return_address_stack;

    map<string, int> fn_map;
    map<int, string> fn_map_reverse;
    map<string, int> var_map;
    map<std::string, int32_t> register_map = {
        {"$zero", 0},
        {"$v0", 2}, {"$v1", 3},
        {"$a0", 4}, {"$a1", 5}, {"$a2", 6}, {"$a3", 7},
        {"$t0", 8}, {"$t1", 9}, {"$t2", 10}, {"$t3", 11}, {"$t4", 12}, {"$t5", 13}, {"$t6", 14}, {"$t7", 15},
        {"$s0", 16}, {"$s1", 17}, {"$s2", 18}, {"$s3", 19}, {"$s4", 20}, {"$s5", 21}, {"$s6", 22}, {"$s7", 23},
        {"$t8", 24}, {"$t9", 25},
        {"$ra", 31},
        {"$f0", 32}, {"$f1", 33}, {"$f2", 34}, {"$f3", 35}, {"$f4", 36}, {"$f5", 37}, {"$f6", 38}, {"$f7", 39}, {"$f8", 40}, {"$f9", 41}, {"$f10", 42},
        {"$f11", 43}, {"$f12", 44}, {"$f13", 45}, {"$f14", 46}, {"$f15", 47}, {"$f16", 48}, {"$f17", 49}, {"$f18", 50}, {"$f19", 51}, 
        {"$f20", 52}, {"$f21", 53}, {"$f22", 54}, {"$f23", 55}, {"$f24", 56}, {"$f25", 57}, {"$f26", 58}, {"$f27", 59}, {"$f28", 60}, 
        {"$f31", 61}
    };

    map<string, OPERATION> string_to_op_map = {
        {"exit", EXIT},
        {"li", LI},
        {"li.f", LI_F},
        {"j", J},
        {"return", RETURN},
        {"beq", BEQ},
        {"blt", BLT},
        {"add", ADD},        
        {"add.f", ADD_F},
        {"addi", ADDI},
        {"addi.f", ADDI_F},
        {"sub", SUB},
        {"subi", SUBI},
        {"mul", MUL},
        {"muli", MULI},
        {"muli.f", MULI_F},
        {"divi", DIVI},
        {"divi.f", DIVI_F},
        {"and", AND},
        {"or", OR},
        {"sll", SLL},
        {"srl", SRL},
        {"mv", MV},
        {"lw", LW},
        {"lw.f", LW_F},
        {"la", LA},
        {"sw", SW},
        {"sw.f", SW_F},
        {"nop", NOP}};


    
    // void create_fn_map();
    void update_instructions();
    void fetch_instructions();
    void decode_instructions();
    void execute_instructions();
    void refresh_pipeline();
    void debug_processor();
    void printRSEntry(RS_entry &rs_entry);

   

public:
    Processor();
    ~Processor();
    void addInstruction(string line);
    void addVariable(string line);
    void addArray(string line);
    void addFunction(string line);

    void incrementPC();
    void incrementCycles();
    void incrementROBCommit();
    void incrementROBIssue();
    void update_branch_state(int branch_taken);

    void issue();
    void dispatch();
    void broadcast();
    void commit();

    void run_program();

    void output_image(char *filename, const int nx, const int ny, int* image);

};    

ostream& operator<<(std::ostream& out, OPERATION op);
