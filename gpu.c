#include "gpu.h"
#include <stdlib.h>

GPU* createGPU(SDL_Surface* back)
{
    GPU* gpu = (GPU*)malloc(sizeof(GPU));
    gpu->back = back;
    return gpu;
}

void readSpritesFromMem(GPU* gpu, uint8_t memory[MEMORY_SEGMENT_COUNT][MEMORY_SEGMENT_SIZE])
{
    int i;
    for (i = 0; i < NUM_SPRITES; i++)
    {
        uint8_t flags = memory[ADDR_SPRITE_ATTR][i * SPRITE_ATTR_LENGTH];
        gpu->sprAttrs[i].active = flags >> 7;
        gpu->sprAttrs[i].flipHor = flags >> 6 & 0x1;
        gpu->sprAttrs[i].flipVer = flags >> 5 & 0x1;
        gpu->sprAttrs[i].color4Alpha = flags >> 4 & 0x1;
        gpu->sprAttrs[i].x = memory[ADDR_SPRITE_ATTR][i * SPRITE_ATTR_LENGTH + 1];
        gpu->sprAttrs[i].y = memory[ADDR_SPRITE_ATTR][i * SPRITE_ATTR_LENGTH + 2];
        gpu->sprAttrs[i].width = memory[ADDR_SPRITE_ATTR][i * SPRITE_ATTR_LENGTH + 3];
        gpu->sprAttrs[i].height = memory[ADDR_SPRITE_ATTR][i * SPRITE_ATTR_LENGTH + 4];
        gpu->sprAttrs[i].segmentAddr = memory[ADDR_SPRITE_ATTR][i * SPRITE_ATTR_LENGTH + 5];
        gpu->sprAttrs[i].byteAddr = memory[ADDR_SPRITE_ATTR][i * SPRITE_ATTR_LENGTH + 6];
        gpu->sprAttrs[i].colors[0] = memory[ADDR_SPRITE_ATTR][i * SPRITE_ATTR_LENGTH + 7];
        gpu->sprAttrs[i].colors[1] = memory[ADDR_SPRITE_ATTR][i * SPRITE_ATTR_LENGTH + 8];
        gpu->sprAttrs[i].colors[2] = memory[ADDR_SPRITE_ATTR][i * SPRITE_ATTR_LENGTH + 9];
        gpu->sprAttrs[i].colors[3] = memory[ADDR_SPRITE_ATTR][i * SPRITE_ATTR_LENGTH + 10];
    }
}

void drawSprites(GPU* gpu, uint8_t memory[MEMORY_SEGMENT_COUNT][MEMORY_SEGMENT_SIZE])
{
    int i;
    SDL_LockSurface(gpu->back);
    uint8_t* pixels = (uint8_t*)gpu->back->pixels;
    for (i = 0; i < NUM_SPRITES; i++)
    {
        if (gpu->sprAttrs[i].active)
        {
            int w, h;
            int x = gpu->sprAttrs[i].x;
            int y = gpu->sprAttrs[i].y;
            // TODO: flipping
            uint8_t* sprite = &memory[gpu->sprAttrs[i].segmentAddr][gpu->sprAttrs[i].byteAddr];
            printf("sprite segment: %d\n", gpu->sprAttrs[i].segmentAddr);
            printf("sprite byte: %d\n\n", gpu->sprAttrs[i].byteAddr);
            printf("width: %d\n", gpu->sprAttrs[i].width);
            printf("height: %d\n\n", gpu->sprAttrs[i].height);
            for (h = 0; h < gpu->sprAttrs[i].height; h++)
            {
                for (w = 0; w < gpu->sprAttrs[i].width / 4; w++)
                {
                    uint8_t fourPixels = *(sprite + w + (h * gpu->sprAttrs[i].height));
                    uint8_t nibble1 = fourPixels & 0x2;
                    uint8_t nibble2 = (fourPixels >> 2) & 0x2;
                    uint8_t nibble3 = (fourPixels >> 4) & 0x2;
                    uint8_t nibble4 = (fourPixels >> 6) & 0x2;
                    uint8_t pixel1 = gpu->sprAttrs[i].colors[nibble1];
                    uint8_t pixel2 = gpu->sprAttrs[i].colors[nibble2];
                    uint8_t pixel3 = gpu->sprAttrs[i].colors[nibble3];
                    uint8_t pixel4 = gpu->sprAttrs[i].colors[nibble4];
                    uint8_t* curPixel = (uint8_t*)(pixels + x + (y * gpu->back->pitch) + (w * 4) + (h * gpu->back->pitch));
                    // TODO: alpha
                    *curPixel = pixel1;
                    *(curPixel + 1) = pixel2;
                    *(curPixel + 2) = pixel3;
                    *(curPixel + 3) = pixel4;
                }
            }
        }
    }
    SDL_UnlockSurface(gpu->back);
}
