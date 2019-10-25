#include "main.h"

using namespace std;

int main(int argn, char *argv[])  {

    // Read in filename and declare variables.
    string filename = argv[1];
    string line;
    ifstream infile(filename);
    Processor processor;

    // Print filename and read filename intro string.
    cout << "Program Name: " << filename << endl;


    //Read file line by line
    if (infile.is_open())  {
        while (getline(infile, line))  {
            // Skip line if: empty or is a comment
            if (line.size() == 0 || line[0] == '#' || line[0] == '\n')
                continue;    
            // Check if line is variable
            else if (line[0] == '.') 
                processor.addVariable(line);
            // Line is an instruction
            else if (line[line.size()-1] == ':')
                processor.addFunction(line);
            else 
                processor.addInstruction(line);
        }
        infile.close();
    }
    else
        cout << "Unable to open file";
    
    // Run the program on the processor

    processor.run_program();

    return 0;
}


