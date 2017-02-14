#ifndef JOYSTICK_H_
#define JOYSTICK_H_

#include <SDL.h>

typedef struct JOYSTICK JOYSTICK;

struct JOYSTICK
{
    SDLKey up;
    SDLKey down;
    SDLKey left;
    SDLKey right;
    SDLKey a;
    SDLKey b;
    SDLKey c;
    SDLKey d;
    uint8_t keysAsBits; // MSB = up LSB = d
};

#endif
