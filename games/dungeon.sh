#!/bin/bash
rm dungeon/dungeon.bin &> /dev/null
rm dungeon/dungeon.asm &> /dev/null
rm dungeon/dungeon.rom &> /dev/null
cd ../compiler
make &> /dev/null
cd ..
make &> /dev/null
./convertsprite games/dungeon/wall_corner.png # 256 bytes
# 256 bytes of rom used
./convertsprite games/dungeon/wall_horizontal.png # 128 bytes
./convertsprite games/dungeon/wall_vertical.png # 128 bytes
# 512 bytes of rom used
./convertsprite games/dungeon/door_horizontal.png # 128 bytes
./convertsprite games/dungeon/door_vertical.png # 128 bytes
# 768 bytes of rom used
./convertsprite games/dungeon/tiles.png # 2048 bytes
# 2816 bytes of rom used
./convertsprite games/dungeon/maptiles.png # 256 bytes
# 3072 bytes of rom used
cat games/dungeon/wall_corner.spr >> games/dungeon/dungeon.rom
cat games/dungeon/wall_horizontal.spr >> games/dungeon/dungeon.rom
cat games/dungeon/wall_vertical.spr >> games/dungeon/dungeon.rom
cat games/dungeon/door_horizontal.spr >> games/dungeon/dungeon.rom
cat games/dungeon/door_vertical.spr >> games/dungeon/dungeon.rom
cat games/dungeon/tiles.spr >> games/dungeon/dungeon.rom
cat games/dungeon/maptiles.spr >> games/dungeon/dungeon.rom
cd games/dungeon
python map.py # 2304 bytes
# 5376 bytes of rom used
cd ../..
cat games/dungeon/mapdata >> games/dungeon/dungeon.rom
./compiler/compiler -f games/dungeon/dungeon.vapor -l sa
./assembler -f games/dungeon/dungeon.asm
