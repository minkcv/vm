# The Assembly Language
This file describes the syntax of the assembly language for the provided assembler. See `opcodes.md` to learn how to use each instruction. See `getting_started.md` to learn how to use the assembler. Be careful when looking at the output binary file in a hex editor as it may not represent the program accurately since the binary is read in to an unsigned type.

#### Constants
For instructions that take a constant, the constant can be specified with `#` for decimal or `$` for hexadecimal.

    LRC r1 #123
    LRC r1 $7B ; same as above

#### Comments
Comments are indicated with a semicolon (`;`). Comments either start after an instruction or at the beginning of a newline.

    ; load r1 with 26
    LRC r1 #26; 26 decimal
    LRC r2 #26 ; also valid

#### Jump Labels
Labels are specified with an at sign (`@`). Lines that start with an at sign indicate that the instruction following them is their address. The jump opcodes operate on registers in order to address 65535 different instructions so the assembler macro "LRL" for "Load Registers Label" is used to make jumping easier. "LRL" inserts two LRC instructions. One for the segment of the label and one for the offset of the label. Example:

    @mylabel
    ADDI r1 #123
    ; load r8 with the segment of @mylable and r9 with the offset of @mylabel
    LRL r8 r9 @mylabel
    JMP r8 r9
