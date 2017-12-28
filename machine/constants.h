#ifndef CONSTANTS_H_
#define CONSTANTS_H_

#define SCREEN_WIDTH 256
#define SCREEN_HEIGHT 192

// Instruction addressing also uses segments and offset
#define JUMP_SEGMENT_SIZE 256
// Number of cells in a Segment
#define MEMORY_SEGMENT_SIZE 256
// Number of segments
#define MEMORY_SEGMENT_COUNT 256
// 256*256 = 357604 Total memory cells

#define REGISTER_COUNT 16


// Number of sprites able to be rendered at once
// Memory 64.0 to 80.15 is for sprite attributes
#define NUM_SPRITES 256

// Number of bytes per sprite attribute
#define SPRITE_ATTR_LENGTH 16

// Segment of memory where sprites attributes start
#define SPRITE_ATTR_SEG 64

#define NUM_JOYSTICK_BUTTONS 8

// 127.0 and 127.1 are joystick 1 and 2 button states
#define JOYSTICK_SEG 127
#define JOYSTICK_1_OFFSET 0
#define JOYSTICK_2_OFFSET 1

// 127.16 is the background color (one byte)
#define BACK_COLOR_SEG 127
#define BACK_COLOR_OFFSET 16

// 127.17 are flags to control the GPU and view the status
#define GPU_FLAG_SEG 127
#define GPU_FLAG_OFFSET 17

// Number of colors in the color palette
#define NUM_COLORS 256

#define INSTRUCTIONS_PER_SECOND 500000
#endif

