#include "Processor.h"
#include "FetchUnit.h"
#include "DecodeUnit.h"
#include "ExecuteUnit.h"
#include "MemoryUnit.h"
#include "ALU.h"
#include "BranchUnit.h"

using namespace std;

// Processor Constructor
Processor::Processor()  
{
    PC = 0;


    fetch_unit = new FetchUnit;
    decode_unit = new DecodeUnit;

    Instruction nop("\tnop", -1);
    nop_instruction = nop;

    for (int e=0; e < EXECUTE_UNITS; e++)
    {
        ExecuteUnit new_execute_unit;
        execute_units.push_back(new_execute_unit);
    }

    for (int a=0; a < ALU_UNITS; a++)
    {
        ALU new_alu;
        alu_units.push_back(new_alu);
    }

    for (int b=0; b < BRANCH_UNITS; b++)
    {
        BranchUnit new_branch_unit;
        branch_units.push_back(new_branch_unit);
    }

    for (int m=0; m < MEM_UNITS; m++)
    {
        MemoryUnit new_mem_unit;
        mem_units.push_back(new_mem_unit);
    }

    for (int i = 0; i < REORDER_BUFFER_SIZE; i++)  
        reorder_buffer[i] = ROB_entry {-1, NOP, 0, false, true};

    for (int i = 0; i < ALU_RES_STATION_SIZE; i++)  
        alu_reservation_station[i] = RS_entry {NOP, -1, -1, -1, -1, 0, 0, 0, -1, true};
    for (int i = 0; i < MEM_RES_STATION_SIZE; i++)  
        mem_reservation_station[i] = RS_entry {NOP, -1, -1, -1, -1, 0, 0, 0, -1, true};
    for (int i = 0; i < BRANCH_RES_STATION_SIZE; i++)  
        branch_reservation_station[i] = RS_entry {NOP, -1, -1, -1, -1, 0, 0, 0, -1, true};
}

// Processor Destructor
Processor::~Processor()  
{
}

ostream& operator<<(std::ostream& out, OPERATION op) {
    switch (op) {
        case LI: out << "LI"; break;
        case LI_F: out << "LI_F"; break;
        case EXIT: out << "EXIT"; break;
        case J: out << "J"; break;
        case RETURN: out << "RETURN"; break;
        case BEQ: out << "BEQ"; break;
        case BLT: out << "BLT"; break;
        case ADD: out << "ADD"; break;        
        case ADD_F: out << "ADD_F"; break;
        case ADDI: out << "ADDI"; break;        
        case ADDI_F: out << "ADDI_F"; break;
        case SUB: out << "SUB"; break;
        case SUBI: out << "SUBI"; break;
        case MUL: out << "MUL"; break;
        case MULI: out << "MULI"; break;
        case MULI_F: out << "MULI_F"; break;
        case DIVI: out << "DIVI"; break;
        case DIVI_F: out << "DIVI_F"; break;
        case AND: out << "AND"; break;
        case OR: out << "OR"; break;
        case SLL: out << "SLL"; break;
        case SRL: out << "SRL"; break;
        case MV: out << "MV"; break;
        case LW: out << "LW"; break;
        case LW_F: out << "LW_F"; break;
        case LA: out << "LA"; break;
        case SW: out << "SW"; break;
        case SW_F: out << "SW_F"; break;
        case NOP: out << "NOP"; break;
    }
    return out;
}

ostream& operator<<(std::ostream& out, BRANCH_STATE bs) {
    switch (bs) {
        case STRONGLY_NOT_TAKEN: out << "STRONGLY_NOT_TAKEN"; break;
        case WEAKLY_NOT_TAKEN: out << "WEAKLY_NOT_TAKEN"; break;
        case WEAKLY_TAKEN: out << "WEAKLY_TAKEN"; break;
        case STRONGLY_TAKEN: out << "STRONGLY_TAKEN"; break;
    }
    return out;
}

template< typename T >
std::string int_to_hex( T i )
{
  std::stringstream stream;
  stream << "0x" 
         << std::setfill ('0') << std::setw(sizeof(T)) 
         << std::hex << i;
  return stream.str();
}

// Increment the program counter
void Processor::incrementPC()  
{
    PC++;
}

// Increment the number of processor cycles
void Processor::incrementCycles()  
{
    cycles++;
}

void Processor::incrementROBCommit()
{
    ROB_commit_pointer = (ROB_commit_pointer + 1) % REORDER_BUFFER_SIZE;
}

void Processor::incrementROBIssue()
{
    ROB_issue_pointer = (ROB_issue_pointer + 1) % REORDER_BUFFER_SIZE;
    currently_issued_instructions++;
}

void Processor:: update_branch_state(int branch_state)
{   
    if (BRANCH_PREDICTOR < 2)  return;
    if (branch_state == 1)
        current_branch_state = (BRANCH_STATE)min((int)current_branch_state + 1, 3);
    else 
        current_branch_state = (BRANCH_STATE)max((int)current_branch_state - 1, 0);
}

void Processor::addInstruction(string line)  
{
    // Instantiate Instruction object, and store in instructions list.
    Instruction new_instruction(line, instructions.size());
    instructions.push_back(new_instruction);
    // cout << instructions.back().to_string() << endl;
}

void Processor::printRSEntry(RS_entry &rs_entry) 
{
    cout << setw(7) << right << rs_entry.op  << " | " << setw(9) << right << rs_entry.rob_dst 
                                                << " | " << setw(6) << right << rs_entry.rob_op0_dependency 
                                                << " | " << setw(6) << right << rs_entry.rob_op1_dependency
                                                << " | " << setw(6) << right << rs_entry.rob_op2_dependency
                                                << " | " << setw(6) << right << rs_entry.val0 
                                                << " | " << setw(6) << right << rs_entry.val1 
                                                << " | " << setw(6) << right << rs_entry.val2 
                                                << " | " << setw(5) << right << (rs_entry.is_empty ? "yes" : "no") << endl;
}

