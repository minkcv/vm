#ifndef OPCODES_H_
#define OPCODES_H_

// Max 16 opcodes so we can use 4 bits to identify each opcode
// Max 16 registers so 4 bits to identify each register
// Max 3 registers to one opcode so max 2 bytes per instruction (16 bits)
// Uses of constants like #123 are interchangeable with hex constants such as $7B

// Halts execution 
// HALT
#define HALT 0x0 

// Adds last two registers and stores the result in the first
// ADD r1 r2 r3
#define ADD 0x1 

// Subtracts r2 - r3 and stores the result in the first
// SUB r1 r2 r3
#define SUB 0x2

// Compares last two registers and stores the result in the first
// CMP r1 r2 r3
#define CMP 0x3

// Jumps to the label if the value in the
// first register is a "less than"
// JLT r1 @label
#define JLT 0x4

// Jumps to the label if the value in the
// first register is a "greater than"
// JGT r1 @label
#define JGT 0x5

// Jumps to the label if the value in the
// first register is a "equal to"
// JEQ r1 @label
#define JEQ 0x6

// Jumps to the label
// JMP @label
#define JMP 0x7

// Copies the value in the second register to the first register
// CPY r1 r2
#define CPY 0x8

// Loads the first register with the value stored at the
// memory address specified by the address of a segment (r2) and the address of a byte (r3)
// LDR r1 r2 r3
#define LDR 0x9

// Stores the value in the first register at the 
// memory address specified by the address of a segment (r2) and the address of a byte (r3)
// STR r1 r2 r3
#define STR 0xA

// Loads the value in the first register with the
// constant value in the second argument which takes up 8 bits (the rest of the instruction)
// LRC r1 103
#define LRC 0xB

// Binary ANDs the second and third register values
// and stores the value in the first register
// AND r1 r2 r3
#define AND 0xC

// Binary ORs the second and third register values
// and stores the value in the first register
// OR r1 r2 r3
#define OR 0xD

// Binary NOTs the second registers value
// and stores the value in the first register
// NOT r1 r2
#define NOT 0xE

// Logical (bitwise) shifts the value in the first register
// by the amount indicated by the second register 
// in the direction indicated by the third register where 0 = left and 2 = right.
// Fills new places with zeroes.
// SHF r1 r2 r3
#define SHF 0xF

#endif
