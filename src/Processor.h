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

#include "Instruction.h"
// #include "FetchUnit.h"

// #define DEBUG 0
// #define PRINT_STATS 0
// #define PRINT_MEM_TO_BITMAP

#define FETCH_UNITS 1
#define DECODE_UNITS 1
#define EXECUTE_UNITS 1

// #define PIPELINED 1

using namespace std;

#pragma once
class Processor
{
private:
    int PC;
    int cycles = 0;
    int executed_instructions = 0;
    int free_mem_pointer = 0;
    int speculative_jumps = 0;
    bool refresh_flag = false;

    Instruction nop_instruction;

    class ExecuteUnit;
    class DecodeUnit;
    class FetchUnit;

    vector<FetchUnit> fetch_units;
    vector<DecodeUnit> decode_units;
    vector<ExecuteUnit> execute_units;
    vector<Instruction> instructions;
    queue<int> branch_record;
    stack<int> return_address_stack;
    map<string, int> fn_map;
    map<int, string> fn_map_reverse;
    map<string, int> var_map;
    map<std::string, int> register_map = {
        {"$zero", 0},
        {"$v0", 2}, {"$v1", 3},
        {"$a0", 4}, {"$a1", 5}, {"$a2", 6}, {"$a3", 7},
        {"$t0", 8}, {"$t1", 9}, {"$t2", 10}, {"$t3", 11}, {"$t4", 12}, {"$t5", 13}, {"$t6", 14}, {"$t7", 15},
        {"$s0", 16}, {"$s1", 17}, {"$s2", 18}, {"$s3", 19}, {"$s4", 20}, {"$s5", 21}, {"$s6", 22}, {"$s7", 23},
        {"$t8", 24}, {"$t9", 25},
        {"$ra", 31},
    };
    map<std::string, int> fp_register_map = {
        {"$f0", 0}, {"$f1", 1}, {"$f2", 2}, {"$f3", 3}, {"$f4", 4}, {"$f5", 5}, {"$f6", 6}, {"$f7", 7}, {"$f8", 8}, {"$f9", 9}, {"$f10", 10},
        {"$f11", 11}, {"$f12", 12}, {"$f13", 13}, {"$f14", 14}, {"$f15", 15}, {"$f16", 16}, {"$f17", 17}, {"$f18", 18}, {"$f19", 19}, 
        {"$f20", 20}, {"$f21", 21}, {"$f22", 22}, {"$f23", 23}, {"$f24", 24}, {"$f25", 25}, {"$f26", 26}, {"$f27", 27}, {"$f28", 28}, 
        {"$f31", 31},
    };

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

    int main_memory[1024] = {0};
    int registers[32]       = { {0} };
    float fp_registers[32]    = { {0} };
    
    // void create_fn_map();
    void fetch_instructions();
    void decode_instructions();
    void execute_instructions();
    void refresh_pipeline();
    void debug_processor();

   

public:
    Processor();
    ~Processor();
    void addInstruction(string line);
    void addVariable(string line);
    void addArray(string line);
    void addFunction(string line);

    void incrementPC();
    void incrementCycles();

    void run_program();

    void output_image(char *filename, const int nx, const int ny, int* image);

    
};    