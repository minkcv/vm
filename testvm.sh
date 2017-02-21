#/bin/bash
./assembler -f test.asm &> /dev/null
./main -f test.bin -r test.bin &> /dev/null
