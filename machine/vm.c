#include "vm.h"
#include "opcodes.h"
#include "instruction.h"
#include <SDL.h>
#include <stdlib.h>
#include <stdio.h>

// Creates a vm with the supplied code
VM* createVM(uint16_t* code, uint8_t* rom, Display* display, int debugMode)
{
    VM* vm = (VM*)malloc(sizeof(VM));
    vm->cpu.code = code;
    vm->cpu.pc = code;
    vm->cpu.debugMode = debugMode;
    vm->cpu.breakState = 0;
    vm->cpu.step = 0;
    vm->display = display;
    vm->gpu = createGPU();
    vm->ipu = createIPU();
    memset(vm->memory, 0, sizeof(vm->memory[0][0]) * MEMORY_SEGMENT_COUNT * MEMORY_SEGMENT_SIZE);
    memset(vm->cpu.regs, 0, sizeof(vm->cpu.regs[0]) * REGISTER_COUNT);
    if (rom != NULL) // ROM is optional
        memcpy(vm->memory[128], rom, 128 * MEMORY_SEGMENT_SIZE);
    return vm;
}

void destroyVM(VM* vm)
{
    destroyGPU(vm->gpu);
    vm->gpu = NULL;
    destroyIPU(vm->ipu);
    vm->ipu = NULL;
    free(vm);
    vm = NULL;
}

// Starts the fetch, decode, execute cycle
// of a vm from the start of its code
void run(VM* vm)
{
    uint32_t displayStartTime = SDL_GetTicks();
    uint32_t displayWaitTime = 16; // 16ms = about 60 refreshes per second
    uint32_t cpuStartTime = SDL_GetTicks();
    uint32_t cpuInstructionCount = 0;

    // Enforce the instructions per second limit in sync with the display refreshes
    // 500,000 instructions per second is almost the same as 8064 instructions per 16 milliseconds.
    // 500,000 IPS == 8000 instructions / 0.016 seconds
    // 500,000 / 8064 = 62
    uint32_t instructionsPerSecondFactor = 62;

    int wait = 0;
    SDL_Event event;
    SDL_PollEvent(&event);
    Instruction decoded;
    while (event.type != SDL_QUIT)
    {
        while (SDL_PollEvent(&event))
        {
            if (event.key.keysym.sym == SDLK_ESCAPE && event.type == SDL_KEYUP)
            {
                printf("Exiting at user escape\n");
                exit(0);
            }
            if (vm->cpu.debugMode && event.type == SDL_KEYUP)
            {
                handleDebugKey(vm, event.key.keysym.sym);
            }
            if (event.type == SDL_KEYDOWN || event.type == SDL_KEYUP)
            {
                if (event.key.repeat == 0)
                {
                    updateIPU(vm->ipu, event.key, vm->memory);
                }
            }
        }

        if (vm->cpu.breakState)
            continue;

        // Executing
        if (!wait)
        {
            uint16_t instr = *(vm->cpu.pc);
            decode(instr, &decoded);
            exec(&vm->cpu, &decoded, vm->memory);
            vm->cpu.pc++;
            cpuInstructionCount++;

            if (cpuInstructionCount > INSTRUCTIONS_PER_SECOND / instructionsPerSecondFactor)
                // We have done 8064 instructions in this segment of 16 milliseconds. Don't do any more until the 16ms have passed.
                wait = 1;
        }
        if ((SDL_GetTicks() - cpuStartTime) > 1000 / instructionsPerSecondFactor)
        {
            // 16 milliseconds have passed
            // Reset the instruction count and timer
            if (cpuInstructionCount < INSTRUCTIONS_PER_SECOND / instructionsPerSecondFactor)
            {
                //printf("Running below desired instructions per second\n");
                //printf("Desired: %d Actual: %d\n", INSTRUCTIONS_PER_SECOND / instructionsPerSecondFactor, cpuInstructionCount);
            }
            cpuInstructionCount = 0;
            cpuStartTime = SDL_GetTicks();
            wait = 0;
        }
        if (displayStartTime + displayWaitTime < SDL_GetTicks())
        {
            updateGPU(vm->gpu, vm->memory);
            if (vm->gpu->active)
            {
                readSpritesFromMem(vm->gpu, vm->memory);
                drawSprites(vm->gpu, vm->memory);
                updateDisplay(vm->display, vm->gpu);
            }
            displayStartTime = SDL_GetTicks();
        }
        if (vm->cpu.step)
            vm->cpu.breakState = 1;
    }
}

void handleDebugKey(VM* vm, SDL_Keycode key)
{
    if (key == SDLK_F2)
    {
        // Continue
        vm->cpu.breakState = 0;
        vm->cpu.step = 0;
        printf("Resuming execution\n");
    }
    if (key == SDLK_F3)
    {
        // Break all
        vm->cpu.breakState = 1;
        printf("Entering break state\n");
    }
    if (key == SDLK_F4)
    {
        // Step
        vm->cpu.breakState = 0;
        vm->cpu.step = 1;
        printf("Stepping\n");
    }
    if (key == SDLK_F9)
    {
	print_cpu_state(&vm->cpu);
    }
    if (key == SDLK_F10)
    {
        // Dump memory segments 0 through 63 (inclusive)
        int segment;
        int offset;
        int rowLength = 16;
        for (segment = 0; segment < 64; segment++)
        {
            for (offset = 0; offset < 256; offset++)
            {
                if (offset % rowLength == 0)
                    printf("\n0x%02X.0x%02X: 0x", segment, offset);
                printf("%02X", vm->memory[segment][offset]);
            }
        }
        printf("\n");
    }
    if (key == SDLK_F11)
    {
        // Dump memory segments 64 through 127 (inclusive)
        int segment;
        int offset;
        int rowLength = 16;
        for (segment = 64; segment < 128; segment++)
        {
            for (offset = 0; offset < 256; offset++)
            {
                if (offset % rowLength == 0)
                    printf("\n0x%02X.0x%02X: 0x", segment, offset);
                printf("%02X", vm->memory[segment][offset]);
            }
        }
        printf("\n");
    }
    if (key == SDLK_F12)
    {
	// Disassemble next 16 instructions
        print_disasm_block(&vm->cpu);
    }
}
