#include "machine/opcodes.h"
#include "machine/constants.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <stdint.h>

#define MAX_LABELS 1024

// Used to map a label to an address
typedef struct
{
    uint8_t segAddr;
    uint8_t offsetAddr;
    char* label;
} LabelAddress;

int countInstructions(FILE* src);
void getAddressesOfLabel(char* label, LabelAddress* labelMap[MAX_LABELS], uint8_t* segAddr, uint8_t* offsetAddr);
void createLabelMap(FILE* src, LabelAddress*** labelMap);

int main (int argc, char** argv)
{
    char* filename = NULL;
    int i;
    for (i = 1; i < argc; i++)
    {
        if (!strcmp(argv[i], "-f"))
        {
            if (i + 1 < argc)
                filename = argv[i + 1];
        }
    }
    if (filename == NULL)
    {
        printf("Usage: assember -f sourcefile.asm\n");
        exit(1);
    }


    FILE* src = fopen(filename, "r");
    if (src)
    {
        uint16_t numInstructions = countInstructions(src); 
        printf("counted %d instructions\n", numInstructions);
        uint16_t* binary = malloc(sizeof(uint16_t) * numInstructions);
        char* line = NULL; // Causes getline to use malloc, we free this for every line
        size_t toRead = 0; // Passing 0 to getline reads until \n
        ssize_t charsRead;
        int instrIndex = 0;
        int lineCount = 0; // Used for error printing
        LabelAddress** labelMap = malloc(sizeof(LabelAddress*) * MAX_LABELS);
        createLabelMap(src, &labelMap);

        // Read each line in the file
        while ((charsRead = getline(&line, &toRead, src)) != -1)
        {
            lineCount++;
            // If line is not a command and we haven't reached the end of the non comment code.
            if (line[0] != '\n' && line[0] != ';' && line[0] != '@' && instrIndex < numInstructions)
            {
                // Parse tokens separated by a space or comment
                char* token = strtok(line, " ;\n");
                uint16_t instr; // Assembled instruction
                int arg = 0; // Which arg we are currently parsing
                while (token != NULL && arg < 4)
                {
                    if (arg == 0)
                    {
                        if (!strcmp(token, "LRL")) // Special assembler macro "Load Registers Label"
                        {
                            uint8_t segAddr = 0;
                            uint8_t offsetAddr = 0;
                            uint8_t reg1;
                            uint8_t reg2;
                            token = strtok(NULL, " "); // Get the next token
                            if (token != NULL && token[0] == 'r')
                                reg1 = atoi(token + 1);
                            token = strtok(NULL, " "); // Get the next token
                            if (token != NULL && token[0] == 'r')
                                reg2 = atoi(token + 1);
                            token = strtok(NULL, " "); // Get the next token
                            if (token[0] == '@')
                            {
                                char* label = token;
                                strsep(&label, "@");
                                char* labelName = strsep(&label, " ;\n"); // Trim any comments or other text off the end
                                getAddressesOfLabel(labelName, labelMap, &segAddr, &offsetAddr);
                                instr = LRC << 12;
                                instr |= reg1 << 8;
                                instr |= segAddr;
                                binary[instrIndex] = instr; // Save the assembled instruction
                                instrIndex++;
                                instr = LRC << 12;
                                instr |= reg2 << 8;
                                instr |= offsetAddr;
                            }
                            arg += 3; // Parsed the entire instruction here
                        }
                        else if (!strcmp(token, "HALT"))
                        {
                            instr = EXT << 12;
                            instr |= EXT_HALT << 8;
                            arg++;
                        }
                        else if (!strcmp(token, "CPY"))
                        {
                            instr = EXT << 12;
                            instr |= EXT_CPY << 8;
                            arg++;
                        }
                        else if (!strcmp(token, "NOT"))
                        {
                            instr = EXT << 12;
                            instr |= EXT_NOT << 8;
                            arg++;
                        }
                        else if (!strcmp(token, "LSL"))
                        {
                            instr = EXT << 12;
                            instr |= EXT_LSL << 8;
                            arg++;
                        }
                        else if (!strcmp(token, "LSR"))
                        {
                            instr = EXT << 12;
                            instr |= EXT_LSR << 8;
                            arg++;
                        }
                        else if (!strcmp(token, "JMP"))
                        {
                            instr = EXT << 12;
                            instr |= EXT_JMP << 8;
                            arg++;
                        }
                        else if (!strcmp(token, "NOP"))
                        {
                            instr = EXT << 12;
                            instr |= EXT_NOP << 8;
                            arg++;
                        }
                        else if (!strcmp(token, "ADD"))
                            instr = ADD << 12;
                        else if (!strcmp(token, "SUB"))
                            instr = SUB << 12;
                        else if (!strcmp(token, "ADDC"))
                            instr = ADDC << 12;
                        else if (!strcmp(token, "SUBC"))
                            instr = SUBC << 12;
                        else if (!strcmp(token, "CMP"))
                            instr = CMP << 12;
                        else if (!strcmp(token, "JLT"))
                            instr = JLT << 12;
                        else if (!strcmp(token, "JGT"))
                            instr = JGT << 12;
                        else if (!strcmp(token, "JEQ"))
                            instr = JEQ << 12;
                        else if (!strcmp(token, "LDR"))
                            instr = LDR << 12;
                        else if (!strcmp(token, "STR"))
                            instr = STR << 12;
                        else if (!strcmp(token, "LRC"))
                            instr = LRC << 12;
                        else if (!strcmp(token, "AND"))
                            instr = AND << 12;
                        else if (!strcmp(token, "OR"))
                            instr = OR << 12;
                        else if (!strcmp(token, "XOR"))
                            instr = XOR << 12;
                        else
                        {
                            printf("Not a known instruction on line %d\n", lineCount);
                            exit(1);
                        }
                        arg++;
                    }
                    else if (arg < 4)
                    {
                        if (token[0] == 'r')
                        {
                            int shift = 0;
                            if (arg == 1)
                                shift = 8;
                            if (arg == 2)
                                shift = 4;
                            if (arg == 3)
                                shift = 0;
                            instr |= (atoi(token + 1) << shift);
                        }
                        else if (token[0] == '#' || token[0] == '$') // Token is a constant
                        {
                            char* num = token;
                            uint8_t constant;
                            if (token[0] == '#') // A constant in base 10
                            {
                                // Remove the # from the string
                                strsep(&num, "#");
                                // Convert to integer
                                constant = (uint8_t)atoi(num);
                            }
                            else if (token[0] == '$')
                            {
                                // Remove the $ from the string
                                strsep(&num, "$");
                                // Convert to base 16
                                constant = (uint8_t)strtol(num, NULL, 16);
                            }
                            instr |= constant;
                            arg++;
                        }
                        arg++;
                    }
                    token = strtok(NULL, " "); // Get the next token
                }
                binary[instrIndex] = instr; // Save the assembled instruction
                instrIndex++;
            }
            // Free and null the line pointer. getline will reallocate for us
            free(line);
            line = NULL;
        }

        // Free the label map
        int i;
        for (i = 0; i < MAX_LABELS; i++)
        {
            if (labelMap[i] != NULL)
            {
                if (labelMap[i]->label != NULL)
                {
                    free(labelMap[i]->label);
                }
                free(labelMap[i]);
            }
        }
        free(labelMap);

        char* filenameNoExtension = strsep(&filename, ".");
        if (filenameNoExtension != NULL)
            filename = filenameNoExtension;

        // Now write the binary array out to a file with .bin appended to the name
        FILE* bin = fopen(strcat(filename, ".bin"), "wb");
        // Prefix binary with length
        fwrite(&numInstructions, sizeof(uint16_t), 1, bin);
        size_t written = fwrite(binary, sizeof(uint16_t), numInstructions, bin);
        fclose(bin);
        free(binary);
        binary = NULL;
        // Check that the instructions were written correctly
        if (written != numInstructions)
        {
            printf("Error writing binary file\n");
        }
        else
        {
            printf("%d instructions written to %s\n", written, filename);
        }
    }
    else
    {
        printf("Failed to open file %s\n", filename);
    }
    return 0;
}

