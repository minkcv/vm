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
./convertsprite games/paint/a.png &> /dev/null
./convertsprite games/paint/b.png &> /dev/null
./convertsprite games/paint/c.png &> /dev/null
./convertsprite games/paint/d.png &> /dev/null
./convertsprite games/paint/e.png &> /dev/null
./convertsprite games/paint/f.png &> /dev/null
./convertsprite games/paint/g.png &> /dev/null
./convertsprite games/paint/h.png &> /dev/null
./convertsprite games/paint/i.png &> /dev/null
./convertsprite games/paint/j.png &> /dev/null
./convertsprite games/paint/k.png &> /dev/null
cat games/paint/*.spr >> games/paint/paint.rom
./assembler -f games/paint/paint.asm &> /dev/null
./main -f games/paint/paint.bin -r games/paint/paint.rom -s 2 &> /dev/null

