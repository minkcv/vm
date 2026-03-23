#!/bin/bash
rm buttontest/buttontest.bin &> /dev/null
rm buttontest/buttontest.vaporbin &> /dev/null
rm buttontest/buttontest.asm &> /dev/null
cd ../compiler
make &> /dev/null
cd ..
make &> /dev/null
./compiler/compiler -f games/buttontest/buttontest.vapor
./assembler -f games/buttontest/buttontest.asm
./main -f games/buttontest/buttontest.vaporbin -s 2

