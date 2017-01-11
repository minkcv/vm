#ifndef VM_H_
#define VM_H_

// Segment size in bytes
#define MEMORY_SEGMENT_SIZE 1024
// Number of segments
#define MEMORY_SEGMENT_COUNT 256

#define REGISTER_COUNT 16

typedef struct
{
    int regs[REGISTER_COUNT]; // General purpose registers
    char memory[MEMORY_SEGMENT_COUNT][MEMORY_SEGMENT_SIZE];
    long* pc; // Program counter / instruction pointer
    long* code; // List of instructions
} VM;

VM* createVM(long* code, int pc);

void run(VM* vm);

char* decode(long* instr);

void exec(VM* vm, char* instr);

#endif
