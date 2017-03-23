#ifndef JOYSTICK_H_
#define JOYSTICK_H_

#include <SDL.h>

typedef struct JOYSTICK JOYSTICK;

struct JOYSTICK
{
    SDLKey buttons[NUM_JOYSTICK_BUTTONS]; // Up, Down, Left, Right, A, B, C, D
    uint8_t keysAsBits; // LSB = Up, MSB = D. 1 = pressed, 0 = not pressed
};

#endif
