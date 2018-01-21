#include "display.h"

Display* createDisplay(int width, int height, int scale, int pitch)
{
    SDL_Init(SDL_INIT_VIDEO);

    Display* display = (Display*)malloc(sizeof(Display));
    display->width = width;
    display->height = height;
    display->scale = scale;
    display->pitch = pitch;
    display->window = SDL_CreateWindow("Vapor Spec", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 
            width * scale, height * scale, 0);
    display->renderer = SDL_CreateRenderer(display->window, -1, 0);
    display->back = SDL_CreateTexture(display->renderer, 
            SDL_PIXELFORMAT_RGB24, SDL_TEXTUREACCESS_STREAMING, width, height);
    SDL_Surface* splash = SDL_LoadBMP("title.bmp");
    SDL_Texture* splashTex = SDL_CreateTextureFromSurface(display->renderer, splash);
    SDL_RenderCopy(display->renderer, splashTex, NULL, NULL);
    SDL_RenderPresent(display->renderer);
    SDL_Delay(2000); // Show splash screen for 2 seconds
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

