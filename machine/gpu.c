#include "gpu.h"
#include <stdlib.h>

GPU* createGPU(SDL_Surface* back)
{
    GPU* gpu = (GPU*)malloc(sizeof(GPU));
    gpu->back = back;
    gpu->active = 1;
    return gpu;
}

void updateGPU(GPU* gpu, uint8_t memory[MEMORY_SEGMENT_COUNT][MEMORY_SEGMENT_SIZE])
{
    gpu->active = memory[GPU_FLAG_SEG][GPU_FLAG_OFFSET] & 0x1;
    if (gpu->active)
    {
        // Preserve other bits, only change the second bit
        gpu->refreshed = !gpu->refreshed;
        memory[GPU_FLAG_SEG][GPU_FLAG_OFFSET] &= 0xFD; // Clear the second bit
        memory[GPU_FLAG_SEG][GPU_FLAG_OFFSET] |= (gpu->refreshed << 1); // Set the second bit according to gpu->refreshed
    }
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
                gpu->sprAttrs[i].x + gpu->sprAttrs[i].width <= SCREEN_WIDTH &&
                gpu->sprAttrs[i].y + gpu->sprAttrs[i].height <= SCREEN_HEIGHT)
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
                    uint8_t fourPixels = *(sprite + w + (h * gpu->sprAttrs[i].width / 4));
                    uint8_t bits1 = (fourPixels >> 6) & 0x3; // MSB
                    uint8_t bits2 = (fourPixels >> 4) & 0x3;
                    uint8_t bits3 = (fourPixels >> 2) & 0x3;
                    uint8_t bits4 = fourPixels & 0x3;
                    uint8_t pixel1 = gpu->sprAttrs[i].colors[bits1];
                    uint8_t pixel2 = gpu->sprAttrs[i].colors[bits2];
                    uint8_t pixel3 = gpu->sprAttrs[i].colors[bits3];
                    uint8_t pixel4 = gpu->sprAttrs[i].colors[bits4];
                    uint8_t* curPixel = (uint8_t*)(pixels + x + (y * gpu->back->pitch) + (w * 4) + (h * gpu->back->pitch));
                    if (!(gpu->sprAttrs[i].color4Alpha && bits1 == 0x3))
                        *curPixel = pixel1;
                    if (!(gpu->sprAttrs[i].color4Alpha && bits2 == 0x3))
                        *(curPixel + 1) = pixel2;
                    if (!(gpu->sprAttrs[i].color4Alpha && bits3 == 0x3))
                        *(curPixel + 2) = pixel3;
                    if (!(gpu->sprAttrs[i].color4Alpha && bits4 == 0x3))
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
