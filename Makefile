CC = g++
CPPFLAGS=-I. -fno-tree-vectorize
HEADERS=benchmark.h fwt_kernels.h interval.h pentagon_dense.h pentagon_fwt.h pentagon.h pentagon_stl.h timer.h pentagon_bp.h
SOURCES=benchmark

DEBUG ?= 0

ifeq ($(DEBUG),0)
	CPPFLAGS += -O2
else
	CPPFLAGS += -g3 -DDEBUG
endif

all: benchmark

benchmark: benchmark.cpp $(HEADERS)
	$(CC) -o benchmark benchmark.cpp

run:
	./benchmark rand 16 5 4

erik:
	c++ -o test.o -c test.cpp
	c++ -o test test.o
	./test

clean:
	rm benchmark
