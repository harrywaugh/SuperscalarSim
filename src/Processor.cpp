#include "Processor.h"

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


        execute_instructions();
        if (!refresh_flag)  
        {
            decode_instructions();
            fetch_instructions();

            incrementPC();
        }

        refresh_flag = false;

        incrementCycles();

        //Increment program counter
        
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
    printf("Executed Instructions = %.2f\n", executed_instructions);
    printf("Total Cycles = %.d\n\n", cycles);
    printf("Instructions per Cycle = %.2f\n", (float)(executed_instructions) / (float)(cycles));
    printf("Instructions per Second = %.2f\n", (float)(executed_instructions) / (float)(elapsed_seconds.count()));
#endif

    cout << registers[16];

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

            // cout << "Fetch unit is " <<  (fetch_units.at(f).is_empty ? "empty" : "full") << endl;


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
            // cout << "Decode unit is " <<  (decode_units.at(d).is_empty ? "empty" : "full") << endl;

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

    for (it1 = var_map.begin(); it1 != var_map.end(); it1++)
    {
        cout << it1->first << " -> &Mem + " << it1->second <<  endl;
    }

    cout << "\nInstructions : \n" << endl;

    for (int i = 0; i < instructions.size(); i++)  {
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

    // cout << "\nMain Memory : \n" << endl;

    // for (int j = 0; j < 10; j++)  {
    //     for (int i = 0; i < 10; i++)  {
    //         if (j*10+i < 10)  cout << " ";
    //         cout << j * 10 + i << ": " << main_memory[j * 10 + i] << " ";
    //     }
    //     cout << endl;
    // }

    cout << endl << endl;

    cout << "Fetch Unit: "   << (PC < instructions.size() ? instructions.at(PC).to_string() : "nop" ) << endl;
    cout << "Decode Unit: "  << (fetch_units.at(0).is_empty  ? "Empty"  : fetch_units.at(0).current_instruction.to_string())  << endl;
    cout << "Execute Unit: " << (decode_units.at(0).is_empty ? "Empty"  : decode_units.at(0).current_instruction.to_string()) << endl;

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


// FETCH UNIT
Processor::FetchUnit::FetchUnit()  
{
    
}

void Processor::FetchUnit::newInstruction(Processor *processor)  
{
    if (processor->PC >= processor->instructions.size())  
        current_instruction = processor->nop_instruction;
    else
        current_instruction = processor->instructions.at(processor->PC);
    is_empty = false;
}

void Processor::FetchUnit::fetch(Processor *processor)
{
    switch (processor->string_to_op_map[current_instruction.opcode]) {
        case J:
            processor->registers[31] = processor->PC;
            processor->PC = processor->fn_map.at(current_instruction.operand0)-1;
            break;
        case RETURN:
            processor->PC = processor->registers[31]-1;
            break;
        case BEQ:
            processor->branch_record.push(processor->PC);
            processor->PC += stoi(current_instruction.operand2)-1;
            break;
        case BLT:
            processor->branch_record.push(processor->PC);
            processor->PC += stoi(current_instruction.operand2)-1;
            break;
        default:
            break;
    }
    return;
}

void Processor::FetchUnit::passToDecodeUnit(Processor::DecodeUnit *decode_unit)  
{
    decode_unit->newInstruction(current_instruction);
}

// DECODE UNIT
Processor::DecodeUnit::DecodeUnit()  
{
    
}

void Processor::DecodeUnit::newInstruction(Instruction new_instruction)  
{
    current_instruction = new_instruction;
    is_empty = false;
}

void Processor::DecodeUnit::decode()  
{
    if (is_empty)  return;
    // cout << "Decoding instruction: " << current_instruction.to_string() << endl;
}

void Processor::DecodeUnit::passToExecuteUnit(Processor::ExecuteUnit *execute_unit)  
{
    execute_unit->newInstruction(current_instruction);
}

// EXECUTE UNIT
Processor::ExecuteUnit::ExecuteUnit()  
{
    
}

void Processor::ExecuteUnit::newInstruction(Instruction new_instruction)  
{
    current_instruction = new_instruction;
    is_empty = false;
}

void Processor::ExecuteUnit::execute(Processor *processor)  
{

    if(is_empty)  return;

    // cout << "Executing instruction: " << current_instruction.to_string() << endl;
    switch (processor->string_to_op_map[current_instruction.opcode]) {
        case EXIT:
            processor->registers[31] = -1;
            break;
        // case J:
        //     processor->registers[31] = processor->PC;
        //     processor->PC = processor->fn_map.at(current_instruction.operand0);
        //     processor->refresh_pipeline();
        //     break;
        // case RETURN:
        //     processor->PC = processor->registers[31];
        //     processor->refresh_pipeline();
        //     break;
        case BEQ:
            if (processor->registers[processor->register_map.at(current_instruction.operand0)] !=
                processor->registers[processor->register_map.at(current_instruction.operand1)])
                {
                    processor->PC = processor->branch_record.front() + 1;
                    processor->branch_record.pop();
                    processor->refresh_pipeline();
                }

            break;
        case BLT:
            if (processor->registers[processor->register_map.at(current_instruction.operand0)] >
                processor->registers[processor->register_map.at(current_instruction.operand1)])
                {
                    processor->PC = processor->branch_record.front() + 1;
                    processor->branch_record.pop();
                    processor->refresh_pipeline();
                }
            break;
        case ADD:   
            processor->registers[processor->register_map.at(current_instruction.operand0)] =
                processor->registers[processor->register_map.at(current_instruction.operand1)] + processor->registers[processor->register_map.at(current_instruction.operand2)];
            break;
        case ADD_F:
            processor->fp_registers[processor->fp_register_map.at(current_instruction.operand0)] =
                processor->fp_registers[processor->fp_register_map.at(current_instruction.operand1)] + processor->fp_registers[processor->fp_register_map.at(current_instruction.operand2)];
            break;
        case  ADDI:
            processor->registers[processor->register_map.at(current_instruction.operand0)] =
                processor->registers[processor->register_map.at(current_instruction.operand1)] + stoi(current_instruction.operand2);
            break;
        case  ADDI_F:
            processor->fp_registers[processor->fp_register_map.at(current_instruction.operand0)] =
                processor->fp_registers[processor->fp_register_map.at(current_instruction.operand1)] + stof(current_instruction.operand2);
            break;
        case SUB:
            processor->registers[processor->register_map.at(current_instruction.operand0)] =
                processor->registers[processor->register_map.at(current_instruction.operand1)] - processor->registers[processor->register_map.at(current_instruction.operand2)];
            break;
        case SUBI:
            processor->registers[processor->register_map.at(current_instruction.operand0)] =
                processor->registers[processor->register_map.at(current_instruction.operand1)] - stoi(current_instruction.operand2);
            break;
        case MUL:
            processor->registers[processor->register_map.at(current_instruction.operand0)] =
                processor->registers[processor->register_map.at(current_instruction.operand1)] * processor->registers[processor->register_map.at(current_instruction.operand2)];
            break;
        case MULI:
            processor->registers[processor->register_map.at(current_instruction.operand0)] =
                processor->registers[processor->register_map.at(current_instruction.operand1)] * stoi(current_instruction.operand2);
            break;
        case MULI_F:
            processor->fp_registers[processor->fp_register_map.at(current_instruction.operand0)] =
                processor->fp_registers[processor->fp_register_map.at(current_instruction.operand1)] * stof(current_instruction.operand2);
            break;
        case DIVI:
            processor->registers[processor->register_map.at(current_instruction.operand0)] =
                processor->registers[processor->register_map.at(current_instruction.operand1)] / stoi(current_instruction.operand2);
            break;
        case DIVI_F:
            processor->fp_registers[processor->fp_register_map.at(current_instruction.operand0)] =
                processor->fp_registers[processor->fp_register_map.at(current_instruction.operand1)] / stof(current_instruction.operand2);
            break;
        case AND:
            processor->registers[processor->register_map.at(current_instruction.operand0)] =
                processor->registers[processor->register_map.at(current_instruction.operand1)] & processor->registers[processor->register_map.at(current_instruction.operand2)];
            break;
        case OR:
            processor->registers[processor->register_map.at(current_instruction.operand0)] =
                processor->registers[processor->register_map.at(current_instruction.operand1)] | processor->registers[processor->register_map.at(current_instruction.operand2)];
            break;
        case SLL:
            processor->registers[processor->register_map.at(current_instruction.operand0)] =
                processor->registers[processor->register_map.at(current_instruction.operand1)] << stoi(current_instruction.operand2);
            break;
        case SRL:
            processor->registers[processor->register_map.at(current_instruction.operand0)] =
                processor->registers[processor->register_map.at(current_instruction.operand1)] >> stoi(current_instruction.operand2);
            break;
        case MV:
            processor->registers[processor->register_map.at(current_instruction.operand0)] = processor->registers[processor->register_map.at(current_instruction.operand1)];
            break;
        case LI:
            processor->registers[processor->register_map.at(current_instruction.operand0)] = stoi(current_instruction.operand1);
            break;
        case LI_F:
            processor->fp_registers[processor->fp_register_map.at(current_instruction.operand0)] = stof(current_instruction.operand1);
            break;
        case LW:
            processor->registers[processor->register_map.at(current_instruction.operand0)] = processor->main_memory[processor->registers[processor->register_map.at(current_instruction.operand2)] +
                                                                                   processor->registers[processor->register_map.at(current_instruction.operand1)]];
            // cycles+=2;
            break;
        case LW_F:
            memcpy(&processor->fp_registers[processor->fp_register_map.at(current_instruction.operand0)], 
                   &(processor->main_memory)[processor->registers[processor->register_map.at(current_instruction.operand2)] + processor->registers[processor->register_map.at(current_instruction.operand1)]],
                   sizeof(float));
            // cycles+=2;
            break;
        case LA:
            processor->registers[processor->register_map.at(current_instruction.operand0)] = processor->var_map.at(current_instruction.operand1);
            // cycles+=2;
            break;
        case SW:
            processor->main_memory[processor->registers[processor->register_map.at(current_instruction.operand2)] +
                        processor->registers[processor->register_map.at(current_instruction.operand1)]] = processor->registers[processor->register_map.at(current_instruction.operand0)];
            // cycles+=2;
            break;
        case SW_F:
            memcpy(&(processor->main_memory)[processor->registers[processor->register_map.at(current_instruction.operand2)] + processor->registers[processor->register_map.at(current_instruction.operand1)]],
                   &processor->fp_registers[processor->fp_register_map.at(current_instruction.operand0)], 
                   sizeof(float));
            // cycles+=2;
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
    // is_empty = true;
    processor->executed_instructions++;
}