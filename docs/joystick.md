# Joysticks and Input
The vm supports 2 joysticks, each with 8 buttons. The buttons can either be pressed (1) or not pressed (0). The button states are located in a byte at memory address `127.0` for the first joystick and `127.1` for the second joystick. See `sample.md` for example code to read a button.

### Button Mapping for Keyboard
#### Joystick 1
- Bit 7 (MSB): H
- Bit 6: Y
- Bit 5: U
- Bit 4: O
- Bit 3: Right Arrow Key
- Bit 2: Left Arrow Key
- Bit 1: Down Arrow Key
- Bit 0 (LSB): Up Arrow Key

#### Joystick 2
- Bit 7 (MSB): F
- Bit 6: R
- Bit 5: E
- Bit 4: Q
- Bit 3: D
- Bit 2: A
- Bit 1: S
- Bit 0 (LSB): W
