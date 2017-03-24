# Getting Started
First, make sure you have built the main program and the assembler via the build instructions in the `README.md`.

#### Assembling
Write your assembly code in a plain text file and give it the extension `.asm`. Check out the `assembly.md` document to get started writing programs. Also see `sample.md` for some useful sample code. Then run

    ./assembler -f myprogram.asm

Which will produce a `myprogram.bin` file.

#### Packing data into a ROM
An included program called `convertsprite` can convert png images into the internal sprite format. The png images must use colors from the screen's 256 color palette as listed in `colorpalette.gpl` with the exception of a fully transparent pixel (alpha 0) which will cause the sprite color palette to use color 4 as transparency. The `colorpalette.gpl` file can be loaded into GIMP to make png creation easier.
Use `convertsprite` as follows:

    ./convertsprite mysprite.png

The program will output information about the sprite's size and color palette as well as a `mysprite.spr` file.
Put `.spr` files together like as follows to have them all in one ROM file:

    cat player.spr >> myrom.rom
    cat enemy.spr >> myrom.rom

Any other data that you want to put in the ROM will have to be done yourself.
#### Running
Run the binary with

    ./main -f myprogram.bin

Optionally add:
- `-s #`, where # is a number, if you want to scale the display up.
- `-r myrom.rom` to load up to 32768 bytes into the ROM section of memory.
- `&> /dev/null` at the end to suppress print statements.

Consider creating a shell script for your program that is similar to the included `mars.sh` script.
