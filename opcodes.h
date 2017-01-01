#ifndef OPCODES_H_
#define OPCODES_H_

enum {
    // Halts execution 
    // HALT
    HALT = 0, 

    // Adds two registers and stores the result in the third
    // ADD r1 r2 r3
    ADD = 1, 

    // Subtracts two registers and stores the result in the third
    // SUB r1 r2 r3
    SUB = 2,

    // Compares two registers and stores the result in the third
    // CMP r1 r2 r3
    CMP = 3,

    // Jumps to the address in the second register if the
    // first register is a "less than"
    // JLT r1 r2
    JLT = 4,

    // Jumps to the address in the second register if the
    // first register is a "greater than"
    // JGT r1 r2
    JGT = 5,

    // Jumps to the address in the second register if the
    // first register is a "equal to"
    // JEQ r1 r2
    JEQ = 6,

    // Jumps to the address in the register
    // JMP r1
    JMP = 7,

    // Copies the value in the first register to the second register
    // MOV r1 r2
    MOV = 8,

    // Loads the first register with the value stored at the
    // memory address in the second register
    // LDR r1 r2
    LDR = 9,

    // Stores the value in the first register with the value stored at the
    // memory address in the second register
    // STR r1 r2
    STR = 10,

    // Loads the value in the first register with the
    // constant value in the second argument
    // LRI r1 #103
    LRC = 11,
};

#endif
