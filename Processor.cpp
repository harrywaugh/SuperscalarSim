#include "Processor.h"

using namespace std;

Processor::Processor()  {
    PC = 0;
}

Processor::~Processor()
{
}

void Processor::incrementPC()  {
    PC++;
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
    create_fn_map();
    PC = fn_map.at("main");

    bool running = true;
    while (running)  {
        debug_processor();
        execute_instruction(instructions.at(PC));
        incrementPC();
        running = false;
    }

}

void Processor::execute_instruction(Instruction current_instruction)  {
    if (current_instruction.is_fn)  return;
    if ( current_instruction.opcode.compare("li") ) {
        stringstream parser(current_instruction.operand1);
        int val = 0;
        parser >> val;
        registers[register_map.at(current_instruction.operand0)] = val;
        cout << "Set register " << current_instruction.operand0 << " to " << current_instruction.operand1;
    }
}

void Processor::debug_processor()  {
    cout << "PROCESSOR DEBUG LOG" << endl;
    cout << "Current Program Counter : " << PC << endl;
    cout << "Instructions : " << PC << endl;

    for (int i = 0; i < instructions.size(); i++)  {
        if (!instructions.at(i).is_fn)
            cout << "\t";
        cout << i << ". " << instructions.at(i).to_string(); 
        if (i == PC)
            cout << "  <- PC";
        cout << endl;
    }
    cout << endl;

}