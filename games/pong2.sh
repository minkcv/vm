#!/bin/bash
rm pong2/pong2.bin &> /dev/null
rm pong2/pong2.asm &> /dev/null
cd compiler
make &> /dev/null
cd ..
make &> /dev/null
./compiler/compiler -f pong2/pong2.vapor &> /dev/null
./assembler -f pong2/pong2.asm &> /dev/null
./main -f pong2/pong2.bin -s 2 &> /dev/null

