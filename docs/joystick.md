# Joysticks and Input
The vm supports 2 joysticks, each with 8 buttons. The buttons can either be pressed (1) or not pressed (0). The button states are located in a byte at memory address `127.0` for the first joystick and `127.1` for the second joystick.

### Button Mapping
#### Joystick 1
- Bit 7 (MSB): Return / Enter
- Bit 6: Right Shift
- Bit 5: Right Alt
- Bit 4: Right Control
- Bit 3: Right Arrow Key
- Bit 2: Left Arrow Key
- Bit 1: Down Arrow Key
- Bit 0 (LSB): Up Arrow Key

#### Joystick 2
- Bit 7 (MSB): Tab
- Bit 6: Left Shift
- Bit 5: Left Alt
- Bit 4: Left Control
- Bit 3: D
- Bit 2: A
- Bit 1: S
- Bit 0 (LSB): W

### Example Assembly

    LRC r0 #127 ; segment of joysticks
    LRC r1 #0 ; offset of first joystick
    LDR r2 r0 r1 ; load r2 with the button states of joystick 1
    LRC r3 #2 ; 0000 0010 down arrow
    AND r2 r2 r3 ; binary and the button states with the down arrow
    CMP r4 r2 r3 ; compare r2 with 0000 0010. will put 1 in r4 if they are equal
    ; jump to downpressed label if down arrow was pressed
    JEQ r4 @downpressed
