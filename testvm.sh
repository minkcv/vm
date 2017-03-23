#/bin/bash
make &> /dev/null
cd testgame
../assembler -f test.asm &> /dev/null
../convertsprite astronaut.png &> /dev/null
../convertsprite stars.png &> /dev/null
../convertsprite rock.png &> /dev/null
cat astronaut.spr > test.rom
cat stars.spr >> test.rom
cat rock.spr >> test.rom
cd ..
./main -f testgame/test.bin -r testgame/test.rom -s 2 &> /dev/null
