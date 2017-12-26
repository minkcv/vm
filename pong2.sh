#!/bin/sh
cd compiler
make &> /dev/null
cd ..
make
./compiler/compiler -f pong2/pong2.src &> /dev/null
./assembler -f pong2/pong2.asm &> /dev/null
./main -f pong2/pong2.bin -s 2 &> /dev/null