// Function takes in string and parses function name, and then creates pointer to the first instruction 
void Processor::addFunction(string line) 
{
    int start_index = 0;
    int current_index = 0;
    string fn_name;

    // Colon immediately follows the function name
    while (current_index < line.size() && line.at(current_index) != ':')
    {
        current_index++;
    }

    // Store name, and then add <function name, pointer to first instruction> to the map
    fn_name = line.substr(start_index, current_index - start_index);
    fn_map.insert(pair<string, int>(fn_name, instructions.size()));
    fn_map_reverse.insert(pair<int, string>(instructions.size(), fn_name));
}

/// Function takes in string and parses variable
void Processor::addVariable(string line)
{
    int start_index = 1;
    int current_index = 1;
    string var_name;
    int value;
    // Split string around white space
    while (current_index < line.size() && line.at(current_index) != ' ')  
    {
        current_index++;
    }
    // Store variable label
    var_name = line.substr(start_index, current_index - start_index);
    current_index++;
    // Store variable value
    value = stoi(line.substr(current_index));

    // Put value in memory, and store <label, pointer to mem> in HashMap
    main_memory[free_mem_pointer] = value;
    var_map.insert(pair<string, int>(var_name, free_mem_pointer++));

}

void Processor::addArray(string line)  
{
    int start_index = 1;
    int current_index = 1;
    string arr_name;
    int length;

    // Split string around white space and store variable name
    while (current_index < line.size() && line.at(current_index) != ' ')
        current_index++;
    arr_name = line.substr(start_index, current_index - start_index);

    // Split string around white space and store variable length
    current_index++;
    start_index = current_index;
    while (current_index < line.size() && line.at(current_index) != ' ')
        current_index++;
    length = stoi(line.substr(start_index, current_index - start_index));

    // Split string around white space and store array values
    int *arr_pointer = &main_memory[free_mem_pointer];
    for (int i = 0; i < length; i++)
    {
        current_index++;
        start_index = current_index;
        while (current_index < line.size() && line.at(current_index) != ' ')
            current_index++;
        if (arr_name[0] == 'f')  
        {
            float tmp = stof(line.substr(start_index, current_index - start_index));
            memcpy(&main_memory[free_mem_pointer++], &tmp, sizeof(float));
        }
        else 
        {
            main_memory[free_mem_pointer++] = stoi(line.substr(start_index, current_index - start_index));
        }
    }
    var_map.insert(pair<string, int>(arr_name, free_mem_pointer - length));
}

// Simulate the program on the processor
void Processor::run_program()  
{   
    // Create variable to store current instruction, set program counter to point at main function
    Instruction current_instruction;
    PC = fn_map.at("main");

    output_image("before.pgm", 16, 16, &main_memory[0]);


    auto start = std::chrono::system_clock::now();

    // While register exit pin is 0
    while ( register_file[31] != -1 )  
    {

        #ifdef PIPELINED
            #ifdef SUPERSCALAR
                // update_instructions();
                decode_unit->get_instructions(*fetch_unit);
                // cout << "PASSED DECODE" << endl;
                fetch_unit->get_instructions(this);
                // cout << "PASSED FETCH" << endl;

                #ifdef DEBUG
                    // if (cycles > 0)
                    // {
                        debug_processor();
                        getchar();
                    // }
                #endif

                // for (int i = 0; i < mem_reservation_station[i])

                commit();
                // cout << "PASSED commit" << endl;

                for (int u = 0; u < BRANCH_UNITS; u++)
                    branch_units.at(u).execute(this);
                // cout << "PASSED BRANCH" << endl;
                for (int u = 0; u < MEM_UNITS; u++)
                    mem_units.at(u).execute(this);
                // cout << "PASSED MEM" << endl;
                for (int u = 0; u < ALU_UNITS; u++)
                    alu_units.at(u).execute(this);
                // cout << "PASSED ALU" << endl;


                dispatch();
                // cout << "PASSED dispatch" << endl;

                issue();
                // cout << "PASSED issue" << endl;


                decode_unit->decode(this);


                incrementCycles();

                refresh_flag = false;

            #else
                // update_instructions();
                execute_units.at(0).update_next_instruction(*decode_unit);
                execute_units.at(0).update_current_instruction();
                decode_unit->update_next_instruction(*fetch_unit);
                decode_unit->update_current_instruction();
                fetch_unit->update_next_instruction(this);
                fetch_unit->update_current_instruction();
                incrementPC();

                #ifdef DEBUG
                    debug_processor();
                    getchar();
                #endif

                

                fetch_unit->fetch(this);
                // cout << "PASSED FETCH" << endl;
                decode_unit->decode(this);
                // cout << "PASSED DECODE" << endl;
                execute_units.at(0).execute(this);
                // cout << "PASSED EXECUTE" << endl;


                incrementCycles();
                refresh_flag = false;
            #endif
        #else
            // cout << "REACHED FETCH" << endl;
            fetch_unit->update_next_instruction(this);
            incrementPC();
            fetch_unit->update_current_instruction();
            #ifdef DEBUG
                debug_processor();
                getchar();
            #endif

            fetch_unit->fetch(this);
            incrementCycles();

            // cout << "REACHED DECODE" << endl;

            decode_unit->update_next_instruction(*fetch_unit);
            decode_unit->update_current_instruction();
            fetch_unit->is_empty = true;
            #ifdef DEBUG
                debug_processor();
                getchar();
            #endif
            decode_unit->decode(this);
            incrementCycles();

            // cout << "REACHED EXECUTE" << endl;

            execute_units.at(0).update_next_instruction(*decode_unit);
            execute_units.at(0).update_current_instruction();
            decode_unit->is_empty = true;

            #ifdef DEBUG
                debug_processor();
                getchar();
            #endif
            execute_units.at(0).execute(this);
            execute_units.at(0).is_empty = true;

            incrementCycles();

        #endif 
    }

    auto end = std::chrono::system_clock::now();

    std::chrono::duration<double> elapsed_seconds = end - start;
    std::time_t end_time = std::chrono::system_clock::to_time_t(end);

    // Print prcoessor state and exit
    #ifdef DEBUG
        PC = -1;
        debug_processor();
    #endif




    

    #ifdef PRINT_STATS
         printf("################################################################");
        printf(" Simulator Config,Value\n");
        #ifdef PIPELINED
            #ifdef SUPERSCALAR
                printf("Simulator Type,SUPERSCALAR\n");
                printf("Fetch Instructions per Cycle,%d\n",  FETCH_INSTR_PER_CYCLE);
                printf("Decode Instructions per Cycle,%d\n", DECODE_INSTR_PER_CYCLE);
                printf("Issue Instructions per Cycle,%d\n", ISSUE_INSTR_PER_CYCLE);
                printf("Commit Instructions per Cycle,%d\n", COMMIT_INSTR_PER_CYCLE);
                printf("Load/Store Units,%d\n", MEM_UNITS);
                printf("Load/Store Reservation Station Size,%d\n", MEM_RES_STATION_SIZE);
                printf("Branch Units,%d\n", BRANCH_UNITS);
                printf("Branch Reservation Station Size,%d\n", BRANCH_RES_STATION_SIZE);
                printf("ALU Units,%d\n", ALU_UNITS);
                printf("ALU Reservation Station Size,%d\n", ALU_RES_STATION_SIZE);
                printf("Reorder Buffer Size,%d\n", REORDER_BUFFER_SIZE);
                printf("Branch Predictor,%d\n", BRANCH_PREDICTOR);
                printf("Time for Memory Access,%d\n", MEM_ACCESS_TIME);
            #else
                printf("Simulator Type,PIPELINED\n");
            #endif
        #else
            printf("Simulator Type,SERIAL\n");
        #endif
        printf("################################################################");
        printf(" Analysis Field,Value\n");
        // printf("Time Elapsed,%.2f\n", elapsed_seconds.count());
        printf("Executed Instructions,%d\n", executed_instructions);
        printf("Total Cycles,%.d\n", cycles);
        printf("Instructions per Cycle,%.2f\n", (float)(executed_instructions) / (float)(cycles));
        // printf("Instructions per Second,%.2f\n", (float)(executed_instructions) / (float)(elapsed_seconds.count()));
        // printf("Cycles spent Waiting for Memory Access,%d\n", cycles_waiting_for_memory);
        printf("Proportion of Program spent Waiting for Memory Access,%.2f\n", (float)(cycles_waiting_for_memory)/(float)cycles);
        printf("Branch Prediction Accuracy,%.2f\n", 1.f - (float)(branches_mispredicts)/(float)executed_branches);
        printf("################################################################ Field, ProgramResult");
        cout << "\nProgram Result," << register_file[16] << endl;

        cout << "\nMain Memory : \n" << endl;
        for (int j = 0; j < 3; j++)  {
            for (int i = 0; i < 20; i++)  {
                if (j*10+i < 10)  cout << " ";
                cout << j * 20 + i << ": " << main_memory[j * 20 + i] << " ";
            }
            cout << endl;
        }
        printf("################################################################\n");

    #else
        cout << register_file[16]; 
    #endif

    output_image("after.pgm", 16, 16, &main_memory[0]);
}

