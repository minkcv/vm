#!/bin/bash
rm descent/descent.asm
rm descent/descent.rom
rm descent/descent.vaporbin
cd descent
rm *.spr
../../convertsprite playerSheet.png # 640 bytes
../../convertsprite 8x8walls.png # 144 bytes
../../convertsprite 16x16blocks.png # 576 bytes (256, 256, 128)
../../convertsprite arrows.png # 128 bytes
../../convertsprite chain.png # 64 bytes
../../convertsprite planet.png # 768 bytes
../../convertsprite endtitle.png # 784
../../convertsprite endstatus.png # 192
../../convertsprite endmode.png # 132
../../convertsprite endcrew.png # 132
../../convertsprite statusdeceased.png # 240
../../convertsprite statussurvived.png # 240
../../convertsprite modestandard.png # 240
../../convertsprite modechallenge.png # 264
../../convertsprite crewcount.png # 180
../../convertsprite oxygen.png # 176
cat playerSheet.spr >> descent.rom
cat pad64.bin >> descent.rom # fill out the segment
cat pad64.bin >> descent.rom
cat 8x8walls.spr >> descent.rom
cat pad112.bin >> descent.rom
cat 16x16blocks.spr >> descent.rom
cat pad64.bin >> descent.rom
cat pad64.bin >> descent.rom
cat arrows.spr >> descent.rom
cat chain.spr >> descent.rom
cat pad64.bin >> descent.rom
cat planet.spr >> descent.rom
cat oxygen.spr >> descent.rom
cat pad80.bin >> descent.rom
cat endtitle.spr >> descent.rom
cat endstatus.spr >> descent.rom
cat endmode.spr >> descent.rom
cat endcrew.spr >> descent.rom
cat statusdeceased.spr >> descent.rom
cat statussurvived.spr >> descent.rom
cat modestandard.spr >> descent.rom
cat modechallenge.spr >> descent.rom
cat crewcount.spr >> descent.rom

cd ../../compiler
make &> /dev/null
cd ..
make &> /dev/null
./compiler/compiler -f games/descent/descent.vapor
./assembler -f games/descent/descent.asm -r games/descent/descent.rom
#./compiler/optimize.py -f games/descent/descent.asm
#./assembler -f games/descent/descent.opt.asm -r games/descent/descent.rom
./main -f games/descent/descent.vaporbin -s 4 -d

