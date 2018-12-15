#include "ipu.h"

IPU* createIPU()
{
    IPU* ipu = (IPU*)malloc(sizeof(IPU));
    JOYSTICK* js1 = (JOYSTICK*)malloc(sizeof(JOYSTICK));
    js1->buttons[0] = SDLK_UP;
    js1->buttons[1] = SDLK_DOWN;
    js1->buttons[2] = SDLK_LEFT;
    js1->buttons[3] = SDLK_RIGHT;
    js1->buttons[4] = SDLK_RCTRL; // Right control
    js1->buttons[5] = SDLK_RALT; // Right alt
    js1->buttons[6] = SDLK_RSHIFT; // Righ shift
    js1->buttons[7] = SDLK_RETURN; // AKA Enter
    js1->keysAsBits = 0;
    ipu->js1 = js1;

    JOYSTICK* js2 = (JOYSTICK*)malloc(sizeof(JOYSTICK));
    js2->buttons[0] = SDLK_w;
    js2->buttons[1] = SDLK_s;
    js2->buttons[2] = SDLK_a;
    js2->buttons[3] = SDLK_d;
    js2->buttons[4] = SDLK_LCTRL; // Left control
    js2->buttons[5] = SDLK_LALT; // Left alt
    js2->buttons[6] = SDLK_LSHIFT; // Left shift
    js2->buttons[7] = SDLK_TAB;
    js2->keysAsBits = 0;
    ipu->js2 = js2;

    return ipu;
}

void destroyIPU(IPU* ipu)
{
    free(ipu->js1);
    ipu->js1 = NULL;
    free(ipu->js2);
    ipu->js2 = NULL;
}

void updateIPU(IPU* ipu, SDL_KeyboardEvent kEvent, 
        uint8_t memory[MEMORY_SEGMENT_COUNT][MEMORY_SEGMENT_SIZE])
{
    updateJoystick(ipu->js1, kEvent);
    updateJoystick(ipu->js2, kEvent);
    memory[JOYSTICK_SEG][JOYSTICK_1_OFFSET] = ipu->js1->keysAsBits;
    memory[JOYSTICK_SEG][JOYSTICK_2_OFFSET] = ipu->js2->keysAsBits;
}

void updateJoystick(JOYSTICK* js, SDL_KeyboardEvent kEvent)
{
    int i;
    uint8_t bit = 1;
    for (i = 0; i < NUM_JOYSTICK_BUTTONS; i++)
    {
        if (kEvent.keysym.sym == js->buttons[i])
        {
            if (kEvent.type == SDL_KEYDOWN)
            {
                // Leave the other bits but ensure the ith bit is 1
                js->keysAsBits |= (bit << i);
            }
            else if (kEvent.type == SDL_KEYUP)
            {
                // Leave the other bits but ensure the ith bit is 0
                js->keysAsBits &= (~(bit << i)); // Bitwise inversion
            }
        }
    }
}

