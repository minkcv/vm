#ifndef OPCODES_H_
#define OPCODES_H_

// Max 16 opcodes so we can use 4 bits to identify each opcode
// Max 16 registers so 4 bits to identify each register
// Max 3 registers to one opcode so max 2 bytes per instruction (32 bits)

// Halts execution 
// HALT
#define HALT 0x0 

// Adds two registers and stores the result in the third
// ADD r1 r2 r3
#define ADD 0x1 

// Subtracts two registers and stores the result in the third
// SUB r1 r2 r3
#define SUB 0x2

// Compares two registers and stores the result in the third
// CMP r1 r2 r3
#define CMP 0x3

// Jumps to the address in the second register if the
// first register is a "less than"
// JLT r1 r2
#define JLT 0x4

// Jumps to the address in the second register if the
// first register is a "greater than"
// JGT r1 r2
#define JGT 0x5

// Jumps to the address in the second register if the
// first register is a "equal to"
// JEQ r1 r2
#define JEQ 0x6

// Jumps to the address in the register
// JMP r1
#define JMP 0x7

// Copies the value in the first register to the second register
// MOV r1 r2
#define MOV 0x8

// Loads the first register with the value stored at the
// memory address specified by the address of a segment (r2) and an offset (r3)
// LDR r1 r2 r3
#define LDR 0x9

// Stores the value in the first register at the 
// memory address specified by the address of a segment (r2) and an offset (r3)
// STR r1 r2 r3
#define STR 0xA

// Loads the value in the first register with the
// constant value in the second argument
// LRC r1 103
#define LRC 0xB

#endif
