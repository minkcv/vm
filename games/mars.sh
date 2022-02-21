#!/bin/bash
cd ..
make &> /dev/null
cd games/mars
rm *.spr
rm mars.rom
rm mars.bin
../../convertsprite astronaut.png &> /dev/null
../../convertsprite stars.png &> /dev/null
../../convertsprite rock.png &> /dev/null
cat astronaut.spr > mars.rom
cat stars.spr >> mars.rom
cat rock.spr >> mars.rom
../../assembler -f mars.asm -r mars.rom &> /dev/null
cd ../..
./main -f games/mars/mars.bin -s 2 &> /dev/null

