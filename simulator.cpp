#include "simulator.h"

using namespace std;

int main(int argn, char *argv[])  {
    // Read in filename and declare variables.
    string filename = argv[1];
    string line;
    ifstream infile(filename);
    Processor processor;

    // Print filename and read filename intro string.
    cout << "Program Name: " << filename << "\nContents:" << endl;

    //Read file line by line
    if (infile.is_open())  {
        while (getline(infile, line))  {
            if (line[0] == '#' || line[0] == '\n')
                continue;    
            cout << line << '\n';
            processor.addInstruction(line);
        }
        infile.close();
    }
    else
        cout << "Unable to open file";
    
    processor.start();

    return 0;
}

// Fetch -> Decode -> Execute -> Writeback
// void run_simulator(char ***program, int program_len)  {
//     int pc = find_fn("main", program, program_len);
//     int clock = 0;
//     uint32_t a[4]  = {0}; // Argument registers
//     uint32_t t[10] = {0}; // Temporary registers
//     uint32_t v[2]  = {0}; // Return registers
//     uint32_t s[8]  = {0}; // Computed registers
//     uint8_t exit   = 0;
//     char *opcode;
//     char *curr_instruction;
    

//     while (exit == 0)  {
//         debug_simulator(pc, program, 1);   // Debug
//         curr_instruction = (*program)[pc]; // Fetch



//         if (pc >= program_len)  exit = 1;
//     }


// }