void Processor::update_instructions()  
{
    fetch_unit->update_next_instruction(this);
    incrementPC();
    decode_unit->update_next_instruction(*fetch_unit);


    #ifdef SUPERSCALAR
        // for (int u = 0; u < BRANCH_UNITS; u++)  
        //     branch_units.at(u).update_next_instruction(this);

        // for (int u = 0; u < MEM_UNITS; u++)  
        //     mem_units.at(u).update_next_instruction(this);

        // for (int u = 0; u < ALU_UNITS; u++)  
        //     alu_units.at(u).update_next_instruction(this);
    #else
        for (int u = 0; u < EXECUTE_UNITS; u++)  
            execute_units.at(u).update_next_instruction(*decode_unit);
    #endif


    fetch_unit->update_current_instruction();
    decode_unit->update_current_instruction();

    #ifdef SUPERSCALAR
        // for (int u = 0; u < BRANCH_UNITS; u++)  
        //     branch_units.at(u).update_current_instruction();

        // for (int u = 0; u < MEM_UNITS; u++)  
        //     mem_units.at(u).update_current_instruction();

        // for (int u = 0; u < ALU_UNITS; u++)  
        //     alu_units.at(u).update_current_instruction();
    #else
        for (int u = 0; u < EXECUTE_UNITS; u++)  
            execute_units.at(u).update_current_instruction();
    #endif
}

