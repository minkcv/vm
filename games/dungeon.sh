#!/bin/bash
rm dungeon/dungeon.bin &> /dev/null
rm dungeon/dungeon.asm &> /dev/null
cd ../compiler
make &> /dev/null
cd ..
make &> /dev/null
./compiler/compiler -f games/dungeon/dungeon.vapor -l s
./assembler -f games/dungeon/dungeon.asm
