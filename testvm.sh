#/bin/bash
./assembler -f test.asm &> /dev/null
./main -f test.bin &> /dev/null
