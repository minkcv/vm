#include "display.h"

Display* createDisplay(int width, int height, int scale)
{
    SDL_Init(SDL_INIT_VIDEO);
    SDL_WM_SetCaption("Virtual Machine", "Virtual Machine");

    Display* display = (Display*)malloc(sizeof(Display));
    display->screen = SDL_SetVideoMode(width * scale, height * scale, COLOR_DEPTH, 0);
    display->back = SDL_CreateRGBSurface(0, width, height, COLOR_DEPTH, 0, 0, 0, 0);
    SDL_Palette* palette = display->screen->format->palette;
    display->colors = palette->colors;
    display->ncolors = palette->ncolors;
    SDL_SetPalette(display->back, 0, display->colors, 0, display->ncolors);
    SDL_Surface* temp = SDL_LoadBMP("sdl_logo.bmp");
    display->back = SDL_DisplayFormat(temp);
    SDL_FreeSurface(temp);

    return display;
}

void updateDisplay(Display* display)
{
    SDL_BlitSurface(display->back, NULL, display->screen, NULL);
    SDL_UpdateRect(display->screen, 0, 0, 0, 0);
}

void dumpColors(SDL_Color* colors, int ncolors)
{
    printf("Number of colors: %d\n", ncolors);
    int i;
    for (i = 0; i < ncolors; i++)
    {
        SDL_Color c = colors[i];
        printf("Color: %3d, R: %d, G: %d, B: %d\n", 
                i, c.r, c.g, c.b);
    }
}

void quitDisplay(Display* display)
{
    SDL_FreeSurface(display->screen);
    display->screen = NULL;
    SDL_FreeSurface(display->back);
    display->back = NULL;
    free(display);
    display = NULL;
    SDL_Quit();
}
