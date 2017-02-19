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
        uint8_t flags = memory[SPRITE_ATTR_SEG][i * SPRITE_ATTR_LENGTH];
        gpu->sprAttrs[i].active = flags >> 7;
        gpu->sprAttrs[i].flipHor = flags >> 6 & 0x1;
        gpu->sprAttrs[i].flipVer = flags >> 5 & 0x1;
        gpu->sprAttrs[i].color4Alpha = flags >> 4 & 0x1;
        gpu->sprAttrs[i].x = memory[SPRITE_ATTR_SEG][i * SPRITE_ATTR_LENGTH + 1];
        gpu->sprAttrs[i].y = memory[SPRITE_ATTR_SEG][i * SPRITE_ATTR_LENGTH + 2];
        gpu->sprAttrs[i].width = memory[SPRITE_ATTR_SEG][i * SPRITE_ATTR_LENGTH + 3];
        gpu->sprAttrs[i].height = memory[SPRITE_ATTR_SEG][i * SPRITE_ATTR_LENGTH + 4];
        gpu->sprAttrs[i].segmentAddr = memory[SPRITE_ATTR_SEG][i * SPRITE_ATTR_LENGTH + 5];
        gpu->sprAttrs[i].byteAddr = memory[SPRITE_ATTR_SEG][i * SPRITE_ATTR_LENGTH + 6];
        gpu->sprAttrs[i].colors[0] = memory[SPRITE_ATTR_SEG][i * SPRITE_ATTR_LENGTH + 7];
        gpu->sprAttrs[i].colors[1] = memory[SPRITE_ATTR_SEG][i * SPRITE_ATTR_LENGTH + 8];
        gpu->sprAttrs[i].colors[2] = memory[SPRITE_ATTR_SEG][i * SPRITE_ATTR_LENGTH + 9];
        gpu->sprAttrs[i].colors[3] = memory[SPRITE_ATTR_SEG][i * SPRITE_ATTR_LENGTH + 10];
    }
}

void drawSprites(GPU* gpu, uint8_t memory[MEMORY_SEGMENT_COUNT][MEMORY_SEGMENT_SIZE])
{
    int i;
    SDL_LockSurface(gpu->back);
    uint8_t* pixels = (uint8_t*)gpu->back->pixels;
    for (i = 0; i < NUM_SPRITES; i++)
    {
        if (gpu->sprAttrs[i].active && 
                gpu->sprAttrs[i].x + gpu->sprAttrs[i].width < SCREEN_WIDTH &&
                gpu->sprAttrs[i].y + gpu->sprAttrs[i].height < SCREEN_HEIGHT)
        {
            int w, h;
            int x = gpu->sprAttrs[i].x;
            int y = gpu->sprAttrs[i].y;
            // TODO: flipping
            uint8_t* sprite = &memory[gpu->sprAttrs[i].segmentAddr][gpu->sprAttrs[i].byteAddr];
            for (h = 0; h < gpu->sprAttrs[i].height; h++)
            {
                for (w = 0; w < gpu->sprAttrs[i].width / 4; w++)
                {
                    uint8_t fourPixels = *(sprite + w + (h * gpu->sprAttrs[i].height));
                    uint8_t nibble1 = fourPixels & 0x2;
                    uint8_t nibble2 = (fourPixels >> 2) & 0x3;
                    uint8_t nibble3 = (fourPixels >> 4) & 0x3;
                    uint8_t nibble4 = (fourPixels >> 6) & 0x3;
                    uint8_t pixel1 = gpu->sprAttrs[i].colors[nibble1];
                    uint8_t pixel2 = gpu->sprAttrs[i].colors[nibble2];
                    uint8_t pixel3 = gpu->sprAttrs[i].colors[nibble3];
                    uint8_t pixel4 = gpu->sprAttrs[i].colors[nibble4];
                    uint8_t* curPixel = (uint8_t*)(pixels + x + (y * gpu->back->pitch) + (w * 4) + (h * gpu->back->pitch));
                    if (!(gpu->sprAttrs[i].color4Alpha && pixel1 == gpu->sprAttrs[i].colors[3]))
                        *curPixel = pixel1;
                    if (!(gpu->sprAttrs[i].color4Alpha && pixel2 == gpu->sprAttrs[i].colors[3]))
                        *(curPixel + 1) = pixel2;
                    if (!(gpu->sprAttrs[i].color4Alpha && pixel3 == gpu->sprAttrs[i].colors[3]))
                        *(curPixel + 2) = pixel3;
                    if (!(gpu->sprAttrs[i].color4Alpha && pixel4 == gpu->sprAttrs[i].colors[3]))
                        *(curPixel + 3) = pixel4;
                }
            }
        }
    }
    SDL_UnlockSurface(gpu->back);
}

void drawBackground(GPU* gpu, uint8_t memory[MEMORY_SEGMENT_COUNT][MEMORY_SEGMENT_SIZE])
{
    uint8_t color = memory[BACK_COLOR_SEG][BACK_COLOR_OFFSET];
    SDL_FillRect(gpu->back, NULL, color);
}
