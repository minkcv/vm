#/bin/bash
make &> /dev/null
cd pong
../assembler -f pong &> /dev/null
cd ..
./main -f pong/pong.bin -s 2 &> /dev/null
