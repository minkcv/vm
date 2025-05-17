#!/bin/bash
i=0

while [ $i -lt 80 ]
do
    echo -e '\n' >> pad80.bin
    i=$(( $i + 2 ))
done

