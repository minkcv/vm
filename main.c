#include "machine/constants.h"
#include "machine/instruction.h"
#include "machine/opcodes.h"
#include "machine/vm.h"
#include "machine/display.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <retro_endianness.h>

uint16_t* readBinary(const char* filename, int print, uint8_t **rom);
uint8_t* readRom(const char* filename, int print);

int main (int argc, char** argv)
{
    char* programName = NULL;
    char* romName = NULL;
    int scale = 1;
    int debugMode = 0;
    int i;
    for (i = 1; i < argc; i++)
    {
        if (!strcmp(argv[i], "-f"))
        {
            if (i + 1 < argc)
                programName = argv[i + 1];
        }
        else if (!strcmp(argv[i], "-r"))
        {
            if (i + 1 < argc)
                romName = argv[i + 1];
        }
        else if (!strcmp(argv[i], "-s"))
        {
            if (i + 1 < argc)
                scale = atoi(argv[i + 1]);
        }
        else if (!strcmp(argv[i], "-d"))
            debugMode = 1;
    }
    if (programName == NULL)
    {
        printf("Usage: main -f program.vaporbin\n");
        exit(1);
    }

    Display* display = createDisplay(SCREEN_WIDTH, SCREEN_HEIGHT, scale, SCREEN_WIDTH / 4);
    uint8_t* rom = NULL;
    uint16_t* code = readBinary(programName, 0, &rom);
    if (romName != NULL) // ROM is optional
        rom = readRom(romName, 0);

    VM* vm = createVM(code, rom, display, debugMode);
    run(vm);
    destroyVM(vm);
    free(code);
    if (rom != NULL)
        free(rom);
    code = NULL;
    rom = NULL;
    quitDisplay(display);
    return 0;
}

uint16_t* readBinary(const char* filename, int print, uint8_t **rom)
{
    FILE* bin = fopen(filename, "rb");
    if (bin == NULL)
    {
        printf("Error reading file %s\n", filename);
        exit(1);
    }
    size_t numInstructions = 0, numInstructionsLe = 0;
    // binaries are length prefixed
    fread(&numInstructionsLe, sizeof(uint16_t), 1, bin);
    numInstructions = retro_le_to_cpu16(numInstructionsLe);
    printf("Binary is %Id instructions\n", numInstructions);
    uint16_t* code = malloc(sizeof(uint16_t) * numInstructions);
    fread(code, sizeof(uint16_t), numInstructions, bin);
    if (print)
    {
        printf("Binary Program:\n");
        int i;
        for (i = 0; i < numInstructions; i++)
            printf("%4X\n", code[i]);
    }

    size_t romSize = 0, romSizeLe = 0;
    // binaries are length prefixed
    if (fread(&romSizeLe, sizeof(uint16_t), 1, bin) == 1)
    {
	romSize = retro_le_to_cpu16(romSizeLe);
	if (romSize > 128 * MEMORY_SEGMENT_SIZE)
	    romSize = 128 * MEMORY_SEGMENT_SIZE;
	*rom = malloc(128 * MEMORY_SEGMENT_SIZE);
	memset(*rom, 0, 128 * MEMORY_SEGMENT_SIZE);
	size_t bytesRead = fread(*rom, sizeof(uint8_t), romSize, bin);
	printf("Read %Id bytes from the embed rom\n", bytesRead);
	if (print)
	{
	    printf("Rom:\n");
	    int i;
	    for (i = 0; i < bytesRead; i++)
		printf("%2X\n", rom[i]);
	}
    }

    fclose(bin);
    return code;
}

uint8_t* readRom(const char* filename, int print)
{
    FILE* romfile = fopen(filename, "rb");
    if (romfile == NULL)
    {
        printf("Error reading file %s\n", filename);
        exit(1);
    }
    uint8_t* rom = malloc(sizeof(uint8_t) * 128 * MEMORY_SEGMENT_SIZE);
    memset(rom, 0, sizeof(uint8_t) * 128 * MEMORY_SEGMENT_SIZE);
    size_t bytesRead = fread(rom, sizeof(uint8_t), 128 * MEMORY_SEGMENT_SIZE, romfile);
    printf("Read %Id bytes from the rom\n", bytesRead);
    if (print)
    {
        printf("Rom:\n");
        int i;
        for (i = 0; i < bytesRead; i++)
            printf("%2X\n", rom[i]);
    }
    fclose(romfile);
    return rom;
}
