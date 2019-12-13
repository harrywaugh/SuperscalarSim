#include "Processor.h"
#include "FetchUnit.h"
#include "DecodeUnit.h"
#include "DispatchUnit.h"
#include "ExecuteUnit.h"
#include "MemoryUnit.h"
#include "ALU.h"
#include "BranchUnit.h"
#include "CommitUnit.h"

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
}

// Processor Destructor
Processor::~Processor()  
{
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

void Processor::addInstruction(string line)  
{
    // Instantiate Instruction object, and store in instructions list.
    Instruction new_instruction(line, instructions.size());
    instructions.push_back(new_instruction);
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
    while ( registers[31] != -1 )  
    {

        #ifdef PIPELINED
            #ifdef SUPERSCALAR
                update_instructions();

                #ifdef DEBUG
                    debug_processor();
                    getchar();
                #endif
                
                fetch_unit->fetch(this);
                decode_unit->decode(this);
                for (int u = 0; u < BRANCH_UNITS; u++)
                    branch_units.at(u).execute(this);
                for (int u = 0; u < MEM_UNITS; u++)
                    mem_units.at(u).execute(this);
                for (int u = 0; u < ALU_UNITS; u++)
                    alu_units.at(u).execute(this);

                incrementCycles();
                // if (!refresh_flag)  incrementPC();

                refresh_flag = false;

            #else
                update_instructions();

                #ifdef DEBUG
                    debug_processor();
                    getchar();
                #endif

                fetch_unit->fetch(this);
                decode_unit->decode(this);
                execute_units.at(0).execute(this);

                incrementCycles();
                refresh_flag = false;
            #endif
        #else

            fetch_unit->update_next_instruction(this);
            incrementPC();
            fetch_unit->update_current_instruction();
            #ifdef DEBUG
                debug_processor();
                getchar();
            #endif

            fetch_unit->fetch(this);
            incrementCycles();
            fetch_unit->is_empty = true;


            decode_unit->update_next_instruction(*fetch_unit);
            decode_unit->update_current_instruction();
            #ifdef DEBUG
                debug_processor();
                getchar();
            #endif
            decode_unit->decode(this);
            incrementCycles();
            decode_unit->is_empty = true;


            execute_units.at(0).update_next_instruction(*decode_unit);
            execute_units.at(0).update_current_instruction();
            #ifdef DEBUG
                debug_processor();
                getchar();
            #endif
            execute_units.at(0).execute(this);
            incrementCycles();
            execute_units.at(0).is_empty = true;

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
        printf("Time Elapsed: %.2f\n", elapsed_seconds.count());
        printf("Executed Instructions = %d\n", executed_instructions);
        printf("Total Cycles = %.d\n\n", cycles);
        printf("Instructions per Cycle = %.2f\n", (float)(executed_instructions) / (float)(cycles));
        printf("Instructions per Second = %.2f\n\n", (float)(executed_instructions) / (float)(elapsed_seconds.count()));
        printf("Time spent waiting for Memory Access = %d\n\n", cycles_waiting_for_memory);
        printf("Fraction of cycles spent waiting for Memory Access = %.2f\n", (float)(cycles_waiting_for_memory)/(float)cycles);

        cout << "\nProgram Result=" << registers[16] << endl;
    #else
        cout << registers[16]; 
    #endif

    output_image("after.pgm", 16, 16, &main_memory[0]);
}

void Processor::update_instructions()  
{


    fetch_unit->update_next_instruction(this);
    incrementPC();

    decode_unit->update_next_instruction(*fetch_unit);


    #ifdef SUPERSCALAR
        for (int u = 0; u < BRANCH_UNITS; u++)  
            branch_units.at(u).update_next_instruction(this);

        for (int u = 0; u < MEM_UNITS; u++)  
            mem_units.at(u).update_next_instruction(this);

        for (int u = 0; u < ALU_UNITS; u++)  
            alu_units.at(u).update_next_instruction(this);
    #else
        for (int u = 0; u < EXECUTE_UNITS; u++)  
            execute_units.at(u).update_next_instruction(*decode_unit);
    #endif


    fetch_unit->update_current_instruction();
    
    decode_unit->update_current_instruction();
    #ifdef SUPERSCALAR
        for (int u = 0; u < BRANCH_UNITS; u++)  
            branch_units.at(u).update_current_instruction();

        for (int u = 0; u < MEM_UNITS; u++)  
            mem_units.at(u).update_current_instruction();

        for (int u = 0; u < ALU_UNITS; u++)  
            alu_units.at(u).update_current_instruction();
    #else
        for (int u = 0; u < EXECUTE_UNITS; u++)  
            execute_units.at(u).update_current_instruction();
    #endif
}

void Processor::refresh_pipeline() 
{
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
    
    mem_reservation_station.clear();
    branch_reservation_station.clear();
    alu_reservation_station.clear();

    refresh_flag = true;
}

void Processor::debug_processor()  
{
    map<string, int>::iterator it;
    map<string, uint32_t>::iterator it1;

    cout << "\n\n###################" << endl;
    cout << "PROCESSOR DEBUG LOG: Total Cycles = " << cycles << endl;
    cout << "###################" << endl;

    cout << "\nFunctions : \n" << endl;

    for (it = fn_map.begin(); it != fn_map.end(); it++)
    {
        cout << it->first << " -> " << it->second << endl;
    }

    cout << "\nArrays and Variables : \n" << endl;

    for (it = var_map.begin(); it != var_map.end(); it++)
    {
        cout << it->first << " -> &Mem + " << it->second <<  endl;
    }

    cout << "\nInstructions : \n" << endl;

    for (int i = max(PC - 5, 0); i < min(PC+5, (int)instructions.size()); i++)  {
        if(fn_map_reverse.count(i) > 0)  
            cout << fn_map_reverse.at(i) << ":" << endl;
        cout << "\t";
        cout << i << ". " << instructions.at(i).to_string(); 
        if (i == PC - 1)
            cout << "  <- PC";
        cout << endl;
    }
    cout << endl;

    cout << "\nRegister Values : \n" << endl;
    for (int j = 0; j < 4; j++)  {
        for (int i = 0; i < 8; i++)  {
            if (j*8+i < 10)  cout << " ";
            cout << j*8+i << ": " << registers[j*8+i] << " ";
        }
        cout << endl;
    }

    cout << "\nFP Register Values : \n" << endl;
    for (int j = 0; j < 4; j++)  {
        for (int i = 0; i < 8; i++)  {
            if (j*8+i < 10)  cout << " ";
            cout << j*8+i << ": " << fp_register_file[j*8+i] << " ";
        }
        cout << endl;
    }

    #ifdef SUPERSCALAR
        cout << "\nALU Reservation Station: \n" << endl;
        for (int i = 0; i < alu_reservation_station.size(); i++)
            cout << i << ". " << alu_reservation_station.at(i).to_string() << endl;

        cout << "\nBranch Reservation Station: \n" << endl;
        for (int i = 0; i < branch_reservation_station.size(); i++)
            cout << i << ". " << branch_reservation_station.at(i).to_string() << endl;

        cout << "\nLoad/Store Reservation Station: \n" << endl;
        for (int i = 0; i < mem_reservation_station.size(); i++)
            cout << i << ". " << mem_reservation_station.at(i).to_string() << endl;

        

    #endif

    // cout << "\nMain Memory : \n" << endl;

    // for (int j = 0; j < 10; j++)  {
    //     for (int i = 0; i < 10; i++)  {
    //         if (j*10+i < 10)  cout << " ";
    //         cout << j * 10 + i << ": " << main_memory[j * 10 + i] << " ";
    //     }
    //     cout << endl;
    // }

    cout << endl << endl;
    cout << "Fetch Unit: " << (fetch_unit->is_empty   ? "Empty"  : fetch_unit->current_instruction.to_string())  << endl;
    cout << "Decode Unit: " << (decode_unit->is_empty  ? "Empty"  : decode_unit->current_instruction.to_string())  << endl;
    
    #ifdef SUPERSCALAR
        cout << "Branch Unit: " << (branch_units.at(0).is_empty   ? "Empty"  : branch_units.at(0).current_instruction.to_string()) << endl;
        cout << "Memory Unit: " << (mem_units.at(0).is_empty      ? "Empty"  : mem_units.at(0).current_instruction.to_string()) << endl;
        cout << "ALU Unit: "    << (alu_units.at(0).is_empty      ? "Empty"  : alu_units.at(0).current_instruction.to_string()) << endl;
    #else
        cout << "Decode Unit: "  << (decode_unit->is_empty  ? "Empty"  : decode_unit->current_instruction.to_string())  << endl;
        cout << "Execute Unit: " << (execute_units.at(0).is_empty ? "Empty"  : execute_units.at(0).current_instruction.to_string()) << endl;
    #endif
    cout << endl << endl;
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

