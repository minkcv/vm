# Memory
The vm addresses memory by segment and offset. There are 256 segments each with 256 bytes. Addresses in this documentation are denoted as `segment.offset` ex: `64.2`. The bytes in memory are unsigned and store a value between 0 and 255.

## Memory Map
- Segments 0 through 63 (inclusive) are general purpose RAM for your program.
- Segments 64 through 127 (inclusive) are read and written to by the graphics processing unit (GPU) and input processing unit (IPU).
- Segments 128 through 255 (inclusive) are for static program resources. These are typically referred to as ROM in classic systems.

### GPU and IPU Map
- `64.0` through `80.15` (inclusive) are for 256 sprite attributes. Each sprite attribute is 16 bytes long.
- `127.0` through `127.1` (inclusive) are the 16 bits indicating which joystick buttons are pressed. There are 8 buttons for each of the 2 joysticks.
- `127.16` is the background color of the screen stored as an index into the screen 256 color palette.
- `127.17` contains flags to change the state of the GPU and indicate the status of the GPU. (All 0 by default)
    - Bit 1: Toggled by the GPU when a screen draw is finished.
    - Bit 0 (LSB): Set to 1 to enable the GPU to render to the screen. Set to 0 to disable the GPU to render to the screen (leaves current image).
