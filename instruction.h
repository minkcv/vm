#ifndef INSTRUCTION_H_
#define INSTRUCTION_H_

typedef struct
{
    unsigned int opcode : 4;
    unsigned int arg0   : 4;
    unsigned int arg1   : 4;
    unsigned int arg2   : 4;
} Instruction;

#endif
