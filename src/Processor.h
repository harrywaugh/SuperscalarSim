#include <stdio.h>
#include <iostream>
#include <sstream>
#include <vector>
#include <map>
#include <chrono>
#include <ctime>

#include "Instruction.h"

// #define DEBUG 0
#define PRINT_STATS 0

using namespace std;


class Processor
{
private:
    int PC;
    int cycles = 0;
    int executed_instructions = 0;
    int free_mem_pointer = 0;

    vector<Instruction> instructions;
    map<string, int> fn_map;
    map<string, uint32_t> var_map;
    map<std::string, int> register_map = {
        {"$zero", 0},
        {"$v0", 2}, {"$v1", 3},
        {"$a0", 4}, {"$a1", 5}, {"$a2", 6}, {"$a3", 7},
        {"$t0", 8}, {"$t1", 9}, {"$t2", 10}, {"$t3", 11}, {"$t4", 12}, {"$t5", 13}, {"$t6", 14}, {"$t7", 15},
        {"$s0", 16}, {"$s1", 17}, {"$s2", 18}, {"$s3", 19}, {"$s4", 20}, {"$s5", 21}, {"$s6", 22}, {"$s7", 23},
        {"$t8", 24}, {"$t9", 25},
        {"$ra", 31},
    };

    enum OPERATION
    {
        LI,
        EXIT,
        J,
        RETURN,
        BEQ,
        BLT,
        ADD,
        ADDI,
        SUB,
        SUBI,
        MUL,
        AND,
        OR,
        SLL,
        SRL,
        MV,
        LW,
        LA,
        SW,
        NOP
    };

    map<string, OPERATION> string_to_op_map = {
        {"exit", EXIT},
        {"li", LI},
        {"j", J},
        {"return", RETURN},
        {"beq", BEQ},
        {"blt", BLT},
        {"add", ADD},
        {"addi", ADDI},
        {"sub", SUB},
        {"subi", SUBI},
        {"mul", MUL},
        {"and", AND},
        {"or", OR},
        {"sll", SLL},
        {"srl", SRL},
        {"mv", MV},
        {"lw", LW},
        {"la", LA},
        {"sw", SW},
        {"nop", NOP}};

    uint32_t main_memory[100] = {0};
    uint32_t registers[32]       = { {0} };
    uint32_t fp_registers[32]    = { {0} };
    
    // void create_fn_map();
    Instruction fetch_instruction();
    void decode_and_execute_instruction(Instruction current_instruction);
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
};


