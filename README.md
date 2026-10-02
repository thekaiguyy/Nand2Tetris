# Nand2Tetris, Part 1

Building a computer from a single NAND gate, then writing the software that runs on it.
This repo is my record of working through *The Elements of Computing Systems* (Part 1, hardware and the assembler), one project at a time.

## The stack I built

```
Project 6   Assembler (C++)        .asm  ->  binary
Project 4   Machine language       Fill.asm, Mult.asm
   |
Project 5   CPU + Memory + Computer
Project 3   Bit, Register, RAM, PC     (state: the DFF)
Project 2   Adders, ALU                (arithmetic)
Project 1   Logic gates                (everything starts at NAND)
```

## Projects

| # | Folder | What's in it |
|---|--------|--------------|
| 1 | `project_01_boolean_logic` | Basic gates, multiplexers, demultiplexers, built from NAND |
| 2 | `project_02_boolean_arithmetic` | Half/full adder, 16-bit adder, incrementer, ALU |
| 3 | `project_03_memory` | Bit, Register, RAM8 to RAM16K, Program Counter |
| 4 | `project_04_machine_language` | `Mult.asm` (R2 = R0 * R1), `Fill.asm` (screen fills while a key is held), plus my pseudocode notes |
| 5 | `project_05_computer_architecture` | Memory, CPU, Computer: the full Hack machine |
| 6 | `project_06_assembler` | Hack assembler in C++ |

## What I learned

- **Everything is NAND underneath.** Once Project 1 was done, I stopped seeing gates as a given.
- **A DFF is the only thing with memory.** Registers, RAM and the PC are all combinational logic wrapped around it, and the clock decides when state changes.
- **The ALU is a few control bits.** `zx, nx, zy, ny, f, no` generate all 18 operations from one small circuit.
- **A CPU is mostly decoding.** The instruction bits are wired straight to control signals, and the A/C instruction split decides what everything else does.
- **Memory-mapped I/O is just addresses.** The screen and keyboard are ordinary RAM locations (`SCREEN = 16384`, `KBD = 24576`), so `Fill.asm` draws by writing to memory.
- **The assembler needs two passes.** Labels can be referenced before they're defined, so the first pass builds the symbol table and the second translates.

## Using this repo

These are my solutions only. The official `.tst` and `.cmp` files for the HDL projects and the course tools aren't included.

1. Download the Nand2Tetris software suite from [nand2tetris.org](https://www.nand2tetris.org).
2. **HDL projects (1, 2, 3, 5):** open the Hardware Simulator, load a `.hdl` file from this repo, and run it against the matching test script from the course's project files.
3. **Assembly programs (4):** open the CPU Emulator, load `Mult.asm` or `Fill.asm`, and run it with the course's test scripts.
4. **Assembler (6):**
```bash
   cd project_06_assembler
   make                               # builds ./assembler
   ./assembler Prog.asm [Prog.hack]   # output file is optional
   make test                          # assembles tests/*.asm and diffs against tests/*.cmp
   make clean                         # removes the binary and object files
```

## Status

Part 1 complete: projects 1 to 6.
