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
    memset(vm->regs, 0, sizeof(vm->regs[0]) * REGISTER_COUNT);
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
    SDL_PollEvent(&event);
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
            free(decoded);
            if (cpuInstructionCount > 500000)
                wait = 1;
        }
        else // Waiting
        {
            if ((SDL_GetTicks() - cpuStartTime) > 1000)
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
    switch(instr->opcode)
    {
        case EXT:
            switch(instr->arg0)
            {
                case EXT_HALT:
                    printf("Exiting at halt instruction\n");
                    exit(0);
                    break;
                case EXT_CPY:
                    vm->regs[instr->arg1] = vm->regs[instr->arg2];
                    break;
                case EXT_NOT:
                    vm->regs[instr->arg1] = ~(vm->regs[instr->arg2]);
                    break;
                case EXT_LSL:
                    vm->regs[instr->arg1] = vm->regs[instr->arg1] << vm->regs[instr->arg2];
                    break;
                case EXT_LSR:
                    vm->regs[instr->arg1] = vm->regs[instr->arg1] >> vm->regs[instr->arg2];
                    break;
                case EXT_JMP:
                    vm->pc = vm->code + (vm->regs[instr->arg1] * JUMP_SEGMENT_SIZE) + vm->regs[instr->arg2] - 1;
                    break;
                case EXT_NOP:
                    break;
            }
            break;
        case ADD:
            vm->regs[instr->arg0] = vm->regs[instr->arg1] + vm->regs[instr->arg2];
            break;
        case SUB:
            vm->regs[instr->arg0] = vm->regs[instr->arg1] - vm->regs[instr->arg2];
            break;
        case ADDC:
            vm->regs[instr->arg0] += ((instr->arg1 << 4) & 0x00F0) + instr->arg2;
            break;
        case SUBC:
            vm->regs[instr->arg0] -= ((instr->arg1 << 4) & 0x00F0) + instr->arg2;
            break;
        case CMP:
            if (vm->regs[instr->arg1] < vm->regs[instr->arg2])
                vm->regs[instr->arg0] = 0;
            else if (vm->regs[instr->arg1] > vm->regs[instr->arg2])
                vm->regs[instr->arg0] = 2;
            else
                vm->regs[instr->arg0] = 1;
            break;
        case JLT:
            if (vm->regs[instr->arg0] == 0)
                vm->pc = vm->code + (vm->regs[instr->arg1] * JUMP_SEGMENT_SIZE) + vm->regs[instr->arg2] - 1;
            break;
        case JGT:
            if (vm->regs[instr->arg0] == 2)
                vm->pc = vm->code + (vm->regs[instr->arg1] * JUMP_SEGMENT_SIZE) + vm->regs[instr->arg2] - 1;
            break;
        case JEQ:
            if (vm->regs[instr->arg0] == 1)
                vm->pc = vm->code + (vm->regs[instr->arg1] * JUMP_SEGMENT_SIZE) + vm->regs[instr->arg2] - 1;
            break;
        case LDR:
            vm->regs[instr->arg0] = vm->memory[vm->regs[instr->arg1]][vm->regs[instr->arg2]];
            break;
        case STR:
            if (vm->regs[instr->arg1] < 128) // Segment is not part of ROM
                vm->memory[vm->regs[instr->arg1]][vm->regs[instr->arg2]] = vm->regs[instr->arg0];
            else
            {
                printf("Attempted illegal write to ROM\n");
                exit(1);
            }
            break;
        case LRC:
            vm->regs[instr->arg0] = ((instr->arg1 << 4) & 0x00F0) + instr->arg2;
            break;
        case AND:
            vm->regs[instr->arg0] = vm->regs[instr->arg1] & vm->regs[instr->arg2];
            break;
        case OR:
            vm->regs[instr->arg0] = vm->regs[instr->arg1] | vm->regs[instr->arg2];
            break;
        case XOR:
            vm->regs[instr->arg0] = vm->regs[instr->arg1] ^ vm->regs[instr->arg2];
            break;
    }
}
