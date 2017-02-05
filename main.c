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

    Display* display = createDisplay(320, 240, 1);
    SDL_Event event;
    //dumpColors(display->colors, display->ncolors);
    while (event.type != SDL_QUIT)
    {
        SDL_PollEvent(&event);
        updateDisplay(display);
    }
    quitDisplay(display);

    uint16_t* code = readBinary(filename, 1);
    VM* vm = createVM(code);
    run(vm);
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
    uint16_t* code = malloc(sizeof(uint16_t) * numInstructions);
    fread(code, sizeof(uint16_t), numInstructions, bin);
    if (print)
    {
        printf("Binary Program:\n");
        int i;
        for (i = 0; i < numInstructions; i++)
        {
            printf("%x\n", code[i]);
        }
    }
    fclose(bin);
    return code;
}
