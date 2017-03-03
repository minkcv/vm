# Getting Started
First, make sure you have built the main program and the assembler via the build instructions in the main README.md

#### Assembling
Write your assembly code in a plain text file and give it the extension `.asm`. Check out the `assembly.md` document to get started writing programs. Also see `sample.md` for some useful sample code. Then run

    ./assembler -f myprogram.asm

    Which will produce a myprogram.bin file.

#### Packing data into a ROM
TODO

#### Running
Run the binary with

    ./main -f myprogram.bin

Optionally add:
- `-s #`, where # is a number, if you want to scale the display up.
- `&> /dev/null` at the end to suppress print statements.
- `-r myrom.rom` to load up to 32768 bytes into the ROM section of memory.

Consider creating a shell script for your program that is similar to the `testvm.sh` that is included.
