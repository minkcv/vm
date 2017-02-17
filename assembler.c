#include "opcodes.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <limits.h>
#include <stdint.h>

#define MAX_LABELS 256

// Used to map a label to an address
typedef struct
{
    uint16_t address;
    char* label;
} LabelAddress;

int countLines(FILE* src);
uint16_t getAddressOfLabel(char* label, LabelAddress* labelMap[MAX_LABELS]);
void createLabelMap(FILE* src, LabelAddress*** labelMap);

int main (int argc, char** argv)
{
    char* filename = NULL;
    int index;
    int c;
    opterr = 0;
    while ((c = getopt(argc, argv, "hf:")) != -1)
    {
        switch(c)
        {
            case 'f':
                filename = optarg;
                break;
            case 'h':
            default:
                printf("Usage: assember -f sourcefile.asm\n");
                exit(1);
                break;
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
        // Number of non comment and non label lines in the file
        uint16_t numInstructions = countLines(src); 
        uint16_t* binary = malloc(sizeof(uint16_t) * numInstructions);
        char* line = NULL; // Causes getline to use malloc, we free this for every line
        size_t toRead = 0; // Passing 0 to getline reads until \n
        ssize_t charsRead;
        int instrIndex = 0;
        int lineCount = 0;
        LabelAddress** labelMap = malloc(sizeof(LabelAddress*) * MAX_LABELS);
        createLabelMap(src, &labelMap);

        // Read each line in the file
        while ((charsRead = getline(&line, &toRead, src)) != -1)
        {
            lineCount++;
            // If line is not a command and we haven't reached the end of the non comment code.
            if (line[0] != ';' && line[0] != '@' && instrIndex < numInstructions)
            {
                // Parse tokens separated by a space
                char* token = strtok(line, " ");
                uint16_t instr; // Assembled instruction
                int arg = 0; // Which arg we are currently parsing
                while (token != NULL && arg < 4)
                {
                    if (arg == 0)
                    {
                        if (!strcmp(token, "HALT"))
                            instr = HALT << 12;
                        else if (!strcmp(token, "ADD"))
                            instr = ADD << 12;
                        else if (!strcmp(token, "SUB"))
                            instr = SUB << 12;
                        else if (!strcmp(token, "CMP"))
                            instr = CMP << 12;
                        else if (!strcmp(token, "JLT"))
                            instr = JLT << 12;
                        else if (!strcmp(token, "JGT"))
                            instr = JGT << 12;
                        else if (!strcmp(token, "JEQ"))
                            instr = JEQ << 12;
                        else if (!strcmp(token, "JMP"))
                            instr = JMP << 12;
                        else if (!strcmp(token, "CPY"))
                            instr = CPY << 12;
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
                        else if (!strcmp(token, "NOT"))
                            instr = NOT << 12;
                        else if (!strcmp(token, "SHF"))
                            instr = SHF << 12;
                        else
                        {
                            printf("Not a known instruction on line %d\n", lineCount);
                            exit(1);
                        }
                    }
                    else if (arg == 1)
                    {
                        if (token[0] == 'r')
                            instr |= (atoi(token + 1) << 8);
                        else if (token[0] == '@')
                        {
                            char* label = token;
                            strsep(&label, "@");
                            int16_t address = getAddressOfLabel(label, labelMap);
                            instr |= address;
                            arg += 2; // Skip the rest of the args
                        }
                    }
                    else if (arg == 2)
                    {
                        if (token[0] == 'r') // Token is a register
                            instr |= (atoi(token + 1) << 4);
                        else if (token[0] == '#') // Token is a constant in base 10
                        {
                            // Remove the # from the string
                            char* num = token;
                            strsep(&num, "#");
                            // Convert to integer and append to instruction
                            instr |= atoi(num);
                            arg++; // Skip the next arg, constants take up 2 args
                        }
                        else if (token[0] == '$') // Token is a constant in base 16 (hex)
                        {
                            // Remove the $ from the string
                            char* num = token;
                            strsep(&num, "$");
                            // Convert to base 16 and append to instruction
                            instr |= (uint8_t)strtol(num, NULL, 16);
                            arg++; // Skip the next arg
                        }
                        else if (token[0] == '@')
                        {
                            char* label = token;
                            strsep(&label, "@");
                            int16_t address = getAddressOfLabel(label, labelMap);
                            instr |= address;
                            arg++; // Skip the next arg
                        }
                    }
                    else if (arg == 3)
                    {
                        if (token[0] == 'r')
                            instr |= atoi(token + 1);
                    }
                    arg++;
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
int countLines(FILE* src)
{
    char c;
    int count = 0;
    int lastnew = 1;
    int skip = 0;
    while (fscanf(src, "%c", &c) != EOF)
    {
        if (lastnew && (c == ';' || c == '@'))
        {
            skip = 1;
        }
        else if(lastnew)
        {
            skip = 0;
        }

        lastnew = 0;
        if (c == '\n')
        {
            if (!skip)
                count++;
            lastnew = 1;
        }
    }
    rewind(src);
    return count;
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
            char* labelName = strsep(&line, " "); // Trim any comments or other text off the end
            LabelAddress* labeladdr = malloc(sizeof(LabelAddress));
            labeladdr->label = malloc(sizeof(char) * (strlen(labelName) + 1));
            strcpy(labeladdr->label, labelName);
            labeladdr->address = instrCount;
            (*labelMap)[labelCount] = labeladdr;
            labelCount++;
            printf("Label %s maps to address %d\n", labelName, instrCount + 1);
            free(start);
            start = NULL;
            labelName = NULL;
            beforeAt = NULL;
            line = NULL;
        }
        else if (line[0] != ';')
        {
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

uint16_t getAddressOfLabel(char* label, LabelAddress* labelMap[MAX_LABELS])
{
    int i;
    for (i = 0; i < MAX_LABELS; i++)
    {
        LabelAddress* current = labelMap[i];
        if (current != NULL && current->label != NULL)
        {
            if (!strcmp(current->label, label))
            {
                return current->address;
            }
        }
    }
    printf("Label \"%s\" not defined\n", label);
    exit(1);
}
