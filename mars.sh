#/bin/bash
make &> /dev/null
cd mars
../assembler -f mars.asm &> /dev/null
../convertsprite astronaut.png &> /dev/null
../convertsprite stars.png &> /dev/null
../convertsprite rock.png &> /dev/null
cat astronaut.spr > mars.rom
cat stars.spr >> mars.rom
cat rock.spr >> mars.rom
cd ..
./main -f mars/mars.bin -r mars/mars.rom -s 2 &> /dev/null
