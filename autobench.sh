#!/bin/bash

for size in 2 4 8 16 32;
do
    FLAGS="-DUALL=$size -DL1_SIZE=$(($size*128))"
    make -s clean
    make SSE=1 CPPFLAGS="$FLAGS"
    ./benchmark benchfile b4096
done
