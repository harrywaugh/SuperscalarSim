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
    var_map.insert(pair<string, int>(var_name, free_mem_pointer++));

}

// Simulate the program on the processor
void Processor::run_program()  
{   
    // Create variable to store current instruction, set program counter to point at main function
    Instruction current_instruction;
    PC = fn_map.at("main");

    // While register exit pin is 0
    while ( registers[31] == 0 )  
    {
        // Print prcoessor state, and wait for 'Enter' keypress
#ifdef DEBUG
        debug_processor();
        getchar();
#endif

        // Fetch stage
        current_instruction = fetch_instruction();
        incrementCycles();

        // Decode and Execute Stages
        incrementCycles();
        decode_and_execute_instruction(current_instruction);
        incrementCycles();

        //Increment program counter
        incrementPC();
        
    }

    // Print prcoessor state and exit
#ifdef DEBUG
    PC = -1;
    debug_processor();
    cout << "Program Result=" << registers[16] << endl;
#endif
    cout << registers[16];
}

// Get the Instruction represented by the program counter
Instruction Processor::fetch_instruction()  {
    return instructions.at(PC);
}

// If statements identify the correct operation, and then execute it.
void Processor::decode_and_execute_instruction(Instruction current_instruction)  {

    if ( current_instruction.opcode.compare("li") == 0 ) {
        registers[register_map.at(current_instruction.operand0)] = stoi(current_instruction.operand1);
        return;
    }
    if ( current_instruction.opcode.compare("exit") == 0 )  {
        registers[31] = 1;
        return;
    }
    if ( current_instruction.opcode.compare("j") == 0 )  {
        PC = fn_map.at(current_instruction.operand0) - 1;
        return;
    }
    if ( current_instruction.opcode.compare("beq") == 0 )  {
        if (registers[register_map.at(current_instruction.operand0)] ==
            registers[register_map.at(current_instruction.operand1)])
            PC = fn_map.at(current_instruction.operand2) - 1;
        return;
    }
    if ( current_instruction.opcode.compare("add") == 0 )  {
        registers[register_map.at(current_instruction.operand0)] =
            registers[register_map.at(current_instruction.operand1)] + registers[register_map.at(current_instruction.operand2)];
        return;
    }
    if ( current_instruction.opcode.compare("addi") == 0 )  {
        registers[register_map.at(current_instruction.operand0)] =
            registers[register_map.at(current_instruction.operand1)] + stoi(current_instruction.operand2);
        return;
    }
    if ( current_instruction.opcode.compare("sub") == 0 )  {
        registers[register_map.at(current_instruction.operand0)] =
            registers[register_map.at(current_instruction.operand1)] - registers[register_map.at(current_instruction.operand2)];
        return;
    }
    if ( current_instruction.opcode.compare("subi") == 0 )  {
        registers[register_map.at(current_instruction.operand0)] =
            registers[register_map.at(current_instruction.operand1)] - stoi(current_instruction.operand2);
        return;
    }
    if ( current_instruction.opcode.compare("mul") == 0 )  {
        registers[register_map.at(current_instruction.operand0)] =
            registers[register_map.at(current_instruction.operand1)] * registers[register_map.at(current_instruction.operand2)];
        return;
    }
    if ( current_instruction.opcode.compare("and") == 0 )  {
        registers[register_map.at(current_instruction.operand0)] =
            registers[register_map.at(current_instruction.operand1)] & registers[register_map.at(current_instruction.operand2)];
        return;
    }
    if ( current_instruction.opcode.compare("or") == 0 )  {
        registers[register_map.at(current_instruction.operand0)] =
            registers[register_map.at(current_instruction.operand1)] | registers[register_map.at(current_instruction.operand2)];
        return;
    }
    if ( current_instruction.opcode.compare("sll") == 0 )  {
        registers[register_map.at(current_instruction.operand0)] =
            registers[register_map.at(current_instruction.operand1)] << stoi(current_instruction.operand2);
        return;
    }
    if ( current_instruction.opcode.compare("srl") == 0 )  {
        registers[register_map.at(current_instruction.operand0)] =
            registers[register_map.at(current_instruction.operand1)] >> stoi(current_instruction.operand2);
        return;
    }
    if ( current_instruction.opcode.compare("mv") == 0 )  {
        registers[register_map.at(current_instruction.operand0)] = registers[register_map.at(current_instruction.operand1)];
        return;
    }
    if ( current_instruction.opcode.compare("lw") == 0 )  {
        registers[register_map.at(current_instruction.operand0)] = main_memory[registers[register_map.at(current_instruction.operand2)]];
        return;
    }
    if (current_instruction.opcode.compare("la") == 0) {
        registers[register_map.at(current_instruction.operand0)] = var_map.at(current_instruction.operand1);
        return;
    }

    if ( current_instruction.opcode.compare("sw") == 0 )  {
        main_memory[registers[register_map.at(current_instruction.operand2)]] = registers[register_map.at(current_instruction.operand0)];
        return;
    }
}

void Processor::debug_processor()  {
    map<string, int>::iterator it;

    cout << "\n\n###################" << endl;
    cout << "PROCESSOR DEBUG LOG: Total Cycles = " << cycles << endl;
    cout << "###################" << endl;

    cout << "\nFunctions : \n" << endl;

    for (it = fn_map.begin(); it != fn_map.end(); it++)
    {
        cout << it->first << " -> " << it->second << endl;
    }

    cout << "\nVariables : \n" << endl;

    for (it = var_map.begin(); it != var_map.end(); it++)
    {
        cout << it->first << " -> Mem + " << it->second << endl;
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