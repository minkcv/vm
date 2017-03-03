#ifndef OPCODES_H_
#define OPCODES_H_

// Max 16 opcodes so we can use 4 bits to identify each opcode
// Max 16 registers so 4 bits to identify each register
// Max 3 registers to one opcode so max 2 bytes per instruction (16 bits)
// Uses of constants like #123 are interchangeable with hex constants such as $7B

// Extended mode
// uses the second set of 4 bits as an opcode 
// to operate on two 4 bit arguments or one 16 bit argument
#define EXT 0x0

// Halts execution 
// EXT HALT
#define EXT_HALT 0x0

// Copies the value in the second register to the first register
// EXT CPY r1 r2
#define EXT_CPY 0x1

// Binary NOTs the second registers value
// and stores the value in the first register
// EXT NOT r1 r2
#define EXT_NOT 0x2

// Logical shift lefts the value in the first register by the amount
// in the second register. Stores the value in the first register
// Fills new bits with 0
// EXT LSL r1 r2
#define EXT_LSL 0x3

// Logical shift rights the value in the first register by the amount
// in the second register. Stores the value in the first register
// Fills new bits with 0
// EXT LSR r1 r2
#define EXT_LSR 0x4

// Unconditional jump to the address specified by the values in the registers
// by segment and offset
// EXT JMP r1 r2
#define EXT_JMP 0x5

// No operation
// Used to waste time
#define EXT_NOP 0x6

// Adds last two registers and stores the result in the first
// ADD r1 r2 r3
#define ADD 0x1 

// Subtracts r2 - r3 and stores the result in the first
// SUB r1 r2 r3
#define SUB 0x2

// Adds the value to the register
// ADDI r1 #103
#define ADDC 0x3

// Subtracts the value from the register
// SUBI r1 #103
#define SUBC 0x4

// Compares last two registers and stores the result in the first
// CMP r1 r2 r3
#define CMP 0x5

// Jumps to the address in the registers if the value in the
// first register is a "less than"
// JLT r1 r2 r3
#define JLT 0x6

// Jumps to the address in the registers if the value in the
// first register is a "greater than"
// JGT r1 r2 r3
#define JGT 0x7

// Jumps to the address in the registers if the value in the
// first register is a "equal to"
// JEQ r1 r2 r3
#define JEQ 0x8

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
// LRC r1 #103
#define LRC 0xB

// Binary ANDs the second and third register values
// and stores the value in the first register
// AND r1 r2 r3
#define AND 0xC

// Binary ORs the second and third register values
// and stores the value in the first register
// OR r1 r2 r3
#define OR 0xD

// Binary XORs the second and third register values
// and stores the value in the first register
// XOR r1 r2 r3
#define XOR 0xE

#endif