void Processor::issue()
{
    for (int i = 0; (i < ISSUE_INSTR_PER_CYCLE) && (currently_issued_instructions < REORDER_BUFFER_SIZE); i++)
    {
        if (instruction_queue.size() == 0)  return;
        bool res_station_full = true;
        switch (string_to_op_map[instruction_queue.front().opcode]) 
        {
            case J: case RETURN:
                instruction_queue.erase(instruction_queue.begin());
                break;
            case BEQ: case BLT: case EXIT:
                for (int i = 0; i < BRANCH_RES_STATION_SIZE; i++)
                    res_station_full &= !branch_reservation_station[i].is_empty;
                if (res_station_full)  return;
                for (int i = 0; i < BRANCH_RES_STATION_SIZE; i++)
                {
                    if (branch_reservation_station[i].is_empty)
                    {
                        
                        switch (string_to_op_map[instruction_queue.front().opcode]) 
                        {
                            case J: case RETURN:
                                break;
                            case BEQ: case BLT:
                                reorder_buffer[ROB_issue_pointer] = ROB_entry {
                                    // If branch taken, store branch not taken. If branch not taken, store taken.
                                    (instruction_queue.front().branch_taken == 1) ? instruction_queue.front().PC + 1 : instruction_queue.front().PC + stoi(instruction_queue.front().operand2), // BRANCH LOC IF NOT TAKEN
                                    string_to_op_map[instruction_queue.front().opcode],
                                    0, 
                                    false, // Marks entry as done
                                    false // Marks entry as not empty
                                };
                                branch_reservation_station[i].op = string_to_op_map.at(instruction_queue.front().opcode);
                                branch_reservation_station[i].rob_dst = ROB_issue_pointer;
                                branch_reservation_station[i].rob_op0_dependency = register_alias_table[register_map.at(instruction_queue.front().operand0)];
                                branch_reservation_station[i].rob_op1_dependency = register_alias_table[register_map.at(instruction_queue.front().operand1)];
                                branch_reservation_station[i].val2 = instruction_queue.front().branch_taken;
                                if(branch_reservation_station[i].rob_op0_dependency == -1)
                                    memcpy(&branch_reservation_station[i].val0, &register_file[register_map.at(instruction_queue.front().operand0)], sizeof(int32_t));
                                if(branch_reservation_station[i].rob_op1_dependency == -1)
                                    memcpy(&branch_reservation_station[i].val1, &register_file[register_map.at(instruction_queue.front().operand1)], sizeof(int32_t));
                                if (branch_reservation_station[i].rob_op0_dependency != -1 && reorder_buffer[branch_reservation_station[i].rob_op0_dependency].done)
                                {
                                    memcpy(&branch_reservation_station[i].val0, &reorder_buffer[branch_reservation_station[i].rob_op0_dependency].value, sizeof(int32_t));
                                    branch_reservation_station[i].rob_op0_dependency = -1;
                                }
                                if (branch_reservation_station[i].rob_op1_dependency != -1 && reorder_buffer[branch_reservation_station[i].rob_op1_dependency].done)
                                {
                                    memcpy(&branch_reservation_station[i].val1, &reorder_buffer[branch_reservation_station[i].rob_op1_dependency].value, sizeof(int32_t));
                                    branch_reservation_station[i].rob_op1_dependency = -1;
                                }
                                break;
                            case EXIT:
                                reorder_buffer[ROB_issue_pointer] = ROB_entry {
                                    31, // Pointer to dst register
                                    string_to_op_map[instruction_queue.front().opcode],
                                    0, // Value - not ready so just placeholder
                                    false, // Marks entry as done
                                    false // Marks entry as not empty
                                };
                                branch_reservation_station[i].op = string_to_op_map.at(instruction_queue.front().opcode);
                                branch_reservation_station[i].rob_dst = ROB_issue_pointer;
                                break;
                            default:
                                break;
                        }
                        branch_reservation_station[i].is_empty = false; 
                        incrementROBIssue();
                        instruction_queue.erase(instruction_queue.begin());
                        break;
                    }
                }
                break;
            case LW: case LA: case LI: case SW: case LW_F: case SW_F: case MV:
                for (int i = 0; i < MEM_RES_STATION_SIZE; i++)
                    res_station_full &= !mem_reservation_station[i].is_empty;
                if (res_station_full)  return;
                for (int i = 0; i < MEM_RES_STATION_SIZE; i++)
                {
                    if (mem_reservation_station[i].is_empty)
                    {
                        reorder_buffer[ROB_issue_pointer] = ROB_entry {
                            register_map.at(instruction_queue.front().operand0), // Pointer to dst register
                            string_to_op_map[instruction_queue.front().opcode],
                            0, // Value - not ready so just placeholder
                            false, // Marks entry as done
                            false // Marks entry as not empty
                        };
                        mem_reservation_station[i] = {NOP, -1, -1, -1, -1, 0, 0, 0, -1, true};
                        mem_reservation_station[i].op = string_to_op_map.at(instruction_queue.front().opcode);
                        mem_reservation_station[i].rob_dst = ROB_issue_pointer;
                        switch (string_to_op_map[instruction_queue.front().opcode]) 
                        {
                            case LA:
                                mem_reservation_station[i].val0 = var_map.at(instruction_queue.front().operand1);
                                register_alias_table[register_map.at(instruction_queue.front().operand0)] = ROB_issue_pointer;
                                break;
                            case MV:
                                mem_reservation_station[i].rob_op0_dependency = register_alias_table[register_map.at(instruction_queue.front().operand1)];
                                if(mem_reservation_station[i].rob_op0_dependency == -1)
                                    memcpy(&mem_reservation_station[i].val0, &register_file[register_map.at(instruction_queue.front().operand1)], sizeof(int32_t));
                                if (mem_reservation_station[i].rob_op0_dependency != -1 && reorder_buffer[mem_reservation_station[i].rob_op0_dependency].done)
                                {
                                    memcpy(&mem_reservation_station[i].val0, &reorder_buffer[mem_reservation_station[i].rob_op0_dependency].value, sizeof(int32_t));
                                    mem_reservation_station[i].rob_op0_dependency = -1;
                                }
                                register_alias_table[register_map.at(instruction_queue.front().operand0)] = ROB_issue_pointer;
                                break;
                            case LI:
                            {
                                int itmp2 = stoi(instruction_queue.front().operand1);
                                memcpy(&mem_reservation_station[i].val0, &itmp2, sizeof(int32_t));
                                register_alias_table[register_map.at(instruction_queue.front().operand0)] = ROB_issue_pointer;
                                break;
                            }
                            case LI_F:
                            {
                                float ftmp2 = stof(instruction_queue.front().operand1);
                                memcpy(&mem_reservation_station[i].val0, &ftmp2, sizeof(float));
                                register_alias_table[register_map.at(instruction_queue.front().operand0)] = ROB_issue_pointer;
                                break;
                            }
                            case SW: case SW_F:
                                reorder_buffer[ROB_issue_pointer].value = -1;
                                reorder_buffer[ROB_issue_pointer].p_register_dst = -1;
                                mem_reservation_station[i].rob_op0_dependency = register_alias_table[register_map.at(instruction_queue.front().operand0)];
                                mem_reservation_station[i].rob_op1_dependency = register_alias_table[register_map.at(instruction_queue.front().operand1)];
                                mem_reservation_station[i].rob_op2_dependency = register_alias_table[register_map.at(instruction_queue.front().operand2)];
                                if(mem_reservation_station[i].rob_op0_dependency == -1)
                                    memcpy(&mem_reservation_station[i].val0, &register_file[register_map.at(instruction_queue.front().operand0)], sizeof(int32_t));
                                if(mem_reservation_station[i].rob_op1_dependency == -1)
                                    memcpy(&mem_reservation_station[i].val1, &register_file[register_map.at(instruction_queue.front().operand1)], sizeof(int32_t));
                                if(mem_reservation_station[i].rob_op2_dependency == -1)
                                    memcpy(&mem_reservation_station[i].val2, &register_file[register_map.at(instruction_queue.front().operand2)], sizeof(int32_t));
                                if (mem_reservation_station[i].rob_op0_dependency != -1 && reorder_buffer[mem_reservation_station[i].rob_op0_dependency].done)
                                {
                                    memcpy(&mem_reservation_station[i].val0, &reorder_buffer[mem_reservation_station[i].rob_op0_dependency].value, sizeof(int32_t));
                                    mem_reservation_station[i].rob_op0_dependency = -1;
                                }
                                if (mem_reservation_station[i].rob_op1_dependency != -1 && reorder_buffer[mem_reservation_station[i].rob_op1_dependency].done)
                                {
                                    memcpy(&mem_reservation_station[i].val1, &reorder_buffer[mem_reservation_station[i].rob_op1_dependency].value, sizeof(int32_t));
                                    mem_reservation_station[i].rob_op1_dependency = -1;
                                }
                                if (mem_reservation_station[i].rob_op2_dependency != -1 && reorder_buffer[mem_reservation_station[i].rob_op2_dependency].done)
                                {
                                    memcpy(&mem_reservation_station[i].val2, &reorder_buffer[mem_reservation_station[i].rob_op2_dependency].value, sizeof(int32_t));
                                    mem_reservation_station[i].rob_op2_dependency = -1;
                                }
                                break;
                            default:
                                mem_reservation_station[i].rob_op0_dependency = register_alias_table[register_map.at(instruction_queue.front().operand1)];
                                mem_reservation_station[i].rob_op1_dependency = register_alias_table[register_map.at(instruction_queue.front().operand2)];
                                if(mem_reservation_station[i].rob_op0_dependency == -1)
                                    memcpy(&mem_reservation_station[i].val0, &register_file[register_map.at(instruction_queue.front().operand1)], sizeof(int32_t));
                                if(mem_reservation_station[i].rob_op1_dependency == -1)
                                    memcpy(&mem_reservation_station[i].val1, &register_file[register_map.at(instruction_queue.front().operand2)], sizeof(int32_t));
                                register_alias_table[register_map.at(instruction_queue.front().operand0)] = ROB_issue_pointer;
                                if (mem_reservation_station[i].rob_op0_dependency != -1 && reorder_buffer[mem_reservation_station[i].rob_op0_dependency].done)
                                {
                                    memcpy(&mem_reservation_station[i].val0, &reorder_buffer[mem_reservation_station[i].rob_op0_dependency].value, sizeof(int32_t));
                                    mem_reservation_station[i].rob_op0_dependency = -1;
                                }
                                if (mem_reservation_station[i].rob_op1_dependency != -1 && reorder_buffer[mem_reservation_station[i].rob_op1_dependency].done)
                                {
                                    memcpy(&mem_reservation_station[i].val1, &reorder_buffer[mem_reservation_station[i].rob_op1_dependency].value, sizeof(int32_t));
                                    mem_reservation_station[i].rob_op1_dependency = -1;
                                }
                                break;
                        }
                        mem_reservation_station[i].is_empty = false; 
                        incrementROBIssue();
                        instruction_queue.erase(instruction_queue.begin());
                        break;
                    }
                }
                break;
            case NOP:
                instruction_queue.erase(instruction_queue.begin());
                break;
            default:
                for (int i = 0; i < ALU_RES_STATION_SIZE; i++)
                    res_station_full &= !alu_reservation_station[i].is_empty;
                if (res_station_full)  return;
                for (int i = 0; i < ALU_RES_STATION_SIZE; i++)
                {
                    if (alu_reservation_station[i].is_empty)
                    {
                        reorder_buffer[ROB_issue_pointer] = ROB_entry {
                            register_map.at(instruction_queue.front().operand0), // Pointer to dst register
                            string_to_op_map[instruction_queue.front().opcode],
                            0, // Value - not ready so just placeholder
                            false, // Marks entry as done
                            false // Marks entry as not empty
                        };
                        alu_reservation_station[i].op = string_to_op_map.at(instruction_queue.front().opcode);
                        alu_reservation_station[i].rob_dst = ROB_issue_pointer;
                        switch (string_to_op_map[instruction_queue.front().opcode]) 
                        {
                            case ADDI: case SUBI: case MULI:
                                int32_t itmp;
                                alu_reservation_station[i].rob_op0_dependency = register_alias_table[register_map.at(instruction_queue.front().operand1)];
                                if(alu_reservation_station[i].rob_op0_dependency == -1)
                                    memcpy(&alu_reservation_station[i].val0, &register_file[register_map.at(instruction_queue.front().operand1)], sizeof(int32_t));
                                if (alu_reservation_station[i].rob_op0_dependency != -1 && reorder_buffer[alu_reservation_station[i].rob_op0_dependency].done)
                                {
                                    memcpy(&alu_reservation_station[i].val0, &reorder_buffer[alu_reservation_station[i].rob_op0_dependency].value, sizeof(int32_t));
                                    alu_reservation_station[i].rob_op0_dependency = -1;
                                }
                                itmp = stoi(instruction_queue.front().operand2);
                                memcpy(&alu_reservation_station[i].val1, &itmp, sizeof(int32_t));
                                break;
                            case ADDI_F: case MULI_F: case DIVI_F:
                                float ftmp;
                                alu_reservation_station[i].rob_op0_dependency = register_alias_table[register_map.at(instruction_queue.front().operand1)];
                                if(alu_reservation_station[i].rob_op0_dependency == -1)
                                    memcpy(&alu_reservation_station[i].val0, &register_file[register_map.at(instruction_queue.front().operand1)], sizeof(int32_t));
                                if (alu_reservation_station[i].rob_op0_dependency != -1 && reorder_buffer[alu_reservation_station[i].rob_op0_dependency].done)
                                {
                                    memcpy(&alu_reservation_station[i].val0, &reorder_buffer[alu_reservation_station[i].rob_op0_dependency].value, sizeof(int32_t));
                                    alu_reservation_station[i].rob_op0_dependency = -1;
                                }
                                ftmp = stof(instruction_queue.front().operand2);
                                memcpy(&alu_reservation_station[i].val1, &ftmp, sizeof(float));
                                break;
                            default:
                                alu_reservation_station[i].rob_op0_dependency = register_alias_table[register_map.at(instruction_queue.front().operand1)];
                                alu_reservation_station[i].rob_op1_dependency = register_alias_table[register_map.at(instruction_queue.front().operand2)];
                                if(alu_reservation_station[i].rob_op0_dependency == -1)
                                    memcpy(&alu_reservation_station[i].val0, &register_file[register_map.at(instruction_queue.front().operand1)], sizeof(int32_t));
                                if(alu_reservation_station[i].rob_op1_dependency == -1)
                                    memcpy(&alu_reservation_station[i].val1, &register_file[register_map.at(instruction_queue.front().operand2)], sizeof(int32_t));
                                if (alu_reservation_station[i].rob_op0_dependency != -1 && reorder_buffer[alu_reservation_station[i].rob_op0_dependency].done)
                                {
                                    memcpy(&alu_reservation_station[i].val0, &reorder_buffer[alu_reservation_station[i].rob_op0_dependency].value, sizeof(int32_t));
                                    alu_reservation_station[i].rob_op0_dependency = -1;
                                }
                                if (alu_reservation_station[i].rob_op1_dependency != -1 && reorder_buffer[alu_reservation_station[i].rob_op1_dependency].done)
                                {
                                    memcpy(&alu_reservation_station[i].val1, &reorder_buffer[alu_reservation_station[i].rob_op1_dependency].value, sizeof(int32_t));
                                    alu_reservation_station[i].rob_op1_dependency = -1;
                                }
                                break;
                        }
                        alu_reservation_station[i].is_empty = false; 
                        register_alias_table[register_map.at(instruction_queue.front().operand0)] = ROB_issue_pointer;
                        incrementROBIssue();
                        instruction_queue.erase(instruction_queue.begin());
                        break;
                    }
                }
                break;
        }

    }
}

