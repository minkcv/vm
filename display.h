#include <SDL.h>

#ifndef DISPLAY_H_
#define DISPLAY_H_

// Color depth aka bits per pixel
#define COLOR_DEPTH 8
// Number of colors in the color palette, directly related to the above number
#define NUM_COLORS 256

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
