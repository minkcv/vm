#/bin/bash
make &> /dev/null
./assembler -f test.asm &> /dev/null
./convertsprite testsprite.png &> /dev/null
./main -f test.bin -r testsprite.spr -s 2 &> /dev/null
