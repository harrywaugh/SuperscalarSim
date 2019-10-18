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
    // Instruction new_instruction(line);
    instructions.push_back(line);
}

void Processor::create_fn_map()  {
    for (int i = 0; i < instructions.size(); i++)  {
        string line = instructions.at(i);
        if (line.back() == ':')  {
            string function_name = line.substr(0, line.size() - 1);
            fn_map.insert(pair<string, int>(function_name, i + 1));
        }
    }
}

void Processor::run_program()
{
    create_fn_map();

    
}