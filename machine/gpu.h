#ifndef GPU_H_
#define GPU_H_

#include "constants.h"
#include <stdint.h>

typedef struct GPU GPU;

// Sprite Attribute
// Describes a sprite and how it should be rendered
typedef struct 
{
    unsigned int active : 1;
    unsigned int flipHor : 1;
    unsigned int flipVer : 1;
    unsigned int color4Alpha : 1;
    unsigned int fullWidth : 1;
    unsigned int fullHeight : 1;
    // 2 bits here for use later
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
    int bytesPerPixel;
    unsigned int active : 1;
    unsigned int refreshed : 1; // Changes 0 -> 1 or 1 -> 0 when the display refreshes
};

GPU* createGPU(void);

void destroyGPU(GPU* gpu);

void updateGPU(GPU* gpu, uint8_t memory[MEMORY_SEGMENT_COUNT][MEMORY_SEGMENT_SIZE]);

void readSpritesFromMem(GPU* gpu, uint8_t memory[MEMORY_SEGMENT_COUNT][MEMORY_SEGMENT_SIZE]);

void drawSprites(GPU* gpu, uint8_t memory[MEMORY_SEGMENT_COUNT][MEMORY_SEGMENT_SIZE]);

uint8_t getRed(uint8_t paletteIndex);
uint8_t getGreen(uint8_t paletteIndex);
uint8_t getBlue(uint8_t paletteIndex);


#endif

