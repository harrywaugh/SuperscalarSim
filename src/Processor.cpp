#include "Processor.h"
#include "FetchUnit.h"
#include "DecodeUnit.h"
#include "ExecuteUnit.h"

using namespace std;

// Processor Constructor
Processor::Processor()  
{
    PC = 0;

    Instruction nop("\tnop");
    nop_instruction = nop;

    for (int f=0; f < FETCH_UNITS; f++)
    {
        FetchUnit new_fetch_unit;
        fetch_units.push_back(new_fetch_unit);
    }

    for (int d=0; d < DECODE_UNITS; d++)
    {
        DecodeUnit new_decode_unit;
        decode_units.push_back(new_decode_unit);
    }

    for (int e=0; e < EXECUTE_UNITS; e++)
    {
        ExecuteUnit new_execute_unit;
        execute_units.push_back(new_execute_unit);
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
    Instruction new_instruction(line);
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
        // Print prcoessor state, and wait for 'Enter' keypress
        #ifdef DEBUG
            debug_processor();
            getchar();
        #endif


        #if PIPELINED==1
            execute_instructions();
            if (!refresh_flag)  
            {
                decode_instructions();
                fetch_instructions();
                incrementPC();
            }
            incrementCycles();
            refresh_flag = false;
        #else
            fetch_instructions();
            incrementCycles();

            #ifdef DEBUG
                debug_processor();
                getchar();
            #endif

            decode_instructions();
            incrementCycles();

            #ifdef DEBUG
                debug_processor();
                getchar();
            #endif

            execute_instructions();
            incrementCycles();
            incrementPC();

            #ifdef DEBUG
                    debug_processor();
                    getchar();
            #endif
            
            execute_units.at(0).is_empty = true;

        #endif 
    }

    // Some computation here
    auto end = std::chrono::system_clock::now();

    std::chrono::duration<double> elapsed_seconds = end - start;
    std::time_t end_time = std::chrono::system_clock::to_time_t(end);

    // Print prcoessor state and exit
    #ifdef DEBUG
        PC = -1;
        debug_processor();
        cout << "Program Result=" << registers[16] << endl;
    #endif

    

    #ifdef PRINT_STATS
        printf("Time Elapsed: %.2f\n", elapsed_seconds.count());
        printf("Executed Instructions = %d\n", executed_instructions);
        printf("Total Cycles = %.d\n\n", cycles);
        printf("Instructions per Cycle = %.2f\n", (float)(executed_instructions) / (float)(cycles));
        printf("Instructions per Second = %.2f\n", (float)(executed_instructions) / (float)(elapsed_seconds.count()));
    #else
        cout << registers[16]; 
    #endif
    
    output_image("after.pgm", 16, 16, &main_memory[0]);
}

// Get the Instruction represented by the program counter
void Processor::fetch_instructions()  {
    for (int f=0; f < FETCH_UNITS; f++)  
    {
        fetch_units.at(f).newInstruction(this);
        fetch_units.at(f).fetch(this);
    }
}

void Processor::decode_instructions()  
{
    for (int d=0; d < DECODE_UNITS; d++)  
    {
        for (int f=0; f < FETCH_UNITS; f++)  
        {
            if (!fetch_units.at(f).is_empty)
            {
                fetch_units.at(f).passToDecodeUnit(&decode_units.at(d));
                break;
            }
        }
        decode_units.at(d).decode();
    }
}


// If statements identify the correct operation, and then execute it.
void Processor::execute_instructions()  
{
    for (int e=0; e < EXECUTE_UNITS; e++)  
    {
        for (int d=0; d < DECODE_UNITS; d++)  
        {
            if (!decode_units.at(d).is_empty)
            {
                decode_units.at(d).passToExecuteUnit(&execute_units.at(e));
                break;
            }
        }
        execute_units.at(e).execute(this);
    }
}

void Processor::refresh_pipeline() 
{
    fetch_units.at(0).current_instruction   = nop_instruction;
    decode_units.at(0).current_instruction  = nop_instruction;
    execute_units.at(0).current_instruction = nop_instruction;
    fetch_units.at(0).is_empty   = true;
    decode_units.at(0).is_empty  = true;
    execute_units.at(0).is_empty = true;
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
        if (i == PC)
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
            cout << j*8+i << ": " << fp_registers[j*8+i] << " ";
        }
        cout << endl;
    }

    // cout << "\nMain Memory : \n" << endl;

    // for (int j = 0; j < 10; j++)  {
    //     for (int i = 0; i < 10; i++)  {
    //         if (j*10+i < 10)  cout << " ";
    //         cout << j * 10 + i << ": " << main_memory[j * 10 + i] << " ";
    //     }
    //     cout << endl;
    // }

    cout << endl << endl;
#if PIPELINED==1
    cout << "Fetch Unit: "   << (PC < instructions.size() ? instructions.at(PC).to_string() : "nop" ) << endl;
    cout << "Decode Unit: "  << (fetch_units.at(0).is_empty  ? "Empty"  : fetch_units.at(0).current_instruction.to_string())  << endl;
    cout << "Execute Unit: " << (decode_units.at(0).is_empty ? "Empty"  : decode_units.at(0).current_instruction.to_string()) << endl;
#else
    cout << "Fetch Unit: "   << (fetch_units.at(0).is_empty ? "Empty"    : fetch_units.at(0).current_instruction.to_string() ) << endl;
    cout << "Decode Unit: "  << (decode_units.at(0).is_empty  ? "Empty"  : decode_units.at(0).current_instruction.to_string())  << endl;
    cout << "Execute Unit: " << (execute_units.at(0).is_empty ? "Empty"  : execute_units.at(0).current_instruction.to_string()) << endl;
#endif
    cout << endl << endl;
}

// Routine to output the image in Netpbm grayscale binary image format
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

