# SuperscalarSim

A superscalar processor simulator that can be configured to run in Scalar, Pipelined and Superscalar modes. It features a pipeline with 6 stages of execution (Fetch, Decode, Issue, Dispatch, Execute, Commit) and implements Out-of-Order execution using Tomosulo's algorithm with a reorder buffer. The simulator is designed to be n-way superscalar is highly configurable. Features:
- Configurable mode: Scalar, Pipelined and Superscalar
- Configurable number of Branch, Load/Store and ALU units.
- Configurable number of Fetch/Decode/Issue/Commit instructions per cycle.
- Configurable choice of branch predictor: Static Taken, Static Not Taken, Dynamic 2-bit, Dynamic 2-bit with Branch Target Buffer default
- Execution unit results are forwarded to reservation stations for use in the next cycle.

A quick overview of the simulator can be found in "Superscalar Simulator.pdf" with an architecture diagram and some experiments.

### Compile and Run

**Compiling:**
```shell
# To compile scalar:
make stats
# To compile pipelined:
make pipelined-stats
# To compile superscalar: where <BPredictor> = T, NT, 2bit, 2bit-btb
make superscalar-stats-<BPredictor>
```

The code can also be run in a step-by-step debug mode by replacing `stats` with `debug`.

**Running**
```shell
./simulator.exe programs/<program>
# where program = {
#    stencil.benchmark
#    vector_add.benchmark
#    imatmul.benchmark
#    addition.asm
#    factorial.asm
#    independent_math.asm
#    gcd.asm
#    sum_n_natural.asm
# }
```


**Useful Links:**

MIPS Reference:
https://www.dsi.unive.it/~gasparetto/materials/MIPS_Instruction_Set.pdf