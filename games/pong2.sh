#!/bin/bash
rm pong2/pong2.bin &> /dev/null
rm pong2/pong2.vaporbin &> /dev/null
rm pong2/pong2.asm &> /dev/null
cd ../compiler
make &> /dev/null
cd ..
make &> /dev/null
./compiler/compiler -f games/pong2/pong2.vapor &> /dev/null
./assembler -f games/pong2/pong2.asm &> /dev/null
./main -f games/pong2/pong2.vaporbin -s 2 &> /dev/null

