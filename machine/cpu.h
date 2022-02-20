#ifndef CPU_H_
#define CPU_H_

#include <stdint.h>
#include "constants.h"
#include "instruction.h"

typedef struct CPU CPU;

struct CPU
{
      uint8_t regs[REGISTER_COUNT]; // General purpose registers
      const uint16_t* pc; // Program counter / instruction pointer
      const uint16_t* code; // List of instructions

      // Debugging
      int debugMode;
      int breakState;
      int step;
};

// Disassemble next 16 instructions
void print_disasm_block(CPU *cpu);
// Print program counter as segment.offset, disassembly of current instruction, and registers
void print_cpu_state(CPU *cpu);

void decode(uint16_t instr, Instruction* decoded);

void exec(CPU* cpu, Instruction* instr, uint8_t memory[MEMORY_SEGMENT_COUNT][MEMORY_SEGMENT_SIZE]);

#endif
