#include "vm.h"
#include "opcodes.h"
#include "instruction.h"
#include <SDL.h>
#include <stdlib.h>
#include <stdio.h>

// Creates a vm with the supplied code
// display is an optional pointer to a display struct
VM* createVM(uint16_t* code, uint8_t* rom, Display* display)
{
    VM* vm = (VM*)malloc(sizeof(VM));
    vm->code = code;
    vm->pc = code;
    vm->display = display;
    vm->gpu = createGPU(display->back);
    vm->ipu = createIPU();
    memset(vm->memory, 0, sizeof(vm->memory[0][0]) * MEMORY_SEGMENT_COUNT * MEMORY_SEGMENT_SIZE);
    if (rom != NULL) // ROM is optional
        memcpy(vm->memory[128], rom, 128 * MEMORY_SEGMENT_SIZE);
    return vm;
}

// Starts the fetch, decode, execute cycle
// of a vm from the start of its code
void run(VM* vm)
{
    uint32_t displayStartTime = SDL_GetTicks();
    uint32_t displayWaitTime = 16; // 16ms = about 60 refreshes per second
    uint32_t cpuStartTime = SDL_GetTicks();
    uint32_t cpuInstructionCount = 0;
    int wait = 0;
    SDL_Event event;
    while (event.type != SDL_QUIT)
    {
        if (event.key.keysym.sym == SDLK_ESCAPE)
        {
            exit(0);
        }
        if (event.type == SDL_KEYDOWN || event.type == SDL_KEYUP)
        {
            updateIPU(vm->ipu, event.key, vm->memory);
        }

        // Executing
        if (!wait)
        {
            uint16_t* instr = vm->pc;
            Instruction* decoded = decode(instr);
            exec(vm, decoded);
            vm->pc++;
            cpuInstructionCount++;
        }
        else // Waiting
        {
            if ((SDL_GetTicks() - cpuStartTime) > 1000 && cpuInstructionCount > 500000)
            {
                // Stop waiting
                cpuInstructionCount = 0;
                cpuStartTime = SDL_GetTicks();
                wait = 0;
            }
        }
        if (displayStartTime + displayWaitTime < SDL_GetTicks())
        {
            updateGPU(vm->gpu, vm->memory);
            if (vm->gpu->active)
            {
                drawBackground(vm->gpu, vm->memory);
                readSpritesFromMem(vm->gpu, vm->memory);
                drawSprites(vm->gpu, vm->memory);
                updateDisplay(vm->display);
            }
            displayStartTime = SDL_GetTicks();
        }
        SDL_PollEvent(&event);
    }
}

// Decodes a uint16_t instruction into a 16 bit instruction struct
Instruction* decode(uint16_t* instr)
{
    Instruction* decoded = malloc(sizeof(Instruction));
    uint16_t clean = 0x000F;
    decoded->opcode = *instr >> 12 & clean;
    decoded->arg0 = *instr >> 8 & clean;
    decoded->arg1 = *instr >> 4 & clean;
    decoded->arg2 = *instr & clean;
    return decoded;
}

// Executes an instruction
void exec(VM* vm, Instruction* instr)
{
    if (instr->opcode == EXT)
    {
        if (instr->arg0 == EXT_HALT)
        {
            printf("Exiting at halt instruction\n");
            exit(0);
        }
        else if (instr->arg0 == EXT_CPY)
        {
            vm->regs[instr->arg1] = vm->regs[instr->arg2];
        }
        else if (instr->arg0 == EXT_NOT)
        {
            vm->regs[instr->arg1] = ~(vm->regs[instr->arg2]);
        }
        else if (instr->arg0 == EXT_LSL)
        {
            vm->regs[instr->arg0] = vm->regs[instr->arg0] << vm->regs[instr->arg1];
        }
        else if (instr->arg0 == EXT_LSR)
        {
            vm->regs[instr->arg0] = vm->regs[instr->arg0] >> vm->regs[instr->arg1];
        }
        else if (instr->arg0 == EXT_JMP)
        {
            vm->pc = vm->code + (vm->regs[instr->arg1] * JUMP_SEGMENT_SIZE) + vm->regs[instr->arg2] - 1;
        }
        else if (instr->arg0 == EXT_NOP)
        {
            // No operation
        }
    }
    else if (instr->opcode == ADD)
    {
        vm->regs[instr->arg0] = vm->regs[instr->arg1] + vm->regs[instr->arg2];
    }
    else if (instr->opcode == SUB)
    {
        vm->regs[instr->arg0] = vm->regs[instr->arg1] - vm->regs[instr->arg2];
    }
    else if (instr->opcode == ADDC)
    {
        vm->regs[instr->arg0] += ((instr->arg1 << 4) & 0x00F0) + instr->arg2;
    }
    else if (instr->opcode == SUBC)
    {
        vm->regs[instr->arg0] -= ((instr->arg1 << 4) & 0x00F0) + instr->arg2;
    }
    else if (instr->opcode == CMP)
    {
        if (vm->regs[instr->arg1] < vm->regs[instr->arg2])
            vm->regs[instr->arg0] = 0;
        else if (vm->regs[instr->arg1] > vm->regs[instr->arg2])
            vm->regs[instr->arg0] = 2;
        else
            vm->regs[instr->arg0] = 1;
    }
    else if (instr->opcode == JLT)
    {
        if (vm->regs[instr->arg0] == 0)
            vm->pc = vm->code + (vm->regs[instr->arg1] * JUMP_SEGMENT_SIZE) + vm->regs[instr->arg2] - 1;
    }
    else if (instr->opcode == JGT)
    {
        if (vm->regs[instr->arg0] == 2)
            vm->pc = vm->code + (vm->regs[instr->arg1] * JUMP_SEGMENT_SIZE) + vm->regs[instr->arg2] - 1;
    }
    else if (instr->opcode == JEQ)
    {
        if (vm->regs[instr->arg0] == 1)
        {
            vm->pc = vm->code + (vm->regs[instr->arg1] * JUMP_SEGMENT_SIZE) + vm->regs[instr->arg2] - 1;
        }
    }
    else if (instr->opcode == LDR)
    {
        vm->regs[instr->arg0] = vm->memory[vm->regs[instr->arg1]][vm->regs[instr->arg2]];
    }
    else if (instr->opcode == STR)
    {
        if (vm->regs[instr->arg1] < 128) // Segment is not part of ROM
            vm->memory[vm->regs[instr->arg1]][vm->regs[instr->arg2]] = vm->regs[instr->arg0];
        else
        {
            printf("Attempted illegal write to ROM\n");
            exit(1);
        }
    }
    else if (instr->opcode == LRC)
    {
        vm->regs[instr->arg0] = ((instr->arg1 << 4) & 0x00F0) + instr->arg2;
    }
    else if (instr->opcode == AND)
    {
        vm->regs[instr->arg0] = vm->regs[instr->arg1] & vm->regs[instr->arg2];
    }
    else if (instr->opcode == OR)
    {
        vm->regs[instr->arg0] = vm->regs[instr->arg1] | vm->regs[instr->arg2];
    }
    else if (instr->opcode == XOR)
    {
        vm->regs[instr->arg0] = vm->regs[instr->arg1] ^ vm->regs[instr->arg2];
    }
}
