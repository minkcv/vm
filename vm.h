#include "instruction.h"
#ifndef VM_H_
#define VM_H_

// Segment size in bytes
#define MEMORY_SEGMENT_SIZE 1024
// Number of segments
#define MEMORY_SEGMENT_COUNT 256

#define REGISTER_COUNT 16

typedef struct
{
    int regs[REGISTER_COUNT]; // General purpose registers
    char memory[MEMORY_SEGMENT_COUNT][MEMORY_SEGMENT_SIZE];
    int* pc; // Program counter / instruction pointer
    int* code; // List of instructions
} VM;

VM* createVM(int* code);

void run(VM* vm);

Instruction* decode(int* instr);

void exec(VM* vm, Instruction* instr);

#endif
