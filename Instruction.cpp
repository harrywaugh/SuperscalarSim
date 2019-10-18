#include "Instruction.h"
 
using namespace std;

Instruction::Instruction(string line)  {
    int pin = 0;
    if (line.at(0) != '\t')  return;
    int start_index = 1;
    int current_index = 1;
    current_index = iterate_until_delim(current_index, line);
    opcode = line.substr(start_index, current_index-start_index);
    
    

    start_index = current_index + 1;
    current_index = start_index;
    if (current_index >= line.size())  {
        debug(pin);
        return;
    }
    current_index = iterate_until_delim(current_index, line);
    operand0 = line.substr(start_index, current_index - start_index - 1);


    start_index = current_index + 1;
    current_index = start_index;
    if (current_index >= line.size())  {
        debug(pin);
        return;
    }
    current_index = iterate_until_delim(current_index, line);
    operand1 = line.substr(start_index, current_index - start_index - 1);


    start_index = current_index + 1;
    current_index = start_index;
    if (current_index >= line.size())  {
        if(line.back() == ')')  
            correct_mem_syntax();
        debug(pin);
        return;
    }
    current_index = iterate_until_delim(current_index, line);
    operand2 = line.substr(start_index, current_index - start_index);

    debug(pin);

}

void Instruction::correct_mem_syntax()  {
    int start_index = 0;
    int current_index = 0;
    while (current_index < operand1.size() && operand1.at(current_index) != '(')  {
        current_index++;
    }
    operand2 = operand1.substr(current_index+1);
    operand1 = operand1.substr(0, current_index);
}

Instruction::~Instruction()
{

}

void Instruction::debug(int pin)  {
    if (pin == 1)
        cout << "Instruction op:" << opcode << " op0:" << operand0 << " op1:" << operand1 << " op2:" << operand2 << endl;
}

int Instruction::iterate_until_delim(int index, string line)  {
    while (index < line.size() && line.at(index) != ' ')
    {
        index++;
    }
    return index;
}