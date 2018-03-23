#!/bin/bash
rm invaders/invaders.asm
rm invaders/invaders.bin
rm invaders/invaders.rom
cd ../compiler
make
cd ..
make
cd games/invaders
../../convertsprite player.png
../../convertsprite bullet.png
../../convertsprite enemy.png
cat player.spr >> invaders.rom
cat bullet.spr >> invaders.rom
cat enemy.spr >> invaders.rom
cd ../..
./compiler/compiler -f games/invaders/invaders.vapor -l sa
./assembler -f games/invaders/invaders.asm
./main -f games/invaders/invaders.bin -r games/invaders/invaders.rom -d