void Processor::dispatch()
{
    // Check if any branch units are ready
    for (int i = 0; i < BRANCH_UNITS; i++)
    {
        if (branch_units.at(i).is_empty)
        {
            for (int r = 0; r < BRANCH_RES_STATION_SIZE; r++)
            {
                RS_entry *rs_entry = &branch_reservation_station[r];
                if (!rs_entry->is_empty && rs_entry->rob_op0_dependency == -1 && rs_entry->rob_op1_dependency == -1 )
                {
                    branch_units.at(i).update_current_instruction(*rs_entry);
                    branch_reservation_station[r] = RS_entry {NOP, -1, -1, -1, -1, 0, 0, 0, -1, true};
                    break;
                }
            }
        }
    }
    // Check if any memory units are ready
    for (int i = 0; i < MEM_UNITS; i++)
    {
        if (mem_units.at(i).is_empty)
        {
            for (int r = 0; r < MEM_RES_STATION_SIZE; r++)
            {
                RS_entry *rs_entry = &mem_reservation_station[r];
                if (!rs_entry->is_empty && rs_entry->rob_op0_dependency == -1 && rs_entry->rob_op1_dependency == -1 && rs_entry->rob_op2_dependency == -1)
                {
                    mem_units.at(i).update_current_instruction(*rs_entry);
                    mem_reservation_station[r] = RS_entry {NOP, -1, -1, -1, -1, 0, 0, 0, -1, true};
                    break;
                }
            }
        }
    }
    // Check if any ALU units are ready
    for (int i = 0; i < ALU_UNITS; i++)
    {
        if (alu_units.at(i).is_empty)
        {
            for (int r = 0; r < ALU_RES_STATION_SIZE; r++)
            {
                RS_entry *rs_entry = &alu_reservation_station[r];
                if (!rs_entry->is_empty && rs_entry->rob_op0_dependency == -1 && rs_entry->rob_op1_dependency == -1 )
                {
                    alu_units.at(i).update_current_instruction(*rs_entry);
                    alu_reservation_station[r] = RS_entry {NOP, -1, -1, -1, -1, 0, 0, 0, -1, true};
                    break;
                }
            }
        }
    }

}

