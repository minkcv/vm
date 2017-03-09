#/bin/bash
make &> /dev/null
cd testgame
../assembler -f test.asm &> /dev/null
../convertsprite testsprite.png &> /dev/null
cd ..
./main -f testgame/test.bin -r testgame/testsprite.spr -s 2 &> /dev/null
