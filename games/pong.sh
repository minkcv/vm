#!/bin/bash
cd ..
make &> /dev/null
cd games/pong
../../assembler -f pong.asm &> /dev/null
cd ../..
./main -f games/pong/pong.vaporbin -s 2 &> /dev/null