void Processor::commit()
{
    for (int i = 0; i < COMMIT_INSTR_PER_CYCLE; i++)
    {
        if (reorder_buffer[ROB_commit_pointer].done)
        {
            switch (reorder_buffer[ROB_commit_pointer].op)
            {
                case J: case NOP: case RETURN:
                    // REFRESH PIPELINE?? EXIT??
                    currently_issued_instructions--;
                    break;
                case BEQ: case BLT:
                    executed_branches++;
                    if (reorder_buffer[ROB_commit_pointer].value == 0)
                    {
                        /// Branch mispredict
                        branches_mispredicts++;

                        for (int r = 0; r < 64; r++)
                            register_alias_table[r] = -1;
                        // for (int r=ROB_commit_pointer+1; r < ROB_issue_pointer; r++)
                        //     reorder_buffer[r] = ROB_entry {-1, NOP, 0, false, true};
                        int index = ((ROB_commit_pointer + 1 )% REORDER_BUFFER_SIZE);
                        if (ROB_issue_pointer != (((ROB_commit_pointer+1) % REORDER_BUFFER_SIZE)))
                        {
                            while (index != ROB_issue_pointer)
                            {
                                reorder_buffer[index] = ROB_entry {-1, NOP, 0, false, true};
                                index = (index + 1) % REORDER_BUFFER_SIZE;
                            }
                        }
                        ROB_issue_pointer = ROB_commit_pointer;
                        currently_issued_instructions = 0;
                        #ifdef DEBUG
                            cout << "BRANCH MISPREDICT -> REFRESH PIPELINE" << endl;
                        #endif

                        incrementROBIssue();
                        PC = reorder_buffer[ROB_commit_pointer].p_register_dst;
                        fetch_unit->instructions.clear();
                        decode_unit->instructions.clear();
                        for (int u = 0; u < ALU_UNITS; u++)
                            alu_units.at(u).is_empty = true;
                        for (int u = 0; u < BRANCH_UNITS; u++)
                            branch_units.at(u).is_empty = true;
                        for (int u = 0; u < MEM_UNITS; u++)
                            mem_units.at(u).is_empty = true;
                            
                        instruction_queue.clear();

                        for (int r = 0; r < ALU_RES_STATION_SIZE; r++)
                            alu_reservation_station[r] = RS_entry {NOP, -1, -1, -1, -1, 0, 0, 0, -1, true};
                        for (int r = 0; r < BRANCH_RES_STATION_SIZE; r++)
                            branch_reservation_station[r] = RS_entry {NOP, -1, -1, -1, -1, 0, 0, 0, -1, true};
                        for (int r = 0; r < MEM_RES_STATION_SIZE; r++)
                            mem_reservation_station[r] = RS_entry {NOP, -1, -1, -1, -1, 0, 0, 0, -1, true};
                    }
                    else
                    {
                        currently_issued_instructions--;
                    }
                    break;
                case SW_F: case SW:
                    // Do nothing, no destination register
                    currently_issued_instructions--;
                default:
                    register_file[reorder_buffer[ROB_commit_pointer].p_register_dst] = reorder_buffer[ROB_commit_pointer].value;
                    if (register_alias_table[reorder_buffer[ROB_commit_pointer].p_register_dst] == ROB_commit_pointer)
                        register_alias_table[reorder_buffer[ROB_commit_pointer].p_register_dst] = -1;
                    currently_issued_instructions--;
                    break;
            }
            incrementROBCommit();
        }
        else
        {
            return;
        }
    }
}

