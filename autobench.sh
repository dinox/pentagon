#!/bin/bash

for size in 512 1024 2048 4096;
do
    TT="-DUALL=16 -DL1_SIZE=$size"
    echo $TT
    g++ -I. -fno-tree-vectorize -funroll-loops -O3 -o benchmark benchmark.cpp $TT
    ./benchmark benchfile b4096
done
