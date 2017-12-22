#!/bin/sh
i=1;
while [ -f "./tests/test$i.src" ]; do
    echo "Compiling test $i"
    ./compiler -f tests/test$i.src
    echo "Test $i source:"
    cat ./tests/test$i.src
    echo "Test $i assembly:"
    cat ./tests/test$i.asm
    i=$((i+1))
done

