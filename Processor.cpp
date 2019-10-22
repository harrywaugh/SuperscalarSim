#include "Processor.h"

using namespace std;

Processor::Processor()  {
    PC = 0;
}

Processor::~Processor()  {
}

void Processor::incrementPC()  {
    PC++;
}

void Processor::incrementCycles()  {
    cycles++;
}

void Processor::addInstruction(string line)  {
    Instruction new_instruction(line);
    instructions.push_back(new_instruction);
    instruction_strings.push_back(line);
}

void Processor::create_fn_map()  {
    for (int i = 0; i < instructions.size(); i++)  {
        string line = instruction_strings.at(i);
        if (line.back() == ':')  {
            string function_name = line.substr(0, line.size() - 1);
            fn_map.insert(pair<string, int>(function_name, i+1));
            // instructions.erase(instructions.begin()+i);
            // i--;
        }
    }
}

void Processor::run_program()  {
    Instruction current_instruction;
    create_fn_map();
    PC = fn_map.at("main");

    while ( registers[31] == 0 )  {

        debug_processor();

        current_instruction = fetch_instruction();
        execute_instruction(current_instruction);

        incrementPC();
        
    }
    PC = -1;
    debug_processor();
    cout << "Program executed successfully" << endl;
}

Instruction Processor::fetch_instruction()  {
    incrementCycles();
    return instructions.at(PC);
}

void Processor::execute_instruction(Instruction current_instruction)  {
    incrementCycles();
    incrementCycles();
    if ( current_instruction.is_fn )  return;
    if ( current_instruction.opcode.compare("li") == 0 ) {
        registers[register_map.at(current_instruction.operand0)] = stoi(current_instruction.operand1);
        return;
    }
    if ( current_instruction.opcode.compare("exit") == 0 )  {
        registers[31] = 1;
        return;
    }
    if ( current_instruction.opcode.compare("add") == 0 )  {
        registers[register_map.at(current_instruction.operand0)] =
            registers[register_map.at(current_instruction.operand1)] + registers[register_map.at(current_instruction.operand2)];
        return;
    }
    if ( current_instruction.opcode.compare("sub") == 0 )  {
        registers[register_map.at(current_instruction.operand0)] =
            registers[register_map.at(current_instruction.operand1)] - registers[register_map.at(current_instruction.operand2)];
        return;
    }
    if ( current_instruction.opcode.compare("sw") == 0 )  {
        main_memory[stoi(current_instruction.operand2)] = registers[register_map.at(current_instruction.operand0)];
        return;
    }
}

void Processor::debug_processor()  {
    cout << "\n\n###################" << endl;
    cout << "PROCESSOR DEBUG LOG: Total Cycles = " << cycles << endl;
    cout << "###################" << endl;
    cout << "\nInstructions : \n" << endl;

    for (int i = 0; i < instructions.size(); i++)  {
        if (!instructions.at(i).is_fn)
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

    for (int j = 0; j < 10; j++)
    {
        for (int i = 0; i < 10; i++)
        {
            if (j*10+i < 10)  cout << " ";
            cout << j * 10 + i << ": " << main_memory[j * 10 + i] << " ";
        }
        cout << endl;
    }

    cout << endl << endl;
}