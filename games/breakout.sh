#!/bin/bash
rm breakout/breakout.bin &> /dev/null
rm breakout/breakout.vaporbin &> /dev/null
rm breakout/breakout.asm &> /dev/null
cd ../compiler
make &> /dev/null
cd ..
make &> /dev/null
./compiler/compiler -f games/breakout/breakout.vapor &> /dev/null
./assembler -f games/breakout/breakout.asm &> /dev/null
./main -f games/breakout/breakout.vaporbin -s 2 &> /dev/null

