# Joysticks and Input
The vm supports 2 joysticks, each with 8 buttons. The buttons can either be pressed (1) or not pressed (0). The button states are located in a byte at memory address `127.0` for the first joystick and `127.1` for the second joystick. See `sample.md` for example code to read a button.

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
