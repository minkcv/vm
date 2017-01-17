#include "vm.h"
#include "opcodes.h"
#include "instruction.h"
#include <stdlib.h>
#include <stdio.h>

VM* createVM(int* code)
{
    VM* vm = (VM*)malloc(sizeof(VM));
    vm->code = code;
    vm->pc = code;
    return vm;
}

void run(VM* vm)
{
    printf("Starting execution\n");
    while (1)
    {
        int* instr = vm->pc;
        Instruction* decoded = decode(instr);
        exec(vm, decoded);
        vm->pc++;
    }
}

Instruction* decode(int* instr)
{
    Instruction* decoded = malloc(sizeof(Instruction));
    int clean = 0x00FF;
    decoded->opcode = *instr >> 12;
    decoded->arg0 = *instr >> 8 & clean;
    decoded->arg1 = *instr >> 4 & clean;
    decoded->arg2 = *instr & clean;
    return decoded;
}

void exec(VM* vm, Instruction* instr)
{
    if (instr->opcode == HALT)
    {
        printf("Exiting\n");
        exit(0);
    }
    else if (instr->opcode == ADD)
    {
        printf("Adding values %d and %d from registers %d and %d into %d\n", vm->regs[instr->arg0], vm->regs[instr->arg1], instr->arg0, instr->arg1, instr->arg2);
        vm->regs[instr->arg0] = vm->regs[instr->arg1] + vm->regs[instr->arg2];
    }
    else if (instr->opcode == LRC)
    {
        int constant = (instr->arg1 << 4) & 0x00F0;
        constant += instr->arg2;
        printf("Loading the constant %d into reg %d\n", constant, instr->arg0);
        vm->regs[instr->arg0] = constant;
    }
}
