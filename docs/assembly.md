# The Assembly Language
This file describes the syntax of the assembly language for the provided assembler. See `opcodes.md` to learn how to use each instruction. See `getting_started.md` to learn how to assemble and run a program. Be careful if you are looking at the output binary file in a hex editor as it may not represent the program accurately due to endianness. Program binaries also use two bytes to prefix the number of instructions. See `sample.md` if you don't want to "reinvent the wheel" for some common functions.

Program code is not stored in addressable memory. You can't read or write the program code at runtime. 

Jump instructions use a segment and an offset similar to memory addressing to have 65536 addressable instructions. Programs have a maximum length of 65536 instructions.

#### Constants
For instructions that take a constant, the constant can be specified with `#` for decimal or `$` for hexadecimal.

    LRC r1 #123
    LRC r1 $7B ; same constant as above

#### Comments
Comments are indicated with a semicolon (`;`). Comments either start after an instruction or at the beginning of a newline.

    ; load r1 with 26
    LRC r1 #26; 26 decimal
    LRC r2 #26 ; also valid

#### Jump Labels
Labels are specified with an at sign (`@`). Lines that start with an at sign indicate that the instruction following them is their address. The assembler has a maximum of 1024 labels.

 The jump opcodes operate on register values in order to address 65536 different instructions so the assembler macro "LRL" for "Load Registers Label" is used to make jumping easier. "LRL" inserts two LRC instructions. One for the segment address of the label and one for the offset address of the label. Example:

    @mylabel
    ADDC r1 #123 ; loop body
    ; load r8 with the segment of @mylable and r9 with the offset of @mylabel
    LRL r8 r9 @mylabel
    JMP r8 r9
