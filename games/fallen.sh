#!/bin/bash
rm fallen/fallen.bin &> /dev/null
rm fallen/fallen.vaporbin &> /dev/null
rm fallen/fallen.asm &> /dev/null
rm fallen/fallen.rom &> /dev/null
cd ../compiler
make &> /dev/null
cd ..
make &> /dev/null
./convertsprite games/fallen/wall_corner.png &> /dev/null # 256 bytes
# 256 bytes of rom used
./convertsprite games/fallen/wall_horizontal.png &> /dev/null # 128 bytes
./convertsprite games/fallen/wall_vertical.png &> /dev/null # 128 bytes
# 512 bytes of rom used
./convertsprite games/fallen/door_horizontal.png &> /dev/null # 128 bytes
./convertsprite games/fallen/door_vertical.png &> /dev/null # 128 bytes
# 768 bytes of rom used
./convertsprite games/fallen/tiles.png &> /dev/null # 16384 bytes
# 17152 bytes of rom used
./convertsprite games/fallen/maptiles.png &> /dev/null # 256 bytes
# 17408 bytes of rom used
cat games/fallen/wall_corner.spr >> games/fallen/fallen.rom
cat games/fallen/wall_horizontal.spr >> games/fallen/fallen.rom
cat games/fallen/wall_vertical.spr >> games/fallen/fallen.rom
cat games/fallen/door_horizontal.spr >> games/fallen/fallen.rom
cat games/fallen/door_vertical.spr >> games/fallen/fallen.rom
cat games/fallen/tiles.spr >> games/fallen/fallen.rom
cat games/fallen/maptiles.spr >> games/fallen/fallen.rom
cd games/fallen
python map.py &> /dev/null # 2304 bytes
cd ../..
cat games/fallen/mapdata >> games/fallen/fallen.rom
# 19712 bytes of rom used, start of segment 77 from rom start . (segment 205 from 0)
./convertsprite games/fallen/player.png &> /dev/null # 64 bytes
cat games/fallen/player.spr >> games/fallen/fallen.rom
# 19778 bytes used
./compiler/compiler -f games/fallen/fallen.vapor &> /dev/null
./assembler -f games/fallen/fallen.asm -r games/fallen/fallen.rom
./main -f games/fallen/fallen.vaporbin -s 2 &> /dev/null

