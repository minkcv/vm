# Memory

The vm addresses memory by segment and offset. There are 256 segments each with 256 bytes. Addresses are denoted as `segment.offset` ex: `64.2`. The bytes in memory are unsigned.

## Memory Map
- Segments 0 through 63 (inclusive) are general purpose RAM for your program.
- Segments 64 through 127 (inclusive) are read by the graphics processing unit (GPU) and written to by the input processing unit (IPU).
- Segments 128 through 255 (inclusive) are for program resources. These are typically referred to as ROM in classic systems.

### GPU and IPU Map
- `64.0` through `80.15` (inclusive) are for 256 sprite attributes. Each sprite attribute is 16 bytes long.
- `80.16` is the background color of the screen.

#### Sprite Attribute
A sprite attribute stores information about how a sprite should be displayed on the screen. It includes an address that points to the data of the sprite. See `sprite.md` to understand how the sprite colors are stored.
- Byte 0 is a set of bit flags about the sprite:
    - Bit 7: 1 when the sprite should be rendered
    - Bit 6: 1 to flip the sprite horizontally
    - Bit 5: 1 to flip the sprite vertically
    - Bit 4: 1 to treat the fourth color in the palette as alpha (transparency)
    - Bit 3: Unused
    - Bit 2: Unused
    - Bit 1: Unused
    - Bit 0: Unused
- Byte 1 is the x position of the sprite
- Byte 2 is the y position of the sprite (top left xy coordinate system)
- Byte 3 is the width of the sprite which must be a multiple of 4
- Byte 4 is the height of the sprite which must be a multiple of 4
- Byte 5 is the segment address of the sprite data in memory
- Byte 6 is the offset address of the sprite data in memory
- Bytes 7-11 are colors from the full 256 color palette that the sprite data maps into
- Bytes 12-15 are currently unused

#### IPU
TODO
