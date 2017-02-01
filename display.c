#include "display.h"

SDL_Surface* initDisplay()
{
    SDL_Init(SDL_INIT_VIDEO);
    SDL_WM_SetCaption("SDL Test", "SDL Test");
    SDL_Surface* screen = SDL_SetVideoMode(640, 480, 0, 0);

    return screen;
}

void updateDisplay(SDL_Surface* screen)
{
    SDL_UpdateRect(screen, 0, 0, 0, 0);
}

void drawBackBuffer(SDL_Surface* back, SDL_Surface* screen)
{
    SDL_BlitSurface(back, NULL, screen, NULL);
}

void quitDisplay()
{
    SDL_Quit();
}
