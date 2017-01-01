#ifndef VM_H_
#define VM_H_

#define MEMORY_SIZE 1024

typedef struct
{
    int regs[12]; // General purpose registers
    int pc; // Program counter / instruction pointer
    int memory[MEMORY_SIZE];
    int* code; // List of instructions
} VM;

VM* createVM(int* code, int pc)

#endif