// Counts lines that don't start with a ; (comment) or @ (label)
// 2 instructions for LRL macro
int countInstructions(FILE* src)
{
    char* line = NULL;
    size_t toRead = 0; // Read until \n
    ssize_t charsRead;
    uint16_t instrCount = 0;
    while ((charsRead = getline(&line, &toRead, src)) != -1)
    {
        if (line[0] != '\n' && line[0] != '@' && line[0] != ';')
        {
            char* token = strtok(line, " ");
            if (!strcmp(token, "LRL"))
                instrCount++;
            instrCount++;
        }
        free(line);
        line = NULL; // Causes getline to keep reading
    }
    rewind(src);
    return instrCount;
}

void createLabelMap(FILE* src, LabelAddress*** labelMap)
{
    char* line = NULL;
    size_t toRead = 0; // Read until \n
    ssize_t charsRead;
    int labelCount = 0;
    uint16_t instrCount = 0;
    while ((charsRead = getline(&line, &toRead, src)) != -1)
    {
        if(line[0] == '@')
        {
            // Create the label entry in labelMap
            char* start = line;
            char* beforeAt = strsep(&line, "@");
            char* labelName = strsep(&line, " ;\n"); // Trim any comments or other text off the end
            LabelAddress* labeladdr = malloc(sizeof(LabelAddress));
            labeladdr->label = malloc(sizeof(char) * (strlen(labelName) + 1));
            strcpy(labeladdr->label, labelName);
            labeladdr->segAddr = instrCount / JUMP_SEGMENT_SIZE;
            labeladdr->offsetAddr = instrCount % JUMP_SEGMENT_SIZE;
            (*labelMap)[labelCount] = labeladdr;
            labelCount++;
            free(start);
            start = NULL;
            labelName = NULL;
            beforeAt = NULL;
            line = NULL;
        }
        else if (line[0] != ';' && line[0] != '\n')
        {
            char* token = strtok(line, " ");
            if (!strcmp(token, "LRL"))
                instrCount++;
            instrCount++;
            free(line);
            line = NULL;
        }
        else
        {
            free(line);
            line = NULL;
        }
    }
    rewind(src);
}

void getAddressesOfLabel(char* label, LabelAddress* labelMap[MAX_LABELS], uint8_t* segAddr, uint8_t* offsetAddr)
{
    int i;
    for (i = 0; i < MAX_LABELS; i++)
    {
        LabelAddress* current = labelMap[i];
        if (current != NULL && current->label != NULL)
        {
            if (!strcmp(current->label, label))
            {
                *segAddr = current->segAddr;
                *offsetAddr = current->offsetAddr;
                return;
            }
        }
    }
    printf("Error: Label \"%s\" not defined\n", label);
    exit(1);
}
