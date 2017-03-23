#ifndef DISPLAY_H_
#define DISPLAY_H_

#include "constants.h"
#include <SDL.h>

typedef struct
{
    SDL_Surface* screen;
    SDL_Surface* back; // Double buffering
    SDL_Color* colors; // Colors of the color palette
    int ncolors;
    int width;
    int height;
    int scale;
} Display;

Display* createDisplay(int width, int height, int scale);

void updateDisplay(Display* display);

void dumpColors(SDL_Color* colors, int ncolors);

void quitDisplay(Display* display);

#endif
