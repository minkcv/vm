# Sprites
Sprites are composed of a sprite attribute and sprite data. The attribute describes to the GPU how (and if) it should render the sprite. The attribute also contains the memory address of the sprite data. The sprite data would typically be located in the 128 through 255 (inclusive) segment range but doesn't have to be. You could generate your own sprites on the fly in RAM (segments 0 - 63). Sprites are drawn in the order that their sprite attributes are in memory. The sprite attribute at address `79.240` is drawn last (topmost).

#### Colors
The sprite attribute defines 4 colors for the sprite to use. The sprite data can then use only 2 bits to represent each pixel. This is designed to reduce memory usage. Sprites have transparency via the sprite attribute flag to treat the fourth color as alpha. Sprites with more than 4 colors can be achieved by overlaying multiple sprites in the same place and using transparency on the upper ones, though this uses more memory.

A sprite will not be rendered if any part of it exceeds the boundaries of the screen. The GPU does not change the active flag of the sprite if this happens.

#### Sprite Attribute
- Byte 0 is a set of bit flags about the sprite:
    - Bit 7 (MSB): 1 when the sprite should be rendered
    - Bit 6: 1 to flip the sprite horizontally
    - Bit 5: 1 to flip the sprite vertically
    - Bit 4: 1 to treat the fourth color in the palette as alpha (transparency)
    - Bit 3: Unused
    - Bit 2: Unused
    - Bit 1: Unused
    - Bit 0 (LSB): Unused
- Byte 1 is the x position of the sprite
- Byte 2 is the y position of the sprite (top left xy coordinate system)
- Byte 3 is the width of the sprite which must be a multiple of 4
- Byte 4 is the height of the sprite which must be a multiple of 4
- Byte 5 is the segment address of the sprite data in memory
- Byte 6 is the offset address of the sprite data in memory
- Bytes 7-10 are 4 colors from the full 256 color palette that the sprite data maps into
- Bytes 11-15 are currently unused
