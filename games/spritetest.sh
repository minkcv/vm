#!/bin/bash
rm spritetest/spritetest.asm &> /dev/null
rm spritetest/spritetest.bin &> /dev/null
rm spritetest/spritetest.rom &> /dev/null
rm spritetest/*.spr &> /dev/null
cd ../compiler
make &> /dev/null
cd ..
make &> /dev/null
./compiler/compiler -f games/spritetest/spritetest.vapor &> /dev/null
./convertsprite games/spritetest/test1.png &> /dev/null
./convertsprite games/spritetest/test2.png &> /dev/null
./convertsprite games/spritetest/test3.png &> /dev/null
cat games/spritetest/test1.spr >> games/spritetest/spritetest.rom
cat games/spritetest/test2.spr >> games/spritetest/spritetest.rom
cat games/spritetest/test3.spr >> games/spritetest/spritetest.rom
./assembler -f games/spritetest/spritetest.asm &> /dev/null
# ./main -f games/spritetest/spritetest.bin -r games/spritetest/spritetest.rom -s 2 &> /dev/null
