# Sprites
Sprites are composed of a sprite attribute and sprite data. The attribute describes to the GPU how (and if) it should render the sprite. The attribute also contains the memory address of the sprite data. The sprite data would typically be located in the 128 through 255 (inclusive) segment range but doesn't have to be. You could generate your own sprites on the fly in RAM (segments 0 - 63) if you want.

The sprite attribute defines 4 colors for the sprite to use. The sprite data can then use only 2 bits to represent each pixel. This is necessary to reduce memory usage. Sprites with more than 4 colors can be achieved by overlaying multiple sprites in the same place and using transparency on the upper ones. This should be used sparingly though.
