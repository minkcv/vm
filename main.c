#include "instruction.h"
#include "opcodes.h"
#include "vm.h"
#include "display.h"
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

uint16_t* readBinary(const char* filename);

int main (char argc, char** argv)
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

    //SDL_Surface* screen = initDisplay();

    uint16_t* code = readBinary(filename);
    int i;
    VM* vm = createVM(code);
    run(vm);
    free(code);
    code = NULL;
    return 0;
}

uint16_t* readBinary(const char* filename)
{
    FILE* bin = fopen(filename, "rb");
    uint16_t numInstructions = 0;
    // binaries are length prefixed
    fread(&numInstructions, sizeof(uint16_t), 1, bin);
    uint16_t* code = malloc(sizeof(uint16_t) * numInstructions);
    fread(code, sizeof(uint16_t), numInstructions, bin);
    fclose(bin);
    return code;
}
