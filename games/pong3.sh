#!/bin/bash
cd ..
make &> /dev/null
cd games/pong3
../../assembler -f pong3.asm &> /dev/null
cd ../..
./main -f games/pong3/pong3.vaporbin -s 2 &> /dev/null

