#include "display.h"

Display* createDisplay(int width, int height, int scale)
{
    SDL_Init(SDL_INIT_VIDEO);

    Display* display = (Display*)malloc(sizeof(Display));
    display->width = width;
    display->height = height;
    display->scale = scale;
    display->window = SDL_CreateWindow("vapor Spec", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 
            width * scale, height * scale, 0);
    display->renderer = SDL_CreateRenderer(display->window, -1, 0);
    display->back = SDL_CreateTexture(display->renderer, 
            SDL_PIXELFORMAT_INDEX8, SDL_TEXTUREACCESS_STREAMING, width, height);
    //SDL_Palette* palette = display->screen->format->palette;
    //display->colors = palette->colors;
    //display->ncolors = palette->ncolors;
    //SDL_Surface* temp = SDL_LoadBMP("title.bmp");
    //display->back = SDL_DisplayFormat(temp);
    //SDL_FreeSurface(temp);
    //updateDisplay(display);
    //SDL_Delay(2000); // Show splash screen for 2 seconds
    // Clear the back buffer with the first color in the color palette (black)
    //SDL_FillRect(display->back, NULL, 0x00); 

    return display;
}

void updateDisplay(Display* display, GPU* gpu)
{
    SDL_UpdateTexture(display->back, NULL, gpu->pixels, gpu->pitch);
    SDL_RenderClear(display->renderer);
    SDL_RenderCopy(display->renderer, display->back, NULL, NULL);
    SDL_RenderPresent(display->renderer);
}

/*
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
*/
void quitDisplay(Display* display)
{
    display->window = NULL;
    display->back = NULL;
    free(display);
    display = NULL;
    SDL_Quit();
}
