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
}

void Processor::addVariable(string line)
{
    int start_index = 1;
    int current_index = 1;
    string var_name;
    int value;

    while (current_index < line.size() && line.at(current_index) != ' ')  {
        current_index++;
    }

    var_name = line.substr(start_index, current_index - start_index);
    current_index++;
    value = stoi(line.substr(current_index));

    main_memory[free_mem_pointer] = value;
    var_map.insert(pair<string, int>(var_name, free_mem_pointer++));

}

void Processor::create_fn_map()  {
    bool debug_pin = false;
    for (int i = 0; i < instructions.size(); i++)  {
        if (instructions.at(i).is_fn)  {
            fn_map.insert(pair<string, int>(instructions.at(i).fn_name, i + 1));
            if (debug_pin)
                cout << instructions.at(i).fn_name << " " << fn_map.at(instructions.at(i).fn_name) << endl;
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
        getchar();
        current_instruction = fetch_instruction();
        incrementCycles();
        execute_instruction(current_instruction);
        incrementCycles();
        incrementCycles();

        incrementPC();
        
    }
    PC = -1;
    debug_processor();
    cout << "Program executed successfully" << endl;
}

Instruction Processor::fetch_instruction()  {
    return instructions.at(PC);
}

void Processor::execute_instruction(Instruction current_instruction)  {

    if ( current_instruction.is_fn )  return;
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
            PC = fn_map.at(current_instruction.operand2);
        return;
    }
    if ( current_instruction.opcode.compare("add") == 0 )  {
        registers[register_map.at(current_instruction.operand0)] =
            registers[register_map.at(current_instruction.operand1)] + registers[register_map.at(current_instruction.operand2)];
        return;
    }
    if ( current_instruction.opcode.compare("addi") == 0 )  {
        cout << stoi(current_instruction.operand2) << endl;
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

    for (int j = 0; j < 10; j++)  {
        for (int i = 0; i < 10; i++)  {
            if (j*10+i < 10)  cout << " ";
            cout << j * 10 + i << ": " << main_memory[j * 10 + i] << " ";
        }
        cout << endl;
    }

    cout << endl << endl;
}