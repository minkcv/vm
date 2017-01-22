#include "opcodes.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <limits.h>
#include <stdint.h>

int countLines(FILE* src);

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
        uint16_t numInstructions = countLines(src);
        uint16_t* binary = malloc(sizeof(uint16_t) * numInstructions);
        char* line = NULL; // causes getline to use malloc, we free later
        size_t toRead = 0; // Passing 0 to getline reads until \n
        ssize_t charsRead;
        int lineIndex = 0;
        while ((charsRead = getline(&line, &toRead, src)) != -1)
        {
            if (line[0] != ';' && lineIndex < numInstructions)
            {
                char* token = strtok(line, " ");
                uint16_t instr;
                int arg = 0;
                while (token != NULL)
                {
                    if (arg == 0)
                    {
                        if (!strcmp(token, "HALT"))
                            instr = HALT << 12;
                        else if (!strcmp(token, "ADD"))
                            instr = ADD << 12;
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
                        else if (!strcmp(token, "MOV"))
                            instr = MOV << 12;
                        else if (!strcmp(token, "LDR"))
                            instr = LDR << 12;
                        else if (!strcmp(token, "STR"))
                            instr = STR << 12;
                        else if (!strcmp(token, "LRC"))
                            instr = LRC << 12;
                        else
                        {
                            printf("Not a known instruction on line %d\n", lineIndex);
                            exit(1);
                        }
                    }
                    else if (arg == 1)
                    {
                        if (token[0] == 'r')
                            instr |= (atoi(token + 1) << 8);
                    }
                    else if (arg == 2)
                    {
                        if (token[0] == 'r')
                            instr |= (atoi(token + 1) << 4);
                        else 
                        {
                            instr |= atoi(token);
                            arg++;
                        }
                    }
                    else if (arg == 3)
                    {
                        if (token[0] == 'r')
                            instr |= atoi(token + 1);
                    }
                    arg++;
                    token = strtok(NULL, " ");
                }
                binary[lineIndex] = instr;
                lineIndex++;
            }
            // Free and null the line pointer. getline will reallocate for us
            free(line);
            line = NULL;
        }

        // Now write the binary out
        FILE* bin = fopen(strcat(filename, ".bin"), "wb");
        // Prefix binary with length
        fwrite(&numInstructions, sizeof(uint16_t), 1, bin);
        size_t written = fwrite(binary, sizeof(uint16_t), numInstructions, bin);
        int i;
        for (i = 0; i < numInstructions; i++)
            printf("instr: %x\n", binary[i]);

        fclose(bin);
        free(binary);
        binary = NULL;
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

// Counts lines that don't start with a semicolon (comment)
int countLines(FILE* src)
{
    char c;
    int count = 0;
    int lastnew = 1;
    int comment = 0;
    while (fscanf(src, "%c", &c) != EOF)
    {
        if (lastnew && c == ';')
        {
            comment = 1;
        }
        else if(lastnew)
        {
            comment = 0;
        }

        lastnew = 0;
        if (c == '\n')
        {
            if (!comment)
                count++;
            lastnew = 1;
        }
    }
    rewind(src);
    return count;
}
