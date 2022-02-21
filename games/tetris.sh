#!/bin/bash
rm tetris/tetris.asm &> /dev/null
rm tetris/tetris.bin &> /dev/null
rm tetris/tetris.vaporbin &> /dev/null
cd ../compiler
make &> /dev/null
cd ..
make &> /dev/null
./compiler/compiler -f games/tetris/tetris.vapor &> /dev/null
./assembler -f games/tetris/tetris.asm &> /dev/null
./main -f games/tetris/tetris.vaporbin -s 2 &> /dev/null

