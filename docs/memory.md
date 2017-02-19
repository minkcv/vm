# Memory and Code
The vm addresses memory by segment and offset. There are 256 segments each with 256 bytes. Addresses are denoted as `segment.offset` ex: `64.2`. The bytes in memory are unsigned.

Code is stored separately, not in addressable memory. Jump instructions use a segment and an offset similar to memory addressing to have 64K of addressable instructions. You can write more instructions than this, you just can't jump to them, and the included assembler will have undefined behavior for labels after the 65535th instruction.

## Memory Map
- Segments 0 through 63 (inclusive) are general purpose RAM for your program.
- Segments 64 through 127 (inclusive) are read by the graphics processing unit (GPU) and written to by the input processing unit (IPU).
- Segments 128 through 255 (inclusive) are for program resources. These are typically referred to as ROM in classic systems.

### GPU and IPU Map
- `64.0` through `80.15` (inclusive) are for 256 sprite attributes. Each sprite attribute is 16 bytes long.
- `80.16` is the background color of the screen.
- `80.17` contains flags to change the state of the GPU.
- `127.0` through `127.1` (inclusive) are the 16 bits indicating which joystick buttons are pressed. There are 8 buttons for each of the 2 joysticks.
