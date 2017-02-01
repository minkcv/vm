#include <SDL.h>

#ifndef DISPLAY_H_
#define DISPLAY_H_

SDL_Surface* initDisplay();

void updateDisplay(SDL_Surface* screen);

void drawBackBuffer(SDL_Surface* back, SDL_Surface* screen);

void quitDisplay();

#endif
