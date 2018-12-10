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
    // Debugging
    int debugMode;
    int breakState;
    int step;
};

VM* createVM(uint16_t* code, uint8_t* rom, Display* display, int debugMode);

void destroyVM(VM* vm);

void run(VM* vm);

void handleDebugKey(VM* vm, SDL_Keycode key);

void disassemble(Instruction* instr, char* assembly);

void decode(uint16_t instr, Instruction* decoded);

void exec(VM* vm, Instruction* instr);

#endif

