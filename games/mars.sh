#!/bin/bash
cd ..
make &> /dev/null
cd games/mars
rm *.spr
rm mars.rom
rm mars.bin
../../assembler -f mars.asm &> /dev/null
../../convertsprite astronaut.png &> /dev/null
../../convertsprite stars.png &> /dev/null
../../convertsprite rock.png &> /dev/null
cat astronaut.spr > mars.rom
cat stars.spr >> mars.rom
cat rock.spr >> mars.rom
cd ../..
./main -f games/mars/mars.bin -r games/mars/mars.rom -s 2 &> /dev/null

