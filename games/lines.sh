#!/bin/bash
rm lines/lines.bin &> /dev/null
rm lines/lines.asm &> /dev/null
cd ../compiler
make &> /dev/null
cd ..
make &> /dev/null
./compiler/compiler -f games/lines/lines.vapor -l sa &> /dev/null
./assembler -f games/lines/lines.asm -r games/lines/model.bin &> /dev/null
./main -f games/lines/lines.bin -s 2 -d

