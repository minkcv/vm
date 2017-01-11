#include "vm.h"
#include "opcodes.h"
#include "instruction.h"

VM* createVM(long* code)
{
    VM* vm = (VM*)malloc(sizeof(VM));
    vm->memory = 
    vm->code = code;
    vm->pc = code;
    return vm;
}

void run(VM* vm)
{
    while (true)
    {
        long* instr = vm->pc;
        Instruction* decoded = decode(instr);
        exec(vm, decoded);
        vm->pc++;
    }
}

Instruction decode(long* instr)
{
    Instruction decoded;
    long clean = 0x000000FF;
    decoded.opcode = instr >> 24;
    decoded.arg0 = instr >> 16 & clean;
    decoded.arg1 = instr >> 8 & clean;
    decoded.arg2 = instr & clean;
    return decoded;
}

void exec(VM* vm, Instruction* instr)
{
    if (instr->opcode == HALT)
    {
        exit(0);
    }
    else if (instr->opcode == ADD)
    {

    }
}
