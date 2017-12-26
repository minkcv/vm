#!/bin/bash
cd compiler
make &> /dev/null
cd ..
make &> /dev/null
./compiler/compiler -f tetris/tetris.vapor &> /dev/null
./assembler -f tetris/tetris.asm &> /dev/null
./main -f tetris/tetris.bin -s 2 &> /dev/null