void Processor::refresh_pipeline() 
{
    branches_mispredicts++;
    Instruction empty_instruction;
    fetch_unit->current_instruction   = empty_instruction;
    fetch_unit->next_instruction      = empty_instruction;
    fetch_unit->is_empty   = true;

    decode_unit->current_instruction  = empty_instruction;
    decode_unit->next_instruction     = empty_instruction;
    decode_unit->is_empty  = true;

    for (int u = 0; u < EXECUTE_UNITS; u++)  
    {
        execute_units.at(u).current_instruction = empty_instruction;
        execute_units.at(u).next_instruction    = empty_instruction;
        execute_units.at(u).is_empty = true;
    }

    for (int u = 0; u < BRANCH_UNITS; u++)  
    {
        branch_units.at(u).current_instruction  = empty_instruction;
        branch_units.at(u).next_instruction     = empty_instruction;
        branch_units.at(u).is_empty  = true;
    }


    for (int u = 0; u < ALU_UNITS; u++)  
    {
        alu_units.at(u).current_instruction     = empty_instruction;
        alu_units.at(u).next_instruction        = empty_instruction;
        alu_units.at(u).is_empty     = true;
    }


    for (int u = 0; u < MEM_UNITS; u++)  
    {
        mem_units.at(u).current_instruction     = empty_instruction;
        mem_units.at(u).next_instruction        = empty_instruction;
        mem_units.at(u).is_empty     = true;
    }
    
    // mem_reservation_station.clear();
    // branch_reservation_station.clear();
    // alu_reservation_station.clear();

    refresh_flag = true;
}

