#include "Instruction.h"
 
using namespace std;


Instruction::Instruction()  {
    
}

Instruction::Instruction(string line)  {
    int pin = 0;
    int comma = 0;

    if (line.at(0) != '\t')  {
        int start_index = 0;
        int current_index = 0;
        current_index = iterate_until_delim(current_index, line);
        fn_name = line.substr(start_index, current_index-start_index - 1);
        return;
    }

    is_fn = false;

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
    if (line.at(current_index-1) == ',')  comma = -1;
    operand0 = line.substr(start_index, current_index - start_index + comma);
    comma = 0;


    start_index = current_index + 1;
    current_index = start_index;
    if (current_index >= line.size())  {
        debug(pin);
        return;
    }
    current_index = iterate_until_delim(current_index, line);
    if (line.at(current_index-1) == ',')  comma = -1;
    operand1 = line.substr(start_index, current_index - start_index + comma);
    comma = 0;


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
    operand2 = operand1.substr(current_index + 1, operand1.size() - current_index - 2);
    operand1 = operand1.substr(0, current_index);
}

Instruction::~Instruction()  {

}

void Instruction::debug(int pin)  {
    if (pin == 1)
        cout << "Instruction op:" << opcode << " op0:" << operand0 << " op1:" << operand1 << " op2:" << operand2 << endl;
}

int Instruction::iterate_until_delim(int index, string line)  {
    while (index < line.size() && line.at(index) != ' ')  {
        index++;
    }
    return index;
}

string Instruction::to_string()  {
    if (is_fn) 
        return fn_name;

    string new_string = opcode;
    new_string = new_string.append(" ");
    new_string = new_string.append(operand0);
    new_string = new_string.append(" ");
    new_string = new_string.append(operand1);
    new_string = new_string.append(" ");
    new_string = new_string.append(operand2);
    return new_string;
}