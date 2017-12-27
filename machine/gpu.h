#ifndef GPU_H_
#define GPU_H_

#include "constants.h"
#include "vm.h"
#include "display.h"
#include <stdint.h>
#include <SDL.h>

typedef struct VM VM;
typedef struct GPU GPU;
typedef struct Display Display;

// Sprite Attribute
// Describes a sprite and how it should be rendered
typedef struct 
{
    unsigned int active : 1;
    unsigned int flipHor : 1;
    unsigned int flipVer : 1;
    unsigned int color4Alpha : 1;
    // 4 bits here for use later
    uint8_t x;
    uint8_t y;
    uint8_t width;
    uint8_t height;
    uint8_t segmentAddr;
    uint8_t byteAddr;
    uint8_t colors[4];
} SpriteAttr;

// Graphics Processing Unit
// Reads System memory and draws sprites / background color to the display back buffer
struct GPU
{
    SpriteAttr sprAttrs[NUM_SPRITES];
    uint8_t* pixels;
    int pitch;
    unsigned int active : 1;
    unsigned int refreshed : 1; // Changes 0 -> 1 or 1 -> 0 when the display refreshes
};

GPU* createGPU(Display* display);

void updateGPU(GPU* gpu, uint8_t memory[MEMORY_SEGMENT_COUNT][MEMORY_SEGMENT_SIZE]);

void readSpritesFromMem(GPU* gpu, uint8_t memory[MEMORY_SEGMENT_COUNT][MEMORY_SEGMENT_SIZE]);

void drawSprites(GPU* gpu, uint8_t memory[MEMORY_SEGMENT_COUNT][MEMORY_SEGMENT_SIZE]);


#endif