void Processor::debug_processor()  
{
    map<string, int>::iterator it;
    map<string, uint32_t>::iterator it1;

    for (int i = 0; i < 10; i++)
        cout << endl << endl;

    cout << "\n\n###################" << endl;
    cout << "PROCESSOR DEBUG LOG: Total Cycles = " << cycles << endl;
    cout << "###################" << endl;

    cout << "\nFunctions : " << endl;

    for (it = fn_map.begin(); it != fn_map.end(); it++)
    {
        cout << it->first << " -> " << it->second << endl;
    }

    cout << "Arrays and Variables : " << endl;

    for (it = var_map.begin(); it != var_map.end(); it++)
    {
        cout << it->first << " -> &Mem + " << it->second << "; ";
    }
    cout << endl;

    cout << "\nInstructions : " << endl;

    for (int i = PC-1; i < min(PC+4, (int)instructions.size()) && i > -1; i++)  {
        if(fn_map_reverse.count(i) > 0)  
            cout << fn_map_reverse.at(i) << ":" << endl;
        cout << "\t";
        cout << i << ". " << instructions.at(i).to_string(); 
        if (i == PC - 1)
            cout << "  <- PC";
        cout << endl;
    }

    cout << "\nRegister Values | FP Register Values : " << endl;
    float tmp;
    for (int j = 0; j < 4; j++)  {
        for (int i = 0; i < 8; i++)  {
            if (j*8+i < 10)  cout << " ";
            cout << j*8+i << ": " << register_file[j*8+i] << " ";
        }
        cout << "  |  ";
        for (int i = 0; i < 8; i++)  {
            if (j*8+i < 10)  cout << " ";
            memcpy(&tmp, &register_file[j*8+i+32], sizeof(float));
            cout << j*8+i << ": " << tmp << " ";
        }
        cout << endl;
    }
    cout << endl;
    

    #ifdef SUPERSCALAR
        for (int i = 0; i < fetch_unit->instructions.size(); i++)
            cout << "Fetch Unit " << i << ": " << fetch_unit->instructions.at(i).to_string() << endl;
        for (int i = 0; i < decode_unit->instructions.size(); i++)
            cout << "Decode Unit " << i << ": " << decode_unit->instructions.at(i).to_string() << endl;
        // cout << "Decode Unit: " << (decode_unit->is_empty  ? "Empty"  : decode_unit->current_instruction.to_string())  << endl;
        cout << "\nInstruction Queue (size=" << instruction_queue.size()<< ") (First 5 elementss): " << endl;
        for (int i = 0; i < min(5, (int)instruction_queue.size()); i++)
            cout << i << ". " << instruction_queue.at(i).to_string() << endl; 

        cout << "\nALU Reservation Station: \t\t\t\t\t\t\t      =====> ";
        for (int i = 0; i < ALU_UNITS; i++)
            alu_units.at(i).print_state_string();
        cout << endl;
        cout << "   OP   |  ROB_DST  |  ROB0  |  ROB1  |  ROB2  |  VAL0  |  VAL1  |  VAL2  |  EMPTY " << endl;

        for (int i = 0; i < ALU_RES_STATION_SIZE; i++)
            if(!alu_reservation_station[i].is_empty)
                printRSEntry(alu_reservation_station[i]);

        cout << "\nBranch Reservation Station: \t\t\t\t\t\t\t      =====> ";
        for (int i = 0; i < BRANCH_UNITS; i++)
            branch_units.at(i).print_state_string();
        cout << endl;

        cout << "   OP   |  ROB_DST  |  ROB0  |  ROB1  |  ROB2  |  VAL0  |  VAL1  |  VAL2  |  EMPTY " << endl;

        for (int i = 0; i < BRANCH_RES_STATION_SIZE; i++)
            if(!branch_reservation_station[i].is_empty)
                printRSEntry(branch_reservation_station[i]);

        cout << "\nMemory Reservation Station: \t\t\t\t\t\t\t      =====> ";
        for (int i = 0; i < MEM_UNITS; i++)
            mem_units.at(i).print_state_string();
        cout << endl;
        cout << "   OP   |  ROB_DST  |  ROB0  |  ROB1  |  ROB2  |  VAL0  |  VAL1  |  VAL2  |  EMPTY " << endl;

        for (int i = 0; i < MEM_RES_STATION_SIZE; i++)
            if(!mem_reservation_station[i].is_empty)
                printRSEntry(mem_reservation_station[i]);

    #endif
    cout << "\nBRANCH PREDICTOR STATE: " << current_branch_state << endl << endl;

    cout << "\nMain Memory : \n" << endl;

    for (int j = 0; j < 2; j++)  {
        for (int i = 0; i < 20; i++)  {
            if (j*10+i < 10)  cout << " ";
            cout << j * 20 + i << ": " << main_memory[j * 20 + i] << " ";
        }
        cout << endl;
    }

    
    #ifdef SUPERSCALAR
        cout << "\nReorder Buffer" << endl;
        cout << "ENTRY  |  OP  |  Reg Dest  |  VAL  |  DONE " << endl;
        int index = ROB_commit_pointer;
        if (ROB_issue_pointer != ROB_commit_pointer)
        {
            while (index != ROB_issue_pointer)
            {
                cout << setw(6) << right << index << " | " << setw(4) << right  << reorder_buffer[index].op
                        << " | " << setw(10) << right  << reorder_buffer[index].p_register_dst
                        << " | " << setw(5) << right  << reorder_buffer[index].value
                        << " | " << setw(3) << right  << reorder_buffer[index].done;
                if (index == ROB_commit_pointer)
                    cout << "  <- ROB Commit Pointer";
                cout << endl;
                index = (index + 1) % REORDER_BUFFER_SIZE;
            }
            cout << "                                          <- ROB Issue Pointer" << endl;
        }

        cout << endl;
        // cout << "ENTRY  |  OP  |  Reg Dest  |  VAL  |  DONE " << endl;

        // for (int index = 0; index < REORDER_BUFFER_SIZE; index++)
        // {
        //     cout << setw(6) << right << index << " | " << setw(4) << right  << reorder_buffer[index].op
        //                 << " | " << setw(10) << right  << reorder_buffer[index].p_register_dst
        //                 << " | " << setw(5) << right  << reorder_buffer[index].value
        //                 << " | " << setw(3) << right  << reorder_buffer[index].done << endl;
        // }
       
    #else
        cout << "\nFetch Unit: "  << (fetch_unit->is_empty  ? "Empty"  : fetch_unit->current_instruction.to_string())  << endl;
        cout << "Decode Unit: "  << (decode_unit->is_empty  ? "Empty"  : decode_unit->current_instruction.to_string())  << endl;
        cout << "Execute Unit: " << (execute_units.at(0).is_empty ? "Empty"  : execute_units.at(0).current_instruction.to_string()) << endl;
    #endif
    
}

// Routine to output the image in pgm grayscale binary image format
void Processor::output_image(char *file_name, const int nx, const int ny, int* image)
{
    // Open output file
    FILE* fp = fopen(file_name, "w");
    if (!fp) {
        fprintf(stderr, "Error: Could not open %s\n", file_name);
        exit(EXIT_FAILURE);
    }

    // Ouptut image header
    fprintf(fp, "P5 %d %d 255\n", nx, ny);

    // Calculate maximum value of image
    // This is used to rescale the values
    // to a range of 0-255 for output
    float maximum = 0.0f;
    for (int j = 0; j < ny; ++j) {
        for (int i = 0; i < nx; ++i) {
            float tmp;
            memcpy(&tmp, &image[j + i * nx], sizeof(float));
            if (tmp > maximum) maximum = tmp;
        }
    }

    // Output image, converting to numbers 0-255
    for (int j = 0; j < ny; ++j) {
        for (int i = 0; i < nx; ++i) {
            float tmp;
            memcpy(&tmp, &image[j + i * nx], sizeof(float));
            fputc((char)(255.0 * tmp / maximum), fp);
        }
    }

    // Close the file
    fclose(fp);
}

