#ifndef GPU_H_
#define GPU_H_

#include "constants.h"
#include "vm.h"
#include <stdint.h>
#include <SDL.h>

typedef struct VM VM;
typedef struct GPU GPU;

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
    SDL_Surface* back; // Screen back buffer
};

GPU* createGPU(SDL_Surface* back);

void readSpritesFromMem(GPU* gpu, uint8_t memory[MEMORY_SEGMENT_COUNT][MEMORY_SEGMENT_SIZE]);

void drawSprites(GPU* gpu, uint8_t memory[MEMORY_SEGMENT_COUNT][MEMORY_SEGMENT_SIZE]);

void drawBackground(GPU* gpu, uint8_t memory[MEMORY_SEGMENT_COUNT][MEMORY_SEGMENT_SIZE]);


#endif
