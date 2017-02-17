#include "vm.h"
#include "opcodes.h"
#include "instruction.h"
#include <SDL.h>
#include <stdlib.h>
#include <stdio.h>

// Creates a vm with the supplied code
// display is an optional pointer to a display struct
VM* createVM(uint16_t* code, Display* display)
{
    VM* vm = (VM*)malloc(sizeof(VM));
    vm->code = code;
    vm->pc = code;
    vm->display = display;
    vm->gpu = createGPU(display->back);
    vm->ipu = createIPU();
    memset(vm->memory, 0, sizeof(vm->memory[0][0]) * MEMORY_SEGMENT_COUNT * MEMORY_SEGMENT_SIZE);
    return vm;
}

// Starts the fetch, decode, execute cycle
// of a vm from the start of its code
void run(VM* vm)
{
    printf("Starting execution\n");
    uint32_t startTime = SDL_GetTicks();
    uint32_t displayWaitTime = 16; // 16ms = about 60 refreshes per second
    SDL_Event event;
    while (event.type != SDL_QUIT)
    {
        if (event.type == SDL_KEYDOWN || event.type == SDL_KEYUP)
        {
            updateIPU(vm->ipu, event.key, vm->memory);
        }
        uint16_t* instr = vm->pc;
        Instruction* decoded = decode(instr);
        exec(vm, decoded);
        vm->pc++;

        if (startTime + displayWaitTime < SDL_GetTicks())
        {
            drawBackground(vm->gpu, vm->memory);
            readSpritesFromMem(vm->gpu, vm->memory);
            drawSprites(vm->gpu, vm->memory);
            updateDisplay(vm->display);
            startTime = SDL_GetTicks();
        }
        SDL_Delay(1);
        SDL_PollEvent(&event);
    }
}

// Decodes a uint16_t instruction into a 16 bit instruction struct
Instruction* decode(uint16_t* instr)
{
    Instruction* decoded = malloc(sizeof(Instruction));
    uint16_t clean = 0x00FF;
    decoded->opcode = *instr >> 12;
    decoded->arg0 = *instr >> 8 & clean;
    decoded->arg1 = *instr >> 4 & clean;
    decoded->arg2 = *instr & clean;
    return decoded;
}

// Executes an instruction
void exec(VM* vm, Instruction* instr)
{
    if (instr->opcode == HALT)
    {
        printf("Exiting\n");
        exit(0);
    }
    else if (instr->opcode == ADD)
    {
        vm->regs[instr->arg0] = vm->regs[instr->arg1] + vm->regs[instr->arg2];
    }
    else if (instr->opcode == SUB)
    {
        vm->regs[instr->arg0] = vm->regs[instr->arg1] - vm->regs[instr->arg2];
    }
    else if (instr->opcode == CMP)
    {
        //printf("value in second arg: %d third: %d\n", vm->regs[instr->arg1], vm->regs[instr->arg2]);
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
            vm->pc = vm->code + ((instr->arg1 << 4) + instr->arg2) - 1;
    }
    else if (instr->opcode == JGT)
    {
        if (vm->regs[instr->arg0] == 2)
            vm->pc = vm->code + ((instr->arg1 << 4) + instr->arg2) - 1;
    }
    else if (instr->opcode == JEQ)
    {
        //printf("value in first arg: %d\n", vm->regs[instr->arg0]);
        if (vm->regs[instr->arg0] == 1)
            vm->pc = vm->code + ((instr->arg1 << 4) + instr->arg2) - 1;
    }
    else if (instr->opcode == JMP)
    {
        vm->pc = vm->code + ((instr->arg1 << 4) + instr->arg2) - 1;
    }
    else if (instr->opcode == CPY)
    {
        vm->regs[instr->arg0] = vm->regs[instr->arg1];
    }
    else if (instr->opcode == LDR)
    {
        //printf("Loading the register %d with the value at memory address: %d-%d\n", instr->arg0, vm->regs[instr->arg1], vm->regs[instr->arg2]);
        vm->regs[instr->arg0] = vm->memory[vm->regs[instr->arg1]][vm->regs[instr->arg2]];
    }
    else if (instr->opcode == STR)
    {
        //printf("Storing the value in register %d (%d) into the memory address %d-%d\n", instr->arg0, vm->regs[instr->arg0], vm->regs[instr->arg1], vm->regs[instr->arg2]);
        vm->memory[vm->regs[instr->arg1]][vm->regs[instr->arg2]] = vm->regs[instr->arg0];
    }
    else if (instr->opcode == LRC)
    {
        int constant = (instr->arg1 << 4) & 0x00F0;
        constant += instr->arg2;
        //printf("Loading the constant %d into reg %d\n", constant, instr->arg0);
        vm->regs[instr->arg0] = constant;
    }
}
