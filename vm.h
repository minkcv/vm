#include "instruction.h"
#include <stdint.h>
#ifndef VM_H_
#define VM_H_

// Number of cells in a Segment
#define MEMORY_SEGMENT_SIZE 256
// Number of segments
#define MEMORY_SEGMENT_COUNT 256
// 256*256 = 357604 Total memory cells

#define REGISTER_COUNT 16

typedef struct
{
    int8_t regs[REGISTER_COUNT]; // General purpose registers
    // Memory is indexed by segment and then cell
    int8_t memory[MEMORY_SEGMENT_COUNT][MEMORY_SEGMENT_SIZE];
    uint16_t* pc; // Program counter / instruction pointer
    uint16_t* code; // List of instructions
} VM;

VM* createVM(uint16_t* code);

void run(VM* vm);

Instruction* decode(uint16_t* instr);

void exec(VM* vm, Instruction* instr);

#endif
