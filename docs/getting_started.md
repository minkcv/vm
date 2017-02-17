# Getting Started
First, make sure you have built the main program and the assembler via the build instructions in the main README.md

#### Assembling
Write your assembly code in a plain text file and give it the extension `.asm`. Follow the guidelines in the `assembly.md` document on the syntax. Run

    ./assembler -f myprogram.asm

Which will produce a myprogram.bin file. If you encounter any errors with assembling then be sure to consult the `assembly.md` file because the syntax is quite strict.

#### Packing data into a ROM
TODO

#### Running
Run the binary with

    ./main -f myprogram.bin

Optionally add:
- `-s #`, where # is a number, if you want to scale the display up.
- `&> /dev/null` at the end to suppress print statements.

Consider creating a shell script for your program that is similar to the `testvm.sh` that is included.
