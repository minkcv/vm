#ifndef VM_H_
#define VM_H_

#include "constants.h"
#include "instruction.h"
#include "display.h"
#include "gpu.h"
#include "ipu.h"
#include <stdint.h>
#include <SDL.h>

typedef struct VM VM;
typedef struct GPU GPU;
typedef struct Display Display;

struct VM
{
    uint8_t regs[REGISTER_COUNT]; // General purpose registers
    // Memory is indexed by segment and then byte
    uint8_t memory[MEMORY_SEGMENT_COUNT][MEMORY_SEGMENT_SIZE];
    uint16_t* pc; // Program counter / instruction pointer
    uint16_t* code; // List of instructions
    GPU* gpu;
    IPU* ipu;
    Display* display;
};

VM* createVM(uint16_t* code, uint8_t* rom, Display* display);

void run(VM* vm);

Instruction* decode(uint16_t* instr);

void exec(VM* vm, Instruction* instr);

#endif

