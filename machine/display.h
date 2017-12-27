#ifndef DISPLAY_H_
#define DISPLAY_H_

#include "constants.h"
#include "gpu.h"
#include <SDL.h>

typedef struct Display Display;
typedef struct GPU GPU;

struct Display
{
    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_Texture* back;
    int width;
    int height;
    int scale;
    int pitch;
};

Display* createDisplay(int width, int height, int scale, int pitch);

void updateDisplay(Display* display, GPU* gpu);

void dumpColors(SDL_Color* colors, int ncolors);

void quitDisplay(Display* display);

#endif
