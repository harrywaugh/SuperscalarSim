#include "Processor.h"

using namespace std;

// Processor Constructor
Processor::Processor()  
{
    PC = 0;
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
    var_map.insert(pair<string, uint32_t>(var_name, free_mem_pointer++));
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
    uint32_t *arr_pointer = &main_memory[free_mem_pointer];
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
    var_map.insert(pair<string, uint32_t>(arr_name, free_mem_pointer - length));
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

        // Fetch stage
        current_instruction = fetch_instruction();

        // Decode and Execute Stages
        decode_and_execute_instruction(current_instruction);

        //Increment program counter
        incrementPC();
        
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
    cout << "Time Elapsed: " << elapsed_seconds.count() << endl;
    cout << "Executed Instructions = " << executed_instructions << endl;
    cout << "Total Cycles = " << cycles << endl;
    cout << "Instructions per Cycle = " << (float)(executed_instructions) / (float)(cycles) << endl;
    cout << "Instructions per Second = " << (float)(executed_instructions) / (float)(elapsed_seconds.count()) << endl;
#endif

    cout << registers[16];

    output_image("after.pgm", 16, 16, &main_memory[0]);
}

// Get the Instruction represented by the program counter
Instruction Processor::fetch_instruction()  {
    incrementCycles();
    return instructions.at(PC);
}

// If statements identify the correct operation, and then execute it.
void Processor::decode_and_execute_instruction(Instruction current_instruction)  
{

    switch (string_to_op_map[current_instruction.opcode]) {
        case EXIT:
            registers[31] = -1;
            break;
        case J:
            registers[31] = PC;
            PC = fn_map.at(current_instruction.operand0) - 1;
            break;
        case RETURN:
            PC = registers[31];
            break;
        case BEQ:
            if (registers[register_map.at(current_instruction.operand0)] ==
                registers[register_map.at(current_instruction.operand1)])
                PC += stoi(current_instruction.operand2) - 1;
            break;
        case BLT:
            if (registers[register_map.at(current_instruction.operand0)] <
                registers[register_map.at(current_instruction.operand1)])
                PC += stoi(current_instruction.operand2) - 1;
            break;
        case ADD:
            registers[register_map.at(current_instruction.operand0)] =
                registers[register_map.at(current_instruction.operand1)] + registers[register_map.at(current_instruction.operand2)];
            break;
        case ADD_F:
            fp_registers[fp_register_map.at(current_instruction.operand0)] =
                fp_registers[fp_register_map.at(current_instruction.operand1)] + fp_registers[fp_register_map.at(current_instruction.operand2)];
            break;
        case  ADDI:
            registers[register_map.at(current_instruction.operand0)] =
                registers[register_map.at(current_instruction.operand1)] + stoi(current_instruction.operand2);
            break;
        case  ADDI_F:
            fp_registers[fp_register_map.at(current_instruction.operand0)] =
                fp_registers[fp_register_map.at(current_instruction.operand1)] + stof(current_instruction.operand2);
            break;
        case SUB:
            registers[register_map.at(current_instruction.operand0)] =
                registers[register_map.at(current_instruction.operand1)] - registers[register_map.at(current_instruction.operand2)];
            break;
        case SUBI:
            registers[register_map.at(current_instruction.operand0)] =
                registers[register_map.at(current_instruction.operand1)] - stoi(current_instruction.operand2);
            break;
        case MUL:
            registers[register_map.at(current_instruction.operand0)] =
                registers[register_map.at(current_instruction.operand1)] * registers[register_map.at(current_instruction.operand2)];
            break;
        case MULI:
            registers[register_map.at(current_instruction.operand0)] =
                registers[register_map.at(current_instruction.operand1)] * stoi(current_instruction.operand2);
            break;
        case MULI_F:
            fp_registers[fp_register_map.at(current_instruction.operand0)] =
                fp_registers[fp_register_map.at(current_instruction.operand1)] * stof(current_instruction.operand2);
            break;
        case DIVI:
            registers[register_map.at(current_instruction.operand0)] =
                registers[register_map.at(current_instruction.operand1)] / stoi(current_instruction.operand2);
            break;
        case DIVI_F:
            fp_registers[fp_register_map.at(current_instruction.operand0)] =
                fp_registers[fp_register_map.at(current_instruction.operand1)] / stof(current_instruction.operand2);
            break;
        case AND:
            registers[register_map.at(current_instruction.operand0)] =
                registers[register_map.at(current_instruction.operand1)] & registers[register_map.at(current_instruction.operand2)];
            break;
        case OR:
            registers[register_map.at(current_instruction.operand0)] =
                registers[register_map.at(current_instruction.operand1)] | registers[register_map.at(current_instruction.operand2)];
            break;
        case SLL:
            registers[register_map.at(current_instruction.operand0)] =
                registers[register_map.at(current_instruction.operand1)] << stoi(current_instruction.operand2);
            break;
        case SRL:
            registers[register_map.at(current_instruction.operand0)] =
                registers[register_map.at(current_instruction.operand1)] >> stoi(current_instruction.operand2);
            break;
        case MV:
            registers[register_map.at(current_instruction.operand0)] = registers[register_map.at(current_instruction.operand1)];
            break;
        case LI:
            registers[register_map.at(current_instruction.operand0)] = stoi(current_instruction.operand1);
            break;
        case LI_F:
            fp_registers[fp_register_map.at(current_instruction.operand0)] = stof(current_instruction.operand1);
            break;
        case LW:
            registers[register_map.at(current_instruction.operand0)] = main_memory[registers[register_map.at(current_instruction.operand2)] +
                                                                                   registers[register_map.at(current_instruction.operand1)]];
            cycles+=2;
            break;
        case LW_F:
            memcpy(&fp_registers[fp_register_map.at(current_instruction.operand0)], 
                   &main_memory[registers[register_map.at(current_instruction.operand2)] + registers[register_map.at(current_instruction.operand1)]],
                   sizeof(float));
            cycles+=2;
            break;
        case LA:
            registers[register_map.at(current_instruction.operand0)] = var_map.at(current_instruction.operand1);
            cycles+=2;
            break;
        case SW:
            main_memory[registers[register_map.at(current_instruction.operand2)] +
                        registers[register_map.at(current_instruction.operand1)]] = registers[register_map.at(current_instruction.operand0)];
            cycles+=2;
            break;
        case SW_F:
            memcpy(&main_memory[registers[register_map.at(current_instruction.operand2)] + registers[register_map.at(current_instruction.operand1)]],
                   &fp_registers[fp_register_map.at(current_instruction.operand0)], 
                   sizeof(float));
            cycles+=2;
            break;
        case NOP:
            break;
    }
    cycles+=2;
    executed_instructions++;
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

    for (it1 = var_map.begin(); it1 != var_map.end(); it1++)
    {
        cout << it1->first << " -> &Mem + " << it1->second <<  endl;
    }

    cout << "\nInstructions : \n" << endl;

    for (int i = 0; i < instructions.size(); i++)  {
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

    cout << "\nMain Memory : \n" << endl;

    for (int j = 0; j < 10; j++)  {
        for (int i = 0; i < 10; i++)  {
            if (j*10+i < 10)  cout << " ";
            cout << j * 10 + i << ": " << main_memory[j * 10 + i] << " ";
        }
        cout << endl;
    }

    cout << endl << endl;
}

// Routine to output the image in Netpbm grayscale binary image format
void Processor::output_image(char *file_name, const int nx, const int ny, uint32_t* image)
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