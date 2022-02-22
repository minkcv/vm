#include "cpu.h"
#include "opcodes.h"
#include "instruction.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

void disassemble(Instruction* instr, char* assembly)
{
    // If the instructions reads the last 2 arguments as a constant
    uint8_t constant = ((instr->arg1 << 4) & 0x00F0) + instr->arg2;
    switch(instr->opcode)
    {
        case EXT:
            switch(instr->arg0)
            {
                case EXT_HALT:
                    sprintf(assembly, "HALT");
                    break;
                case EXT_CPY:
                    sprintf(assembly, "CPY r%02d r%02d", instr->arg1, instr->arg2);
                    break;
                case EXT_NOT:
                    sprintf(assembly, "NOT %02d %02d", instr->arg1, instr->arg2);
                    break;
                case EXT_LSL:
                    sprintf(assembly, "LSL r%02d r%02d", instr->arg1, instr->arg2);
                    break;
                case EXT_LSR:
                    sprintf(assembly, "LSR r%02d r%02d", instr->arg1, instr->arg2);
                    break;
                case EXT_JMP:
                    sprintf(assembly, "JMP r%02d r%02d", instr->arg1, instr->arg2);
                    break;
                case EXT_NOP:
                    sprintf(assembly, "NOP");
                    break;
                case EXT_BRKP:
                    sprintf(assembly, "BRKP");
                    break;
            }
            break;
        case ADD:
            sprintf(assembly, "ADD r%02d r%02d r%02d", instr->arg0, instr->arg1, instr->arg2);
            break;
        case SUB:
            sprintf(assembly, "SUB r%02d r%02d r%02d", instr->arg0, instr->arg1, instr->arg2);
            break;
        case ADDC:
            sprintf(assembly, "ADDC r%02d #%03d ; (0x%04X)", instr->arg0, constant, constant);
            break;
        case SUBC:
            sprintf(assembly, "SUBC r%02d #%03d ; (0x%04X)", instr->arg0, constant, constant);
            break;
        case CMP:
            sprintf(assembly, "CMP r%02d r%02d r%02d", instr->arg0, instr->arg1, instr->arg2);
            break;
        case JLT:
            sprintf(assembly, "JLT r%02d r%02d r%02d", instr->arg0, instr->arg1, instr->arg2);
            break;
        case JGT:
            sprintf(assembly, "JGT r%02d r%02d r%02d", instr->arg0, instr->arg1, instr->arg2);
            break;
        case JEQ:
            sprintf(assembly, "JEQ r%02d r%02d r%02d", instr->arg0, instr->arg1, instr->arg2);
            break;
        case LDR:
            sprintf(assembly, "LDR r%02d r%02d r%02d", instr->arg0, instr->arg1, instr->arg2);
            break;
        case STR:
            sprintf(assembly, "STR r%02d r%02d r%02d", instr->arg0, instr->arg1, instr->arg2);
            break;
        case LRC:
            sprintf(assembly, "LRC r%02d #%03d ; (0x%04X)", instr->arg0, constant, constant);
            break;
        case AND:
            sprintf(assembly, "AND r%02d r%02d r%02d", instr->arg0, instr->arg1, instr->arg2);
            break;
        case OR:
            sprintf(assembly, "OR r%02d r%02d r%02d", instr->arg0, instr->arg1, instr->arg2);
            break;
        case XOR:
            sprintf(assembly, "XOR r%02d r%02d r%02d", instr->arg0, instr->arg1, instr->arg2);
            break;
    }
}

// Decodes a uint16_t instruction into a 16 bit instruction struct
void decode(uint16_t instr, Instruction* decoded)
{
    uint16_t clean = 0x000F;
    decoded->opcode = instr >> 12 & clean;
    decoded->arg0 = instr >> 8 & clean;
    decoded->arg1 = instr >> 4 & clean;
    decoded->arg2 = instr & clean;
}

