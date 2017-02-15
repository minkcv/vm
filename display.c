#include "display.h"

Display* createDisplay(int width, int height, int scale)
{
    SDL_Init(SDL_INIT_VIDEO);
    SDL_WM_SetCaption("Vapor Spec", "Vapor Spec");

    Display* display = (Display*)malloc(sizeof(Display));
    display->width = width;
    display->height = height;
    display->scale = scale;
    display->screen = SDL_SetVideoMode(width * scale, height * scale, COLOR_DEPTH, 0);
    display->back = SDL_CreateRGBSurface(0, width, height, COLOR_DEPTH, 0, 0, 0, 0);
    SDL_Palette* palette = display->screen->format->palette;
    display->colors = palette->colors;
    display->ncolors = palette->ncolors;
    SDL_Surface* temp = SDL_LoadBMP("title.bmp");
    display->back = SDL_DisplayFormat(temp);
    SDL_FreeSurface(temp);
    updateDisplay(display);
    SDL_Delay(2000); // Show splash screen for 2 seconds
    // Clear the back buffer with the first color in the color palette (black)
    SDL_FillRect(display->back, NULL, 0x00); 

    return display;
}

void updateDisplay(Display* display)
{
    SDL_Delay(16);
    if (display->scale != 1)
    {
        SDL_LockSurface(display->back);
        int s = display->scale;
        int pitch = display->back->pitch;
        int x, y;
        uint8_t* pixels = (uint8_t*)display->back->pixels;
        for (x = 0; x < display->back->w; x++)
        {
            for (y = 0; y < display->back->h; y++)
            {
                SDL_Rect rect = { x * s, y * s, s, s };
                // Draw the rectangle with the color of the pixel in the unscaled surface
                // x is not multiplied by a size because we are using 1 byte per pixel here
                SDL_FillRect(display->screen, &rect, 
                        *(pixels + (y * pitch) + x) & 0xFF);
            }
        }
        SDL_UnlockSurface(display->back);
    }
    else
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
