#!/bin/sh
i=1;
iformat=$(printf "%02d" $i)
while [ -f "./tests/test$iformat.src" ]; do
    echo "========================="
    echo "Compiling test $iformat"
    ./compiler -f tests/test$iformat.src
    echo "Test $iformat source:"
    cat ./tests/test$iformat.src
    echo "Test $iformat assembly:"
    cat ./tests/test$iformat.asm
    i=$((i+1))
    iformat=$(printf "%02d" $i)
done