// Executes an instruction
void exec(CPU* cpu, Instruction* instr, uint8_t memory[MEMORY_SEGMENT_COUNT][MEMORY_SEGMENT_SIZE])
{
    switch(instr->opcode)
    {
        case EXT:
            switch(instr->arg0)
            {
                case EXT_HALT:
                    printf("Exiting at halt instruction (instruction %ld)\n", (cpu->pc - cpu->code));
                    exit(0);
                    break;
                case EXT_CPY:
                    cpu->regs[instr->arg1] = cpu->regs[instr->arg2];
                    break;
                case EXT_NOT:
                    cpu->regs[instr->arg1] = ~(cpu->regs[instr->arg2]);
                    break;
                case EXT_LSL:
                    cpu->regs[instr->arg1] = cpu->regs[instr->arg1] << cpu->regs[instr->arg2];
                    break;
                case EXT_LSR:
                    cpu->regs[instr->arg1] = cpu->regs[instr->arg1] >> cpu->regs[instr->arg2];
                    break;
                case EXT_JMP:
                    cpu->pc = cpu->code + (cpu->regs[instr->arg1] * JUMP_SEGMENT_SIZE) + cpu->regs[instr->arg2] - 1;
                    break;
                case EXT_NOP:
                    break;
                case EXT_BRKP:
                    if (cpu->debugMode)
                    {
                        cpu->breakState = 1;
                        printf("Entering break state\n");
                    }
                    break;
            }
            break;
        case ADD:
            cpu->regs[instr->arg0] = cpu->regs[instr->arg1] + cpu->regs[instr->arg2];
            break;
        case SUB:
            cpu->regs[instr->arg0] = cpu->regs[instr->arg1] - cpu->regs[instr->arg2];
            break;
        case ADDC:
            cpu->regs[instr->arg0] += ((instr->arg1 << 4) & 0x00F0) + instr->arg2;
            break;
        case SUBC:
            cpu->regs[instr->arg0] -= ((instr->arg1 << 4) & 0x00F0) + instr->arg2;
            break;
        case CMP:
            if (cpu->regs[instr->arg1] < cpu->regs[instr->arg2])
                cpu->regs[instr->arg0] = 0;
            else if (cpu->regs[instr->arg1] > cpu->regs[instr->arg2])
                cpu->regs[instr->arg0] = 2;
            else
                cpu->regs[instr->arg0] = 1;
            break;
        case JLT:
            if (cpu->regs[instr->arg0] == 0)
                cpu->pc = cpu->code + (cpu->regs[instr->arg1] * JUMP_SEGMENT_SIZE) + cpu->regs[instr->arg2] - 1;
            break;
        case JGT:
            if (cpu->regs[instr->arg0] == 2)
                cpu->pc = cpu->code + (cpu->regs[instr->arg1] * JUMP_SEGMENT_SIZE) + cpu->regs[instr->arg2] - 1;
            break;
        case JEQ:
            if (cpu->regs[instr->arg0] == 1)
                cpu->pc = cpu->code + (cpu->regs[instr->arg1] * JUMP_SEGMENT_SIZE) + cpu->regs[instr->arg2] - 1;
            break;
        case LDR:
            cpu->regs[instr->arg0] = memory[cpu->regs[instr->arg1]][cpu->regs[instr->arg2]];
            break;
        case STR:
            if (cpu->regs[instr->arg1] < 128) // Segment is not part of ROM
                memory[cpu->regs[instr->arg1]][cpu->regs[instr->arg2]] = cpu->regs[instr->arg0];
            else
            {
                printf("Attempted illegal write to ROM\n");
                exit(1);
            }
            break;
        case LRC:
            cpu->regs[instr->arg0] = ((instr->arg1 << 4) & 0x00F0) + instr->arg2;
            break;
        case AND:
            cpu->regs[instr->arg0] = cpu->regs[instr->arg1] & cpu->regs[instr->arg2];
            break;
        case OR:
            cpu->regs[instr->arg0] = cpu->regs[instr->arg1] | cpu->regs[instr->arg2];
            break;
        case XOR:
            cpu->regs[instr->arg0] = cpu->regs[instr->arg1] ^ cpu->regs[instr->arg2];
            break;
    }
}

void print_disasm_block(CPU *cpu) {
    // Disassemble next 16 instructions
    int i;
    for (i = 0; i < 16; i++)
    {
	int instructionCount = cpu->pc - cpu->code + i;
	if (instructionCount > JUMP_SEGMENT_SIZE * JUMP_SEGMENT_SIZE)
	  continue; // End of program, can't keep reading instructions.
	int instructionSegment = instructionCount / JUMP_SEGMENT_SIZE;
	int instructionOffset = instructionCount % JUMP_SEGMENT_SIZE;
	char assembly[256];
	memset(assembly, 0, sizeof(char) * 256);
	Instruction decoded;
	decode(*(cpu->pc + i), &decoded);
	disassemble(&decoded, assembly);
	// Print star for current instruction
	if (i == 0)
	  printf("*");
	else
	  printf(" ");

	// Print disassembly with segment.offset address
	printf(" %-30s ; %03d.%03d 0x%02X.0x%02X\n", assembly,
	       instructionSegment, instructionOffset,
	       instructionSegment, instructionOffset);
      }
}

void print_cpu_state(CPU *cpu)
{
    // Print program counter as segment.offset, disassembly of current instruction, and registers
    int instructionCount = cpu->pc - cpu->code;
    int instructionSegment = instructionCount / JUMP_SEGMENT_SIZE;
    int instructionOffset = instructionCount % JUMP_SEGMENT_SIZE;
    printf("PC:  %03d.%03d 0x%02X.0x%02X\n", 
	   instructionSegment, instructionOffset,
	   instructionSegment, instructionOffset);
    char assembly[256];
    memset(assembly, 0, sizeof(char) * 256);
    Instruction decoded;
    decode(*(cpu->pc), &decoded);
    disassemble(&decoded, assembly);
    printf("ASM: %s\n", assembly);
    int i;
    for (i = 0; i < REGISTER_COUNT; i++)
	printf("r%02d: %03d (0x%02X)\n", i, cpu->regs[i], cpu->regs[i]);
}
