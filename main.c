#include "constants.h"
#include "instruction.h"
#include "opcodes.h"
#include "vm.h"
#include "display.h"
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

uint16_t* readBinary(const char* filename, int print);

int main (char argc, char** argv)
{
    char* filename = NULL;
    int index;
    int c;
    int scale = 1;
    opterr = 0;
    while ((c = getopt(argc, argv, "hf:s:")) != -1)
    {
        switch(c)
        {
            case 'f':
                filename = optarg;
                break;
            case 's':
                scale = atoi(optarg);
                break;
            default:
            case 'h':
                printf("Usage: main -f program.bin\n");
                break;
                exit(1);
        }
    }
    if (filename == NULL)
    {
        printf("Usage: main -f program.bin\n");
        exit(1);
    }

    Display* display = createDisplay(SCREEN_WIDTH, SCREEN_HEIGHT, scale);
    uint16_t* code = readBinary(filename, 0);
    VM* vm = createVM(code, display);
    run(vm);
    quitDisplay(display);
    free(code);
    code = NULL;
    return 0;
}

uint16_t* readBinary(const char* filename, int print)
{
    FILE* bin = fopen(filename, "rb");
    if (bin == NULL)
    {
        printf("Error reading file %s\n", filename);
        exit(1);
    }
    uint16_t numInstructions = 0;
    // binaries are length prefixed
    fread(&numInstructions, sizeof(uint16_t), 1, bin);
    printf("Binary is %d instructions\n", numInstructions);
    uint16_t* code = malloc(sizeof(uint16_t) * numInstructions);
    fread(code, sizeof(uint16_t), numInstructions, bin);
    if (print)
    {
        printf("Binary Program:\n");
        int i;
        for (i = 0; i < numInstructions; i++)
        {
            printf("%4X\n", code[i]);
        }
    }
    fclose(bin);
    return code;
}
