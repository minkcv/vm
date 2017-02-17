# The Assembly Language
This file describes the syntax of the assembly language. See `opcodes.md` to learn how to use each instruction. See `getting_started.md` to learn how to use the assembler. Be careful when looking at the output binary file in a hex editor as it may not represent the program accurately since the binary is read in to an unsigned type.

#### Some important rules
Blank lines are not allowed, use an empty comment instead.

#### Comments
Comments are indicated with a semicolon (`;`). Comments either start after an instruction or at the beginning of a newline. Comments starting after an instruction must be preceded by a space. Example:

    Allowed:
    LRC r0 #10 ; load r0 with decimal 10

    Not allowed:
    LRC r0 #10; load r0 with decimal 10

#### Jump Labels
Labels are specified with an at sign (`@`). Lines that start with an at sign indicate that the instruction following them is their address. Instructions with labels can not have trailing comments. Label names must not include spaces. Example:

    Allowed:
    @loopforever
    ; a comment following a label
    JMP @loopforever

    Not allowed:
    @my label
    JMP @my label
    @mylabel ; my label
    JMP @mylabel ; goto my label
