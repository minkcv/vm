#!/bin/bash
rm paint/paint.asm &> /dev/null
rm paint/paint.bin &> /dev/null
rm paint/paint.rom &> /dev/null
rm paint/*.spr &> /dev/null
cd ../compiler
make &> /dev/null
cd ..
make &> /dev/null
./compiler/compiler -f games/paint/paint.vapor &> /dev/null
./convertsprite games/paint/abc.png &> /dev/null
cat games/paint/abc.spr >> games/paint/paint.rom
./assembler -f games/paint/paint.asm &> /dev/null
./main -f games/paint/paint.bin -r games/paint/paint.rom -s 2 &> /dev/null

