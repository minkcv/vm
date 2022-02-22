#ifndef VM_H_
#define VM_H_

#include "constants.h"
#include "instruction.h"
#include "display.h"
#include "gpu.h"
#include "cpu.h"
#include "ipu.h"
#include <stdint.h>
#include <SDL.h>

typedef struct VM VM;
typedef struct GPU GPU;
typedef struct Display Display;

struct VM
{
    struct CPU cpu;
    // Memory is indexed by segment and then byte
    uint8_t memory[MEMORY_SEGMENT_COUNT][MEMORY_SEGMENT_SIZE];
    GPU* gpu;
    IPU* ipu;
    Display* display;
};

VM* createVM(uint16_t* code, uint8_t* rom, Display* display, int debugMode);

void destroyVM(VM* vm);

void run(VM* vm);

void handleDebugKey(VM* vm, SDL_Keycode key);

void disassemble(Instruction* instr, char* assembly);

#endif

