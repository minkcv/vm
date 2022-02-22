#!/bin/bash
rm paint/paint.asm &> /dev/null
rm paint/paint.bin &> /dev/null
rm paint/paint.vaporbin &> /dev/null
rm paint/paint.rom &> /dev/null
rm paint/*.spr &> /dev/null
cd ../compiler
make &> /dev/null
cd ..
make &> /dev/null
./compiler/compiler -f games/paint/paint.vapor &> /dev/null
./convertsprite games/paint/abc.png &> /dev/null
./convertsprite games/paint/123.png &> /dev/null
./convertsprite games/paint/sym.png &> /dev/null
./convertsprite games/paint/shapes.png &> /dev/null
cat games/paint/abc.spr >> games/paint/paint.rom
cat games/paint/123.spr >> games/paint/paint.rom
cat games/paint/sym.spr >> games/paint/paint.rom
cat games/paint/shapes.spr >> games/paint/paint.rom
./assembler -f games/paint/paint.asm -r games/paint/paint.rom &> /dev/null
./main -f games/paint/paint.vaporbin -s 2 &> /dev/null

