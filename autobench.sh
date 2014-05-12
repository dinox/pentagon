#!/bin/bash

for size in 512 1024 2048 4096;
do
    TT="-DUALL=16 -DL1_SIZE=$size"
    echo $TT
    make CPPFLAGS="$TT" AVX=1
    ./benchmark benchfile b4096
done
