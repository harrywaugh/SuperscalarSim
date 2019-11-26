# SuperscalarSim
Advanced Architecture Coursework at the University of Bristol. Superscalar simulator.

MIPS Reference:
https://www.dsi.unive.it/~gasparetto/materials/MIPS_Instruction_Set.pdf

C to MIPS Compiler:
http://reliant.colab.duke.edu/c2mips/

### Compile and Run

The code can be compiled by running the following:
```shell
make
./simulator.exe programs/<program_name>.asm
```

**where**:
    program_name = [addition, bubble_sort, factorial, gcd, sum_n_natural]


Currently the processor is in **debug** mode, and you can step through each instruction by pressing 'Enter'. 
At each step, the processor will output the current values in main memory, the register values, and more info on any functions, 
variables or arrays that the program is using. 

The program will also display the current instruction and the number of cycles that have been run.


**Note:**
The processor currently has 3 stages: fetch -> decode -> execute.